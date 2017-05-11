
SRCFILES := $(wildcard *.cc)
$(shell cp $(HSANA)/THSParticle.C THSParticle.cc)$
$(shell cp $(HSANA)/THSParticle.h .)$
$(shell cp $(HSANA)/THSDataManager.C THSDataManager.cc)$
$(shell cp $(HSANA)/THSDataManager.h .)$
$(shell cp $(HSANA)/THSWeights.C THSWeights.cc)$
$(shell cp $(HSANA)/THSWeights.h .)$

#INC_DIR += -I$(HSANA)/
.PHONY: default

default: $(LIB)

%.a: $(SRCFILES:.cc=.o)
	ar -rsv $@ $^

%.o : %.cc
	$(CXX) $(CXX_FLAGS) -M -o $*.d $< $(INC_DIR); \
	cp $*.d $*.dep; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.dep; \
	rm -f $*.d $*.d.bak
	$(CXX) $(CXX_FLAGS) -c -o $@ $< $(INC_DIR)
dict: 
	rootcint -f THSParticle_Dict.cc -c -p THSParticle.h THS_LinkDef.h
	rootcint -f THSDataManager_Dict.cc -c -p THSDataManager.h THS_LinkDef.h
	rootcint -f THSWeights_Dict.cc -c -p THSWeights.h THS_LinkDef.h
	mv *pcm $(MYAMPTOOLS)/lib

clean:
	rm *.o *.dep *.a *Dict*

-include $(SRCFILES:.cc=.dep)
