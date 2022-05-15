PARTICLE=mu-
ENERGY=3 GeV
ZPOSITION:=$(shell seq -90 10 90)
RUNCOUNT=1000

.PHONY: all

all: ./data/muon_z.csv

./macro/run1.mac:
	touch $@
	echo "/run/initialize" >> $@

	echo "/control/verbose 0" >> $@
	echo "/run/verbose 0" >> $@ 
	echo "/event/verbose 0" >> $@
	echo "/tracking/verbose 0" >> $@

	echo "/gun/particle ${PARTICLE}" >> $@
	echo "/gun/energy $(ENERGY)" >> $@

	for Z in $(ZPOSITION); do \
		echo "/gun/position 0 -20 $$Z cm" >> $@; \
		echo "/run/beamOn $(RUNCOUNT)" >> $@; \
	done

./data/muon_z.csv: ./macro/run1.mac
	touch $@
	mkdir -p build/
	cd build;cmake ../;make -j8
	./build/MuonVeto $^ | grep ">>>>" | sed 's/>>>> //g' > $@

.PHONY: clean

clean:
	rm -f ./macro/run1.mac ./data/muon_z.csv
