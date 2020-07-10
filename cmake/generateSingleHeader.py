
import os
import subprocess

from glob import glob
from pathlib import Path


def readFile(filepath):

    try:
        with open(filepath) as f:
            lines = f.readlines()
    
    except:
        return []

    return lines


def readFiles(filepaths):
    
    names = map(lambda p: Path(p).stem, filepaths)
    files = map(readFile, filepaths)

    return dict(zip(names, files))


def inlineSourceFunctions(sources):

    # TODO: Tidy with clang first?

    for name in sources.keys():
        source = sources[name]

        if name == "Exceptions":
            for i, line in enumerate(source):
                if line.strip().startswith("void ") and line.strip().endswith(")"):
                    sources[name][i] = "inline " + line

        if name == "FileTypes":
            for i, line in enumerate(source):
                if line.strip().startswith("Type ") and line.strip().endswith(")"):
                    sources[name][i] = "inline " + line
       
        else:
            for i, line in enumerate(source):
                if name + "::" in line and not line.startswith(" ") and not line.strip().endswith(";"):
                    sources[name][i] = "inline " + line

                elif line.startswith("std::shared_ptr<BuiltInReader> BuiltIn()"):
                    sources[name][i] = "inline " + line


    return sources


def stripComments(lines):

    lines = [l for l in lines if not l.strip().startswith("/")]
    lines = [l for l in lines if not l.strip().startswith("*")]
    lines = [l.split("//")[0] for l in lines]

    return lines


def stripMacros(lines):

    lines = [l for l in lines if not l.strip().startswith("#pragma")]

    return lines


def isLocalInclude(line, localIncludes):
    
    if (not line.strip().startswith("#include")):
        return False

    include = Path(line.strip().split()[-1].strip("\"")).stem

    return include in localIncludes 


def stripLocalIncludes(lines, localIncludes):
   
    lines = [l for l in lines if not isLocalInclude(l, localIncludes)]

    return lines


def gatherIncludes(lines):

    seen = []
    unseen = []
   
    for l in lines:
        if (l.startswith("#include")):
            if (l not in seen):
                seen.append(l)

            else:
                continue

        unseen.append(l)

    return unseen


def addLicense(lines):

    license = readFile("../LICENSE")
    license = [ "// " + l for l in license ]
    license.append("\n\n")

    message = """// CADMESH - Load CAD files into Geant4 quickly and easily.
    //
    // Read all about it at: https://github.com/christopherpoole/CADMesh
    //
    // Basic usage:
    //
    //     #include "CADMesh.hh" // This file.
    //     ...
    //     auto mesh = CADMesh::TessellatedMesh::FromSTL("mesh.stl");
    //     G4VSolid* solid = mesh->GetSolid();
    //     ...
    //

    // !! THIS FILE HAS BEEN AUTOMATICALLY GENERATED. DON'T EDIT IT DIRECTLY. !!
    
    """

    return license + [ message ] + lines


if __name__ == "__main__":

    includes = [
      "FileTypes"
    , "Mesh"
    , "Reader"
    , "Lexer"
    , "ASSIMPReader"
    , "BuiltInReader"
    , "CADMeshTemplate"
    , "Exceptions"
    , "TessellatedMesh"
    , "TetrahedralMesh"
    ]

    excludes = [
      "CADMesh"
    , "Configuration"
    ]

    sources = [
      "FileTypes"
    , "Mesh"
    , "Reader"
    , "Lexer"
    , "CADMeshTemplate"
    , "Exceptions"
    , "TessellatedMesh"
    , "TetrahedralMesh"
    ]

    readers = [
      "LexerMacros"
    , "STLReader"
    , "OBJReader"
    , "PLYReader"
    , "ASSIMPReader"
    , "BuiltInReader"
    ]

    hh = readFiles([os.path.join("../include", i + ".hh") for i in includes + readers])
    cc = readFiles([os.path.join("../src", s + ".cc") for s in sources + readers])

    cc = inlineSourceFunctions(cc)

    header = ["class " + h + ";\n" for h in hh]

    for i in includes:
        if i == "CADMeshTemplate":
            header += "#ifndef CADMESH_DEFAULT_READER\n#define CADMESH_DEFAULT_READER BuiltIn\n#endif\n\n"
   
        header += hh[i]

    for s in sources:
        header += cc[s]

    for r in readers:
        if r in includes and r in readers:
            continue
        
        header += hh[r]

    for r in readers:
        header += cc [r]

    header = stripComments(header)
    header = stripMacros(header)

    header = stripLocalIncludes(header, sources + readers + excludes)
    
    header = gatherIncludes(header)
  
    header = [ "#pragma once\n\n" ] + header

    header = addLicense(header)

    with open("CADMesh.hh", "w") as f:
        f.writelines(header)

    subprocess.run(["clang-format", "-i", "CADMesh.hh"])

