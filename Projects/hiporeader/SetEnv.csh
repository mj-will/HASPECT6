setenv CLARA_HOME /home/dglazier/Dropbox/hsana/Events/ExtraPackages/clara
#setenv COATJAVA /work/clas12/dglazier/rhipo/plugins/clas12/
setenv RHIPO /home/dglazier/Dropbox/hsana/Events/ExtraPackages/hipo/rhipo
setenv CHIPO $RHIPO/chipo/libcpp
setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/group/clas12/packages/lz4/lib
#setenv LZ4_h /group/clas12/packages/lz4/lib
setenv LZ4_h /usr/include/
alias hipo2root root -l $RHIPO/Hipo2Root.C 
alias allhipo2root root -l $RHIPO/Hipo2Root.C $RHIPO/ConvertAll.C
