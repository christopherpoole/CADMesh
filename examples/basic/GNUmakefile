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

CADMESH_INSTALL = ../..

CPPFLAGS += -I$(CADMESH_INSTALL)/include
CPPFLAGS += -I$(CADMESH_INSTALL)/vcglib
EXTRALIBS += $(CADMESH_INSTALL)/CADMesh.a
EXTRALIBSSOURCEDIRS += $(CADMESH_INSTALL)/src
EXTRA_LINK_DEPENDENCIES += $(CADMESH_INSTALL)/CADMesh.a

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/binmake.gmk
include $(CADMESH_INSTALL)/cadmesh.gmk

