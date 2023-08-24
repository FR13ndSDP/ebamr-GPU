#include "EBR.H"
#include "Reconstruction.H"
#include "Kernels.H"

using namespace amrex;

void EBR::scalar_dSdt(const amrex::MultiFab &S, const amrex::MultiFab &state, amrex::MultiFab &dSdt, amrex::Real dt, amrex::EBFluxRegister *fine, amrex::EBFluxRegister *current)
{
    BL_PROFILE("EBR::scalar_dSdt()");

    const auto dx = geom.CellSize();
    const auto dxinv = geom.InvCellSizeArray();
    const int ncomp = dSdt.nComp();

    auto const& fact = dynamic_cast<EBFArrayBoxFactory const&>(S.Factory());
    auto const& flags = fact.getMultiEBCellFlagFab();

    Parm const* lparm = d_parm;

    FArrayBox dm_as_fine(Box::TheUnitBox(), ncomp);

    GpuArray<FArrayBox,AMREX_SPACEDIM> flux;

    for (MFIter mfi(S, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const Box& bx = mfi.tilebox();

        const auto& flag = flags[mfi];

        if (flag.getType(bx) == FabType::covered) {
            dSdt[mfi].setVal<RunOn::Device>(0.0, bx , 0, ncomp);
        } else {
            // flux is used to store centroid flux needed for reflux
            // for flux_x in x direction is nodal, in other directions centroid
            // for flux_y in y ...
            for (int idim=0; idim < AMREX_SPACEDIM; ++idim) {
                flux[idim].resize(amrex::surroundingNodes(bx,idim),ncomp);
                flux[idim].setVal<RunOn::Device>(0.0);
            }

            auto const& sfab = S.array(mfi);
            auto const& statefab = state.array(mfi);
            auto const& dsdtfab = dSdt.array(mfi);
            auto const& fxfab = flux[0].array();
            auto const& fyfab = flux[1].array();
            auto const& fzfab = flux[2].array();

            // no cut cell around
            if (flag.getType(amrex::grow(bx,NUM_GROW)) == FabType::regular)
            {
                const Box& bxg = amrex::grow(bx,NUM_GROW);

                // f+ and f-
                FArrayBox fptmp(bxg, ncomp, The_Async_Arena());
                FArrayBox fmtmp(bxg, ncomp, The_Async_Arena());
                auto const& fp = fptmp.array();
                auto const& fm = fmtmp.array();

                // X-direction
                int cdir = 0;
                const Box& xflxbx = amrex::surroundingNodes(bx, cdir);

                ParallelFor<NTHREADS>(bxg, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    Real un = statefab(i,j,k,UMX)/statefab(i,j,k,URHO);
                    fp(i,j,k,n) = 0.5*(un+amrex::Math::abs(un))*sfab(i,j,k,n);
                    fm(i,j,k,n) = 0.5*(un-amrex::Math::abs(un))*sfab(i,j,k,n);
                });

                ParallelFor<NTHREADS>(xflxbx, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    reconstruction_x(i,j,k,n,fp,fm,fxfab,*lparm);
                });

                // Y-direction
                cdir = 1;
                const Box& yflxbx = amrex::surroundingNodes(bx, cdir);

                ParallelFor<NTHREADS>(bxg, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    Real un = statefab(i,j,k,UMY)/statefab(i,j,k,URHO);
                    fp(i,j,k,n) = 0.5*(un+amrex::Math::abs(un))*sfab(i,j,k,n);
                    fm(i,j,k,n) = 0.5*(un-amrex::Math::abs(un))*sfab(i,j,k,n);
                });

                ParallelFor<NTHREADS>(yflxbx, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    reconstruction_y(i,j,k,n,fp,fm,fyfab,*lparm);
                });

                // Z-direction
                cdir = 2;
                const Box& zflxbx = amrex::surroundingNodes(bx, cdir);

                ParallelFor<NTHREADS>(bxg, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    Real un = statefab(i,j,k,UMZ)/statefab(i,j,k,URHO);
                    fp(i,j,k,n) = 0.5*(un+amrex::Math::abs(un))*sfab(i,j,k,n);
                    fm(i,j,k,n) = 0.5*(un-amrex::Math::abs(un))*sfab(i,j,k,n);
                });

                ParallelFor<NTHREADS>(zflxbx, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    reconstruction_z(i,j,k,n,fp,fm,fzfab,*lparm);
                });

                ParallelFor<NTHREADS>(bx, ncomp,
                [=] AMREX_GPU_DEVICE (int i, int j, int k, int n) noexcept
                {
                    divop(i,j,k,n,dsdtfab,AMREX_D_DECL(fxfab, fyfab, fzfab), dxinv);
                });

#ifdef AMREX_USE_GPU
                // sync here to avoid out of if loop synchronize
                Gpu::streamSynchronize();
#endif
                if (do_reflux)
                {
                    if (current) {
                        current->FineAdd(mfi, {&flux[0], &flux[1], &flux[2]}, dx, dt, RunOn::Device);
                    }

                    if (fine) {
                        fine->CrseAdd(mfi, {&flux[0], &flux[1], &flux[2]}, dx, dt, RunOn::Device);
                    }
                }
            }
        }
    }
}