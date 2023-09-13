#include <Constants.H>
#include <IndexDefines.H>
#include <AMReX_GpuContainers.H>

using amrex::Real;

void Parm::Initialize ()
{
    Rg = Ru/eos_m;
    cv = Rg / (eos_gamma-Real(1.0));
    cp = eos_gamma * cv;
    kOverMu = cp/Pr;
#ifdef CHEM
    // kg/mol
    amrex::Gpu::HostVector<Real> h_mw = {2.016 * 1e-3,   /*H2 */
                                               31.998 * 1e-3,   /*O2 */
                                               18.015 * 1e-3,   /*H2O */
                                               1.008 * 1e-3,   /*H */
                                               15.999 * 1e-3,   /*O */
                                               17.007 * 1e-3,   /*OH */
                                               33.006 * 1e-3,   /*HO2 */
                                               34.014 * 1e-3,   /*H2O2 */
                                               28.014 * 1e-3}; /*N2 */

    amrex::Gpu::HostVector<Real> h_H2_lo = {3.29812,      8.24944e-4,    -81.43015e-8,
                                    -9.47543e-11, 413.48718e-15, -0.01013e+5,
                                    -3.29409};
    amrex::Gpu::HostVector<Real> h_H2_hi = {2.99142,      7.00064e-4,  -5.63383e-8,
                                    -0.92316e-11, 1.58275e-15, -0.00835e+5,
                                    -1.35511};
    amrex::Gpu::HostVector<Real> h_O2_lo = {3.21294,       11.27486e-4,   -57.56150e-8,
                                    131.38770e-11, -876.8554e-15, -0.01005e+5,
                                    6.03474};
    amrex::Gpu::HostVector<Real> h_O2_hi = {3.69758,     6.13520e-4,   -12.58842e-8,
                                    1.77528e-11, -1.13644e-15, -0.01234e+5,
                                    3.18917};
    amrex::Gpu::HostVector<Real> h_H2O_lo = {3.38684,      34.74982e-4,   -635.4696e-8,
                                    696.8581e-11, -2506.588e-15, -0.30208e+5,
                                    2.59023};
    amrex::Gpu::HostVector<Real> h_H2O_hi = {2.67215,      30.56293e-4,  -87.30260e-8,
                                    12.00996e-11, -6.39162e-15, -0.29899e+5,
                                    6.86282};
    amrex::Gpu::HostVector<Real> h_H_lo = {2.5, 0.0, 0.0, 0.0, 0.0, 0.25472e+5, -0.46012};
    amrex::Gpu::HostVector<Real> h_H_hi = {2.5, 0.0, 0.0, 0.0, 0.0, 0.25472e+5, -0.46012};
    amrex::Gpu::HostVector<Real> h_O_lo = {2.94643,      -16.3817e-4,   242.10320e-8,
                                -160.284e-11, 389.06961e-15, 0.29148e+5,
                                2.96399};
    amrex::Gpu::HostVector<Real> h_O_hi = {2.54206,     -0.27551e-4,  -0.31028e-8,
                                0.45511e-11, -0.43681e-15, 0.29231e+5,
                                4.92031};
    amrex::Gpu::HostVector<Real> h_OH_lo = {3.63727,       1.85091e-4,    -167.6165e-8,
                                    238.72031e-11, -843.1442e-15, 0.03607e+5,
                                    1.35886};
    amrex::Gpu::HostVector<Real> h_OH_hi = {2.88273,     10.13974e-4,  -22.76877e-8,
                                    2.17468e-11, -0.51263e-15, 0.03887e+5,
                                    5.59571};
    amrex::Gpu::HostVector<Real> h_HO2_lo = {2.97996,       49.96697e-4,   -379.0997e-8,
                                    235.41919e-11, -808.9024e-15, 0.00176e+5,
                                    9.22272};
    amrex::Gpu::HostVector<Real> h_HO2_hi = {4.07219,     21.31296e-4,  -53.08145e-8,
                                    6.11227e-11, -2.84116e-15, -0.00158e+5,
                                    3.47603};
    amrex::Gpu::HostVector<Real> h_H2O2_lo = {3.38875,        65.69226e-4,  -14.85013e-8,
                                    -462.58060e-11, 2471.515e-15, -0.17663e+5,
                                    6.78536};
    amrex::Gpu::HostVector<Real> h_H2O2_hi = {4.57317,      43.36136e-4,   -147.4689e-8,
                                    23.48904e-11, -14.31654e-15, -0.18007e+5,
                                    0.50114};
    amrex::Gpu::HostVector<Real> h_N2_lo = {3.29868,       14.08240e-4,   -396.3222e-8,
                                    564.15149e-11, -2444.855e-15, -0.01021e+5,
                                    3.95037};
    amrex::Gpu::HostVector<Real> h_N2_hi = {2.92664,      14.87977e-4,  -56.84761e-8,
                                    10.09704e-11, -6.75335e-15, -0.00923e+5,
                                    5.98053};

    // enthalpy of production at lo
    // K
    amrex::Gpu::HostVector<Real> h_HP = {h_H2_lo[5], h_O2_lo[5], h_H2O_lo[5], h_H_lo[5], h_O_lo[5], h_OH_lo[5], h_HO2_lo[5], h_H2O2_lo[5], h_N2_lo[5]};

    // used to get initial T
    amrex::Gpu::HostVector<Real> h_A0 = {h_H2_lo[0]-1.0, h_O2_lo[0]-1.0, h_H2O_lo[0]-1.0, h_H_lo[0]-1.0, h_O_lo[0]-1.0, h_OH_lo[0]-1.0, h_HO2_lo[0]-1.0, h_H2O2_lo[0]-1.0, h_N2_lo[0]-1.0};


    amrex::Gpu::HostVector<Real> h_visccoeff = {-1.85174759e-04,  3.89197447e-04, -6.98160471e-05,  6.36454790e-06, -2.11216442e-07,
                                                    -5.80149070e-03,  3.39295201e-03, -6.36767978e-04,  5.43779133e-05, -1.73237416e-06,
                                                     1.49332282e-02, -8.19367041e-03,  1.68282727e-03, -1.45738528e-04,  4.60116686e-06,
                                                    -5.03440195e-03,  2.77694310e-03, -5.05779281e-04,  4.17970313e-05, -1.28989348e-06,
                                                    -4.25439228e-03,  2.69409815e-03, -5.12461257e-04,  4.46919118e-05, -1.45052986e-06,
                                                    -4.31987570e-03,  2.73556559e-03, -5.20349037e-04,  4.53798077e-05, -1.47285635e-06,
                                                    -5.84665026e-03,  3.41936319e-03, -6.41724663e-04,  5.48011981e-05, -1.74585918e-06,
                                                    -5.89078698e-03,  3.44517618e-03, -6.46569083e-04,  5.52148957e-05, -1.75903878e-06,
                                                    -4.86883676e-03,  2.91081563e-03, -5.49840279e-04,  4.73395563e-05, -1.51957597e-06};


    amrex::Gpu::HostVector<Real> h_conductcoeff = {-5.28463600e-01,  3.15473947e-01, -6.87861093e-02,  6.58583530e-03, -2.30878456e-04,
                                                       3.45755008e-02, -2.10056875e-02,  4.78193956e-03, -4.69828968e-04,  1.73609698e-05,
                                                      -2.42370932e-01,  1.55066730e-01, -3.67910197e-02,  3.82882196e-03, -1.45626051e-04,
                                                      -2.05169993e-01,  1.03579992e-01, -1.83028199e-02,  1.47068561e-03, -4.35660111e-05,
                                                       5.22190001e-02, -3.07414624e-02,  7.02757543e-03, -7.03400778e-04,  2.64083229e-05,
                                                      -1.41026759e-01,  8.85651492e-02, -2.02651378e-02,  2.03706447e-03, -7.50266004e-05,
                                                      -7.12040025e-02,  4.31981643e-02, -9.82859052e-03,  1.00407694e-03, -3.78442817e-05,
                                                      -1.18157269e-02,  9.27318610e-03, -2.71924641e-03,  3.59761794e-04, -1.62746865e-05,
                                                       4.77081884e-02, -2.51859514e-02,  4.94733498e-03, -4.15782937e-04,  1.29219096e-05};

                                            
    amrex::Gpu::HostVector<Real> h_diffcoeff = {-0.006098424587788583,0.004074726320140367,-0.0007655079217743792,7.01945414920243e-05,-2.283978048241683e-06,
                                                    -0.007361772345996411,0.0042187108652972285,-0.0007832424157195558,6.805076239783791e-05,-2.1468141590124147e-06,
                                                    -0.012714368881604958,0.005800464307529211,-0.0008628173792959883,5.797865818973548e-05,-1.3148664989894858e-06,
                                                    -0.02331895278393948,0.012988306044054893,-0.0023909974764966175,0.00020481645048811995,-6.393432695483336e-06,
                                                    -0.008203316666546017,0.004854681011212246,-0.0009069544030339767,7.98972126021157e-05,-2.5440602562722086e-06,
                                                    -0.008176066416389936,0.004838554451993892,-0.0009039416296196897,7.963180543591231e-05,-2.535609250271386e-06,
                                                    -0.007355106588534061,0.004214891010226138,-0.000782533224544873,6.798914545886926e-05,-2.144870314265137e-06,
                                                    -0.00734883038938788,0.0042112943940458315,-0.0007818654797209853,6.793112951970908e-05,-2.143040071146875e-06,
                                                    -0.006631024777860074,0.0038397370846872426,-0.0007137696048348218,6.226499117199988e-05,-1.968810850179303e-06,
                                                    -0.003165622529780972,0.0016565295961648823,-0.0002955102867456441,2.4323268960998937e-05,-7.332687938295335e-07,
                                                    0.0017962300955720687,-0.0016252040565658543,0.00047039226523034656,-5.1041731991760684e-05,1.9707403545544835e-06,
                                                    -0.02065558573348347,0.01049932817565325,-0.0018228205967741945,0.00014586133392843038,-4.265266434634806e-06,
                                                    -0.004477615809775433,0.002401743913536847,-0.00043555046112257176,3.6523201605646507e-05,-1.1207561496132533e-06,
                                                    -0.004388261916818509,0.002353815467310766,-0.00042685875309455987,3.579435608041233e-05,-1.0983906923530078e-06,
                                                    -0.003141360101617588,0.0016438333792438298,-0.0002932453934942289,2.4136846998057358e-05,-7.276487676670712e-07,
                                                    -0.0031183633828342915,0.001631799491779716,-0.00029109865398359785,2.3960150196485035e-05,-7.223219240282344e-07,
                                                    -0.0030766151054801255,0.001623930137648841,-0.0002916902121621802,2.4185952795909278e-05,-7.345781692660118e-07,
                                                    0.015418678757875483,-0.008276025270343653,0.0016022988850484384,-0.0001292488075144311,3.7780416116707573e-06,
                                                    0.015491646067634311,-0.012297762819716031,0.0033220236270480115,-0.00034981378874120536,1.3233960398093872e-05,
                                                    -0.0031409034359764906,0.000964215499007546,-1.5256977763517157e-05,-1.0009613901179592e-05,6.865263141553794e-07,
                                                    -0.00309121182555696,0.0009489608368058763,-1.501560014564807e-05,-9.851253784321545e-06,6.756649174673959e-07,
                                                    -9.806367280891379e-05,-0.0005259982739909082,0.00023777270488750687,-2.9610758656448636e-05,1.2409556192468912e-06,
                                                    -9.754926582674518e-05,-0.0005232390750052945,0.00023652543424357053,-2.945543119768198e-05,1.2344460095131642e-06,
                                                    0.0009009636432407803,-0.0010975268417085945,0.0003568805024623778,-4.047906132942509e-05,1.6078992712041145e-06,
                                                    -0.049547371639744826,0.02420786673131437,-0.0040039853524907396,0.0003046592732139884,-8.373772281551857e-06,
                                                    -0.027018754679375097,0.014131423473429378,-0.0025198932193017465,0.00020732039220446453,-6.247265305088386e-06,
                                                    -0.026971255852400414,0.01410658050614936,-0.002515463260429814,0.00020695592405809058,-6.236282645924077e-06,
                                                    -0.020645950902716106,0.01049443074735431,-0.0018219703392133265,0.00014579329667774188,-4.2632768943398505e-06,
                                                    -0.02063688301930936,0.010489821500976207,-0.0018211701137997654,0.00014572926297830028,-4.261404425595428e-06,
                                                    -0.019430526862365516,0.00997359743800644,-0.0017481642351558765,0.00014128443680858248,-4.177030598880565e-06,
                                                    -0.0059382904901673595,0.0032649962117391896,-0.0005991311922260038,5.1014036740744293e-05,-1.5859623241784843e-06,
                                                    -0.00584963856139101,0.0032162535286223044,-0.0005901868443753459,5.02524551442533e-05,-1.5622857089560921e-06,
                                                    -0.0044547665185479274,0.0023894878048250404,-0.00043332784539284135,3.633682356306633e-05,-1.1150369265388267e-06,
                                                    -0.004433163993709295,0.0023779004479025216,-0.00043122650618556915,3.616061519603177e-05,-1.109629771571358e-06,
                                                    -0.00419972224671333,0.0022703026942310655,-0.0004131673665395828,3.48089622951453e-05,-1.0723125190961097e-06,
                                                    -0.005759622266467147,0.003166760688484433,-0.0005811048417034481,4.947915269564291e-05,-1.538244707164061e-06,
                                                    -0.004364944907483492,0.002341308479746039,-0.0004245906410903713,3.5604162935439344e-05,-1.0925543985047233e-06,
                                                    -0.004342895613050795,0.0023294814805234227,-0.0004224458433307047,3.542431033976762e-05,-1.0870354162206174e-06,
                                                    -0.004119743857368838,0.0022270676557828092,-0.0004052991175070151,3.414606970952483e-05,-1.05189168573903e-06,
                                                    -0.003116908817255142,0.0016310383363007772,-0.00029096287055129454,2.3948973946122656e-05,-7.219849958113728e-07,
                                                    -0.0030937303493271603,0.0016189093418438713,-0.00028879916479071873,2.37708806629901e-05,-7.166160527175076e-07,
                                                    -0.0030546059172749706,0.0016123130250736487,-0.00028960354725398777,2.40129336925455e-05,-7.293232158117316e-07,
                                                    -0.003070376910660832,0.001606688787447522,-0.00028661912554336986,2.35914429806519e-05,-7.112065802854437e-07,
                                                    -0.0030337554543677816,0.0016013075226178426,-0.00028762673971045356,2.3849023585380798e-05,-7.243449216977582e-07,
                                                    -0.0029800170761932095,0.0015859694924580709,-0.000286608674495536,2.3921390316404743e-05,-7.312450753144484e-07};

    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_mw.data(), h_mw.data() + NSPECS, mw);

    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H2_lo.data(), h_H2_lo.data() + 7, H2_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H2_hi.data(), h_H2_hi.data() + 7, H2_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_O2_lo.data(), h_O2_lo.data() + 7, O2_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_O2_hi.data(), h_O2_hi.data() + 7, O2_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H2O_lo.data(), h_H2O_lo.data() + 7, H2O_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H2O_hi.data(), h_H2O_hi.data() + 7, H2O_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H_lo.data(), h_H_lo.data() + 7, H_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H_hi.data(), h_H_hi.data() + 7, H_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_O_lo.data(), h_O_lo.data() + 7, O_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_O_hi.data(), h_O_hi.data() + 7, O_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_OH_lo.data(), h_OH_lo.data() + 7, OH_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_OH_hi.data(), h_OH_hi.data() + 7, OH_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_HO2_lo.data(), h_HO2_lo.data() + 7, HO2_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_HO2_hi.data(), h_HO2_hi.data() + 7, HO2_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H2O2_lo.data(), h_H2O2_lo.data() + 7, H2O2_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_H2O2_hi.data(), h_H2O2_hi.data() + 7, H2O2_hi);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_N2_lo.data(), h_N2_lo.data() + 7, N2_lo);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_N2_hi.data(), h_N2_hi.data() + 7, N2_hi);

    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_HP.data(), h_HP.data() + NSPECS, HP);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_A0.data(), h_A0.data() + NSPECS, A0);

    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_visccoeff.data(), h_visccoeff.data() + NSPECS*5, visccoeff);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_conductcoeff.data(), h_conductcoeff.data() + NSPECS*5, conductcoeff);
    amrex::Gpu::copyAsync(amrex::Gpu::hostToDevice, h_diffcoeff.data(), h_diffcoeff.data() + 45*5, diffcoeff);
