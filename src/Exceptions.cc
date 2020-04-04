// The MIT License (MIT)
//
// Copyright (c) 2011-2020 Christopher M. Poole <mail@christopherpoole.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// CADMesh //
#include "Exceptions.hh"


namespace CADMesh
{

namespace Exceptions
{

void FileNotFound(G4String origin, G4String filepath)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "FileNotFound"
               , FatalException
               , ("\nThe file: \n\t" + filepath + "\ncould not be found.").c_str());
}


void LexerError(G4String origin, G4String message)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "LexerError"
               , FatalException
               , ("\nThe CAD file appears to contain incorrect syntax:\n\t" + message).c_str());
}


void ParserError(G4String origin, G4String message)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "ParserError"
               , FatalException
               , ("\nThe CAD file appears to contain invalid data:\n\t" + message).c_str());
}


void ReaderCantReadError(G4String origin, File::Type file_type, G4String filepath)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "ReaderCantReadError"
               , FatalException
               , (G4String("\nThe the reader can't read files of type '")
                   + File::TypeString[file_type]
                   + ".'\n\tSpecified the incorrect file type (?) for file:\n\t\t"
                   + filepath).c_str());
}



} // Exceptions namespace

} // CADMesh namespace
