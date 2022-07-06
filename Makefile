DEFAULT_PARTICLE=mu-
DEFAULT_ENERGY=3 GeV
DEFAULT_ZPOSITION=0
MUENERGY:=0.5 2 5 10
MUENERGY:=3
GAMMAENERGY:=0.2 0.5 1 2
ZPOSITION:=$(shell seq -90 10 90)
XPOSITION:=$(shell seq -9 1 9)
RUNCOUNT=10000

.PHONY: all
all: data

.PHONY: build
build: ./build/MuonVeto

.PHONY: data
data: ./data/muon_z.csv ./data/muon_x.csv ./data/gamma_z.csv

.PHONY: macro
macro: ./macro/muon_z.mac ./macro/muon_x.mac ./macro/gamma_z.mac

./build/MuonVeto: ./macro/muon_z.mac ./macro/muon_x.mac ./macro/gamma_z.mac
	mkdir -p build/
	cd build;cmake ../;make -j8

./macro/muon_z.mac:
	echo "/run/initialize" >> $@

	echo "/control/verbose 0" >> $@
	echo "/run/verbose 0" >> $@ 
	echo "/event/verbose 0" >> $@
	echo "/tracking/verbose 0" >> $@

	echo "/gun/particle ${DEFAULT_PARTICLE}" >> $@
	echo "/gun/energy $$DEFAULT_ENERGY" >> $@; \
	for Z in $(ZPOSITION); do \
		echo "/gun/position 0 -20 $$Z cm" >> $@; \
		echo "/run/beamOn $(RUNCOUNT)" >> $@; \
	done

./macro/muon_x.mac:
	echo "/run/initialize" >> $@

	echo "/control/verbose 0" >> $@
	echo "/run/verbose 0" >> $@ 
	echo "/event/verbose 0" >> $@
	echo "/tracking/verbose 0" >> $@

	echo "/gun/particle ${DEFAULT_PARTICLE}" >> $@
	echo "/gun/energy ${DEFAULT_ENERGY}" >> $@

	for X in $(XPOSITION); do \
		echo "/gun/position $$X -20 ${DEFAULT_ZPOSITION} cm" >> $@; \
		echo "/run/beamOn $(RUNCOUNT)" >> $@; \
	done

./macro/gamma_z.mac:
	echo "/run/initialize" >> $@

	echo "/control/verbose 0" >> $@
	echo "/run/verbose 0" >> $@ 
	echo "/event/verbose 0" >> $@
	echo "/tracking/verbose 0" >> $@

	echo "/gun/particle gamma" >> $@
	for E in $(GAMMAENERGY); do \
		echo "/gun/energy $$E MeV" >> $@; \
		for Z in $(ZPOSITION); do \
			echo "/gun/position 0 -20 $$Z cm" >> $@; \
			echo "/run/beamOn $(RUNCOUNT)" >> $@; \
		done; \
	done

./data/muon_z.csv: ./build/MuonVeto ./macro/muon_z.mac
	$^ | grep ">>>>" | sed 's/>>>> //g' > $@
	mv SiPM_0 ./data/muon_z_SiPM_0.csv
	mv SiPM_1 ./data/muon_z_SiPM_1.csv


./data/muon_x.csv: ./build/MuonVeto ./macro/muon_x.mac
	$^ | grep ">>>>" | sed 's/>>>> //g' > $@
	mv SiPM_0 ./data/muon_x_SiPM_0.csv
	mv SiPM_1 ./data/muon_x_SiPM_1.csv

./data/gamma_z.csv: ./build/MuonVeto ./macro/gamma_z.mac
	$^ | grep ">>>>" | sed 's/>>>> //g' > $@
	mv SiPM_0 ./data/gamma_z_SiPM_0.csv
	mv SiPM_1 ./data/gamma_z_SiPM_1.csv

.PHONY: clean_macro

clean_macro:
	rm -r ./macro/muon_x.mac ./macro/muon_z.mac ./macro/gamma_z.mac

.PHONY: clean_build

clean_build:
	rm -r ./build

.PHONY: clean

clean:
	rm -rf ./macro/muon_z.mac ./data/muon_z.csv ./macro/muon_x.mac ./data/muon_x.csv ./macro/gamma_z.mac ./data/gamma_z.csv ./build
