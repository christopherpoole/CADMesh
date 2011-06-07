####################################################
# GEANT4 VCGLIB/CAD INTERFACE
#
# File:      Makefile - shared library
#
# Author:    Christopher M Poole,
# Email:     mail@christopherpoole.net
#
# Date:      7th April, 2011
####################################################

G4INCLUDE_EXEC := $(shell [ -r $(G4INCLUDE)/globals.hh ] && echo 1)

libcadmesh.so: src/CADMesh.o vcglib/wrap/ply/plylib.o
	g++ -shared -o libcadmesh.so src/CADMesh.o vcglib/wrap/ply/plylib.o -ltet 

src/CADMesh.o: src/CADMesh.cc
	cd src/; g++ -g -fPIC -O2 -I../include/ -I../vcglib -I$(G4INCLUDE) -I$(CLHEP_INCLUDE_DIR) -c CADMesh.cc

vcglib/wrap/ply/plylib.o: vcglib/wrap/ply/plylib.cpp
	cd vcglib/wrap/ply/; g++ -g -fPIC -O2 -c plylib.cpp

install:
	cp libcadmesh.so /usr/lib/libcadmesh.so

remove:
	rm /usr/lib/libcadmesh.so

clean:
	rm libcadmesh.so src/CADMesh.o vcglib/wrap/ply/plylib.o
	
