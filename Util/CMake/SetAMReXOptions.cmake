set(USE_XSDK_DEFAULTS OFF)
set(AMReX_SPACEDIM "${EBAMR_DIM}" CACHE STRING "Number of physical dimensions" FORCE)
set(AMReX_MPI ${OPTION_MPI})
set(AMReX_OMP ${OPTION_OPENMP})
set(AMReX_EB ON) 
set(AMReX_PARTICLES OFF)
set(AMReX_SUNDIALS ${OPTION_ENABLE_SUNDIALS})
set(AMReX_FORTRAN OFF)
set(AMReX_FORTRAN_INTERFACES OFF)
set(AMReX_PIC OFF)
set(AMReX_PRECISION "${EBAMR_PRECISION}" CACHE STRING "Floating point precision" FORCE)
set(AMReX_LINEAR_SOLVERS OFF)
set(AMReX_AMRDATA OFF)
set(AMReX_AMRLEVEL ON)
set(AMReX_HYPRE ${OPTION_ENABLE_HYPRE})
set(AMReX_FPE OFF)
set(AMReX_ASSERTIONS OFF)
set(AMReX_BASE_PROFILE OFF)
set(AMReX_TINY_PROFILE ${OPTION_ENABLE_TINY_PROFILE})
set(AMReX_TRACE_PROFILE OFF)
set(AMReX_MEM_PROFILE OFF)
set(AMReX_COMM_PROFILE OFF)
set(AMReX_PROFPARSER OFF)
set(AMReX_ACC OFF)
set(AMReX_INSTALL OFF) 
set(AMReX_HDF5 ${OPTION_ENABLE_HDF5})

if(OPTION_CUDA)
  set(AMReX_GPU_BACKEND CUDA CACHE STRING "AMReX GPU type" FORCE)
elseif(OPTION_HIP)
  set(AMReX_GPU_BACKEND HIP CACHE STRING "AMReX GPU type" FORCE)
elseif(OPTION_SYCL)
  set(AMReX_GPU_BACKEND SYCL CACHE STRING "AMReX GPU type" FORCE)
else()
  set(AMReX_GPU_BACKEND NONE CACHE STRING "AMReX GPU type" FORCE)
endif()