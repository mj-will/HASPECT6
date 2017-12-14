setenv RHIPO /pat_to/rhipo3
setenv CHIPO /path_to/libcpp
setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/work/clas12/mesonex/lz4-1.7.5/lib
setenv LZ4_h /work/clas12/mesonex/lz4-1.7.5/lib
alias hipo2root root -l $RHIPO/Hipo2Root.C 
alias allhipo2root root -l $RHIPO/Hipo2Root.C $RHIPO/ConvertAll.C
