#setenv CLARA_HOME 
#setenv COATJAVA 
setenv RHIPO $PWD/rhipo
setenv CHIPO $PWD/libcpp
setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/work/clas12/mesonex/lz4-1.7.5/lib
#setenv LZ4_h /group/clas12/packages/lz4/lib
setenv LZ4_h /work/clas12/mesonex/lz4-1.7.5/lib
alias hipo2root root -l $RHIPO/Hipo2Root.C 
alias allhipo2root root -l $RHIPO/Hipo2Root.C $RHIPO/ConvertAll.C
