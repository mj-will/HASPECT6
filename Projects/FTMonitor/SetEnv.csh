setenv ROOTSYS /apps/root/6.10.02/root
source /group/clas12/gemc/environment.csh  4a.2.1
setenv HSANA /work/clas12/mesonex/HASPECT6
setenv RHIPO $HSANA/../ExtraPackages/rhipo3/
setenv CHIPO /work/clas12/mesonex/libcpp
setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/work/clas12/mesonex/lz4-1.7.5/lib
setenv LZ4_h  /work/clas12/mesonex/lz4-1.7.5/lib
alias hipo2root root -l $RHIPO/Hipo2Root.C
alias allhipo2root root -l $RHIPO/Hipo2Root.C $RHIPO/ConvertAll.C
alias hipo2hs root -l --hsdata $PWD/ConvertHSHipoChain.C
alias hipo2ftmon  root --hsdata --THSHipoFTMon.C FTMonitor.C
