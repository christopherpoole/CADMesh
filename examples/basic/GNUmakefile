####################################################
# GEANT4 VCGLIB/CAD INTERFACE - basic example
#
# File:      GNUMakefile
#
# Author:    Christopher M Poole,
# Email:     mail@christopherpoole.net
#
# Date:      20th March, 2011
####################################################

name := cadmesh_example
G4TARGET := $(name)
G4EXLIB := true

CPPFLAGS += -I/home/chris/Software/vcglib
EXTRALIBS := -lcadmesh

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/binmake.gmk

