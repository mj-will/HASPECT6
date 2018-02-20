#rootcint -f TFastMCDict.cxx -c TFastMC.hxx TFastMCLinkDef.h 
#g++ $ROOTCFLAGS -I$ROOTSYS/include $ROOTSYS/main/src/rmain.cxx TFastMC.cxx FastMCDict.cxx -o rootFMC /usr/lib/cernlib/2006/lib/libmathlib.a $ROOTLIBS -L/usr/lib/X11R5 -lXpm -lX11 -lm -ldld
#
#g++ -shared -o libFastMC.so TFastMC.cxx -I$ROOTSYS/include $ROOTSYS/main/src/rmain.cxx TFastMC.cxx FastMCDict.cxx -o rootFMC /usr/lib/cernlib/2006/lib/libmathlib.a $ROOTLIBS -L/usr/lib/X11R5 -lXpm -lX11 -lm 

#rootcint -f TFastMCDict.cxx -c TFastMC.hxx TFastMCLinkDef.h 
#     g++ -fPIC -I/$ROOTSYS/include -o TFastMCDict.o TFastMCDict.cxx -c
#     g++ -fPIC -I/$ROOTSYS/include -o TFastMC.o TFastMC.cxx -c
#     g++ -o libFastMC.so -shared TFastMC.o  TFastMCDict.o 


S     := $(shell root-config --glibs) -lEG
ROOTGLIBS    := $(shell root-config --glibs)
ROOTCFLAGS   := $(shell root-config --auxcflags)

g++ -c -Wall -fPIC $(ROOTCFLAGS) -I$(ROOTSYS)/include \
          $(ROOTSYS)/include/rmain.cxx TFastMC.cxx FastMCDict.cxx -o rootFMC \
          /cern/pro/lib/libmathlib.a \
         $(ROOTLIBS)	\
         -L/usr/lib/X11R5 -lXpm  -lX11 -lm -ldld