#endif
}

#ifdef CHEM
Parm::Parm() {
    mw = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*NSPECS);

    H2_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H2_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    O2_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    O2_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H2O_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H2O_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    O_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    O_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    OH_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    OH_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    HO2_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    HO2_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H2O2_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    H2O2_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    N2_lo = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);
    N2_hi = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*7);

    HP = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*NSPECS);
    A0 = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*NSPECS);

    visccoeff = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*NSPECS*5);
    conductcoeff = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*NSPECS*5);
    diffcoeff = (Real*)amrex::The_Arena()->alloc(sizeof(Real)*45*5);
}

Parm::~Parm() {
    amrex::The_Arena()->free(mw);
    amrex::The_Arena()->free(H2_lo);
    amrex::The_Arena()->free(H2_hi);
    amrex::The_Arena()->free(O2_lo);
    amrex::The_Arena()->free(O2_hi);
    amrex::The_Arena()->free(H2O_lo);
    amrex::The_Arena()->free(H2O_hi);
    amrex::The_Arena()->free(H_lo);
    amrex::The_Arena()->free(H_hi);
    amrex::The_Arena()->free(O_lo);
    amrex::The_Arena()->free(O_hi);
    amrex::The_Arena()->free(OH_lo);
    amrex::The_Arena()->free(OH_hi);
    amrex::The_Arena()->free(HO2_lo);
    amrex::The_Arena()->free(HO2_hi);
    amrex::The_Arena()->free(H2O2_lo);
    amrex::The_Arena()->free(H2O2_hi);
    amrex::The_Arena()->free(N2_lo);
    amrex::The_Arena()->free(N2_hi);
    amrex::The_Arena()->free(HP);
    amrex::The_Arena()->free(A0);
    amrex::The_Arena()->free(visccoeff);
    amrex::The_Arena()->free(conductcoeff);
    amrex::The_Arena()->free(diffcoeff);
}
#endif