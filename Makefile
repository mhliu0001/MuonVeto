DEFAULT_PARTICLE=mu-
DEFAULT_ENERGY=3 GeV
DEFAULT_ZPOSITION=0
ENERGY:=0.5 2 5 10
ZPOSITION:=$(shell seq -90 10 90)
RUNCOUNT=1000

.PHONY: all

all: ./data/muon_z.csv

.PHONY: build
build: ./build/MuonVeto

./build/MuonVeto: ./macro/muon_z.mac ./macro/muon_energy.mac
	mkdir -p build/
	cd build;cmake ../;make -j8

./macro/muon_z.mac:
	touch $@
	echo "/run/initialize" >> $@

	echo "/control/verbose 0" >> $@
	echo "/run/verbose 0" >> $@ 
	echo "/event/verbose 0" >> $@
	echo "/tracking/verbose 0" >> $@

	echo "/gun/particle ${DEFAULT_PARTICLE}" >> $@
	for E in $(ENERGY); do \
		echo "/gun/energy $$E GeV" >> $@; \
		for Z in $(ZPOSITION); do \
			echo "/gun/position 0 -20 $$Z cm" >> $@; \
			echo "/run/beamOn $(RUNCOUNT)" >> $@; \
		done; \
	done

./macro/muon_energy.mac:
	touch $@
	echo "/run/initialize" >> $@

	echo "/control/verbose 0" >> $@
	echo "/run/verbose 0" >> $@ 
	echo "/event/verbose 0" >> $@
	echo "/tracking/verbose 0" >> $@

	echo "/gun/particle ${DEFAULT_PARTICLE}" >> $@
	echo "/gun/position 0 -20 ${DEFAULT_ZPOSITION} cm" >> $@

	for E in $(ENERGY); do \
		echo "/gun/energy $$E GeV" >> $@; \
		echo "/run/beamOn $(RUNCOUNT)" >> $@; \
	done

./data/muon_z.csv: ./build/MuonVeto ./macro/muon_z.mac
	touch $@
	$^ | grep ">>>>" | sed 's/>>>> //g' > $@

./data/muon_energy.csv: ./build/MuonVeto ./macro/muon_energy.mac
	touch $@
	$^ | grep ">>>>" | sed 's/>>>> //g' > $@

.PHONY: clean


clean:
	rm -rf ./macro/muon_z.mac ./data/muon_z.csv ./macro/muon_energy.mac ./data/muon_energy.mac ./build
