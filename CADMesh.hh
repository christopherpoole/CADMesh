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

// CADMESH - Load CAD files into Geant4 quickly and easily.
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

#pragma once

class BuiltInReader;
class Exceptions;
class PLYReader;
class Reader;
class CADMeshTemplate;
class FileTypes;
class TessellatedMesh;
class ASSIMPReader;
class STLReader;
class TetrahedralMesh;
class OBJReader;
class Mesh;
class Lexer;
class LexerMacros;

#include "G4String.hh"

#include <algorithm>
#include <map>

namespace CADMesh {

namespace File {

enum Type {
  Unknown,
  PLY,
  STL,
  DAE,
  OBJ,
  TET,
  OFF,
};

static std::map<Type, G4String> Extension = {
    {Unknown, "unknown"}, {PLY, "ply"}, {STL, "stl"}, {DAE, "dae"},
    {OBJ, "obj"},         {TET, "tet"}, {OFF, "off"}};

static std::map<Type, G4String> TypeString = {
    {Unknown, "UNKNOWN"}, {PLY, "PLY"}, {STL, "STL"}, {DAE, "DAE"},
    {OBJ, "OBJ"},         {TET, "TET"}, {OFF, "OFF"}};

static std::map<Type, G4String> TypeName = {
    {Unknown, "Unknown File Format"}, {PLY, "Stanford Triangle Format (PLY)"},
    {STL, "Stereolithography (STL)"}, {DAE, "COLLADA (DAE)"},
    {OBJ, "Wavefront (OBJ)"},         {TET, "TetGet (TET)"},
    {OFF, "Object File Format (OFF)"}};

Type TypeFromExtension(G4String extension);
Type TypeFromName(G4String name);
}
}

#include "G4ThreeVector.hh"
#include "G4TriangularFacet.hh"

#include <memory>
#include <vector>

namespace CADMesh {

typedef std::vector<G4ThreeVector> Points;
typedef std::vector<G4TriangularFacet *> Triangles;

class Mesh {
public:
  Mesh(Points points, Triangles triangles, G4String name = "");

  static std::shared_ptr<Mesh> New(Points points, Triangles triangles,
                                   G4String name = "");

  static std::shared_ptr<Mesh> New(Triangles triangles, G4String name = "");

  static std::shared_ptr<Mesh> New(std::shared_ptr<Mesh> mesh,
                                   G4String name = "");

public:
  G4String GetName();
  Points GetPoints();
  Triangles GetTriangles();

  G4bool IsValidForNavigation();

private:
  G4String name_ = "";

  Points points_;
  Triangles triangles_;
};

typedef std::vector<std::shared_ptr<Mesh>> Meshes;
}

namespace CADMesh {

namespace File {

class Reader {
public:
  Reader(G4String reader_name);
  ~Reader();

  virtual G4bool Read(G4String filepath) = 0;
  virtual G4bool CanRead(Type file_type) = 0;

public:
  G4String GetName();

  std::shared_ptr<Mesh> GetMesh();
  std::shared_ptr<Mesh> GetMesh(size_t index);
  std::shared_ptr<Mesh> GetMesh(G4String name, G4bool exact = true);

  size_t GetNumberOfMeshes();

  Meshes GetMeshes();

protected:
  size_t AddMesh(std::shared_ptr<Mesh> mesh);
  void SetMeshes(Meshes meshs);

private:
  Meshes meshes_;

  G4String name_ = "";
};
}
}

#include <iostream>
#include <string>

namespace CADMesh {

namespace File {

struct Token {
  std::string name;

  bool operator==(Token other) { return name == other.name; };
  bool operator!=(Token other) { return name != other.name; };
};

static Token ErrorToken{"ErrorToken"};
static Token EnfOfFileToken{"EndOfFileToken"};

static Token ParentToken{"Parent"};
static Token WordToken{"Word"};
static Token NumberToken{"Number"};
static Token ThreeVectorToken{"ThreeVector"};
static Token VertexToken{"Vertex"};
static Token VerticesToken{"Vertices"};
static Token FacetToken{"Facet"};
static Token LineToken{"Line"};
static Token NormalToken{"Normal"};
static Token TextureToken{"Texture"};
static Token SolidToken{"Solid"};
static Token ObjectToken{"Object"};
static Token CommentToken{"Comment"};
static Token BlankLineToken{"BlankLine"};

struct Item {
  Token token;

  size_t position;
  size_t line;

  std::string value;
  std::string error;

  Item *parent;

  std::vector<Item> children;
};

typedef std::vector<Item> Items;
typedef Items::iterator ItemsIterator;

class Lexer;

struct State {
  virtual State *operator()(Lexer *) const = 0;
};

struct __FinalState : public State {
  State *operator()(Lexer *) const { return nullptr; }
};

class Lexer {
public:
  Lexer(std::string filepath, State *initial_state = nullptr);

public:
  std::string String();

  void Run(State *initial_state, size_t lines = 0);
  Items GetItems();

  void Backup();
  void BackupTo(int position);

  std::string Next();
  std::string Peek();

  void Skip();

  Item *ThisIsA(Token token, std::string error = "");
  Item *StartOfA(Token token, std::string error = "");
  Item *EndOfA(Token token, std::string error = "");
  Item *MaybeEndOfA(Token token, std::string error = "");

  bool OneOf(std::string possibles);
  bool ManyOf(std::string possibles);
  bool Until(std::string match);
  bool MatchExactly(std::string match);

  bool OneDigit();
  bool ManyDigits();

  bool OneLetter();
  bool ManyLetters();

  bool ManyCharacters();

  bool Integer();
  bool Float();
  bool Number();

  bool SkipWhiteSpace();
  bool SkipLineBreak();
  bool SkipLineBreaks();
  bool SkipLine();

  State *Error(std::string message);
  State *LastError();

  bool TestState(State *state);

  bool IsDryRun();

  void PrintMessage(std::string name, std::string message);
  void PrintItem(Item item);

  size_t LineNumber();

private:
  State *state_;

  Item *parent_item_ = nullptr;
  Items items_;

  std::string input_;

  size_t position_ = 0;
  size_t start_ = 0;
  size_t width_ = 1;
  size_t line_ = 1;
  size_t end_line_ = 0;

  bool dry_run_ = false;

  int depth_ = 0;

  std::string last_error_ = "";
};
}
}

#ifdef USE_CADMESH_ASSIMP_READER

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace CADMesh {

namespace File {

class ASSIMPReader : public File::Reader {
public:
  ASSIMPReader();
  ~ASSIMPReader();

public:
  G4bool Read(G4String filepath);
  G4bool CanRead(Type file_type);

private:
  Assimp::Importer *importer_;
};

std::shared_ptr<ASSIMPReader> ASSIMP();
}
}
#endif

namespace CADMesh {

namespace File {

class BuiltInReader : public Reader {
public:
  BuiltInReader() : Reader("BuiltInReader"){};

public:
  G4bool Read(G4String filepath);
  G4bool CanRead(File::Type file_type);
};

std::shared_ptr<BuiltInReader> BuiltIn();
}
}
#ifndef CADMESH_DEFAULT_READER
#define CADMESH_DEFAULT_READER BuiltIn
#endif

#include "G4AssemblyVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4TessellatedSolid.hh"
#include "G4Tet.hh"
#include "G4UIcommand.hh"

namespace CADMesh {

template <typename T> class CADMeshTemplate {
public:
  CADMeshTemplate();

  CADMeshTemplate(G4String file_name);

  CADMeshTemplate(G4String file_name, File::Type file_type);

  CADMeshTemplate(std::shared_ptr<File::Reader> reader);

  CADMeshTemplate(G4String file_name, std::shared_ptr<File::Reader> reader);

  CADMeshTemplate(G4String file_name, File::Type file_type,
                  std::shared_ptr<File::Reader> reader);

  static std::shared_ptr<T> From(G4String file_name);

  static std::shared_ptr<T> From(G4String file_name,
                                 std::shared_ptr<File::Reader> reader);

  static std::shared_ptr<T> FromPLY(G4String file_name);

  static std::shared_ptr<T> FromPLY(G4String file_name,
                                    std::shared_ptr<File::Reader> reader);

  static std::shared_ptr<T> FromSTL(G4String file_name);

  static std::shared_ptr<T> FromSTL(G4String file_name,
                                    std::shared_ptr<File::Reader> reader);

  static std::shared_ptr<T> FromOBJ(G4String file_name);

  static std::shared_ptr<T> FromOBJ(G4String file_name,
                                    std::shared_ptr<File::Reader> reader);

  ~CADMeshTemplate();

public:
  virtual G4VSolid *GetSolid() = 0;
  virtual G4VSolid *GetSolid(G4int index) = 0;
  virtual G4VSolid *GetSolid(G4String name, G4bool exact = true) = 0;

  virtual std::vector<G4VSolid *> GetSolids() = 0;

  virtual G4AssemblyVolume *GetAssembly() = 0;

  bool IsValidForNavigation();

public:
  G4String GetFileName();

  File::Type GetFileType();

  void SetVerbose(G4int verbose);
  G4int GetVerbose();

  void SetScale(G4double scale);
  G4double GetScale();

  void SetOffset(G4double x, G4double y, G4double z);
  void SetOffset(G4ThreeVector offset);
  G4ThreeVector GetOffset();

protected:
  G4String file_name_;
  File::Type file_type_;
  G4int verbose_;
  G4double scale_;
  G4ThreeVector offset_;

  G4AssemblyVolume *assembly_ = nullptr;

  std::shared_ptr<File::Reader> reader_ = nullptr;
};
}

#include "globals.hh"

namespace CADMesh {

namespace Exceptions {

void FileNotFound(G4String origin, G4String filepath);

void LexerError(G4String origin, G4String message);

void ParserError(G4String origin, G4String message);

void ReaderCantReadError(G4String origin, File::Type file_type,
                         G4String filepath);

void MeshNotFound(G4String origin, size_t index);
void MeshNotFound(G4String origin, G4String name);
}
}

namespace CADMesh {

class TessellatedMesh : public CADMeshTemplate<TessellatedMesh> {
  using CADMeshTemplate::CADMeshTemplate;

public:
  G4VSolid *GetSolid();
  G4VSolid *GetSolid(G4int index);
  G4VSolid *GetSolid(G4String name, G4bool exact = true);

  std::vector<G4VSolid *> GetSolids();

  G4TessellatedSolid *GetTessellatedSolid();
  G4TessellatedSolid *GetTessellatedSolid(G4int index);
  G4TessellatedSolid *GetTessellatedSolid(G4String name, G4bool exact = true);
  G4TessellatedSolid *GetTessellatedSolid(std::shared_ptr<Mesh> mesh);

  G4AssemblyVolume *GetAssembly();

public:
  void SetReverse(G4bool reverse) { this->reverse_ = reverse; };

  G4bool GetReverse() { return this->reverse_; };

private:
  G4bool reverse_;
};
}

#ifdef USE_CADMESH_TETGEN

#include "tetgen.h"

namespace CADMesh {

class TetrahedralMesh : public CADMeshTemplate<TetrahedralMesh> {
public:
  using CADMeshTemplate::CADMeshTemplate;

  TetrahedralMesh();
  ~TetrahedralMesh();

public:
  G4VSolid *GetSolid();
  G4VSolid *GetSolid(G4int index);
  G4VSolid *GetSolid(G4String name, G4bool exact = true);

  std::vector<G4VSolid *> GetSolids();

  G4AssemblyVolume *GetAssembly();

public:
  void SetMaterial(G4Material *material) { this->material_ = material; };

  G4Material *GetMaterial() { return this->material_; };

  void SetQuality(G4double quality) { this->quality_ = quality; };

  G4double GetQuality() { return this->quality_; };

  std::shared_ptr<tetgenio> GetTetgenInput() { return in_; };

  std::shared_ptr<tetgenio> GetTetgenOutput() { return out_; };

private:
  G4ThreeVector GetTetPoint(G4int index_offset);

private:
  std::shared_ptr<tetgenio> in_ = nullptr;
  std::shared_ptr<tetgenio> out_ = nullptr;

  G4double quality_;

  G4Material *material_;
};
}
#endif

namespace CADMesh {

namespace File {

Type TypeFromExtension(G4String extension) {
  std::for_each(extension.begin(), extension.end(),
                [](char &e) { e = ::tolower(e); });

  for (auto e = Extension.begin(); e != Extension.end(); ++e) {
    if (e->second == extension) {
      return e->first;
    }
  }

  return Unknown;
}

Type TypeFromName(G4String name) {
  auto extension = name.substr(name.find_last_of(".") + 1);

  return TypeFromExtension(extension);
}
}
}

namespace CADMesh {

Mesh::Mesh(Points points, Triangles triangles, G4String name)
    : name_(name), points_(points), triangles_(triangles) {}

std::shared_ptr<Mesh> Mesh::New(Points points, Triangles triangles,
                                G4String name) {
  return std::make_shared<Mesh>(points, triangles, name);
}

std::shared_ptr<Mesh> Mesh::New(Triangles triangles, G4String name) {
  Points points;

  return New(points, triangles, name);
}

std::shared_ptr<Mesh> Mesh::New(std::shared_ptr<Mesh> mesh, G4String name) {
  return New(mesh->GetPoints(), mesh->GetTriangles(), name);
}

G4String Mesh::GetName() { return name_; }

Points Mesh::GetPoints() { return points_; }

Triangles Mesh::GetTriangles() { return triangles_; }

G4bool Mesh::IsValidForNavigation() {
  std::map<G4ThreeVector, size_t> point_index;

  size_t index = 0;
  for (auto point : points_) {
    point_index[point] = index;
    index++;
  }

  typedef std::pair<size_t, size_t> Edge;
  std::map<Edge, G4int> edge_use_count;

  for (size_t i = 0; i < triangles_.size(); i++) {
    auto triangle = triangles_[i];

    size_t a = point_index[triangle->GetVertex(0)];
    size_t b = point_index[triangle->GetVertex(1)];
    size_t c = point_index[triangle->GetVertex(2)];

    if (a < b) {
      edge_use_count[Edge(a, b)] += 1;
    }

    else if (a > b) {
      edge_use_count[Edge(b, a)] += 1;
    }

    if (b < c) {
      edge_use_count[Edge(b, c)] += 1;
    }

    else if (b > c) {
      edge_use_count[Edge(c, b)] += 1;
    }

    if (c < a) {
      edge_use_count[Edge(c, a)] += 1;
    }

    else if (c > a) {
      edge_use_count[Edge(a, c)] += 1;
    }
  }

  for (auto count : edge_use_count) {
    if (count.second != 2) {
      return false;
    }
  }

  return true;
}
}

namespace CADMesh {

namespace File {

Reader::Reader(G4String reader_name) : name_(reader_name) {}

Reader::~Reader() {}

G4String Reader::GetName() { return name_; }

std::shared_ptr<Mesh> Reader::GetMesh() {
  if (meshes_.size() > 0) {
    return meshes_[0];
  }

  return nullptr;
}

std::shared_ptr<Mesh> Reader::GetMesh(size_t index) {
  if (index < meshes_.size()) {
    return meshes_[index];
  }

  Exceptions::MeshNotFound("Reader::GetMesh", index);

  return nullptr;
}

std::shared_ptr<Mesh> Reader::GetMesh(G4String name, G4bool exact) {
  for (auto mesh : meshes_) {
    if (exact) {
      if (mesh->GetName() == name)
        return mesh;
    }

    else {
      if (mesh->GetName().find(name) != std::string::npos)
        return mesh;
    }
  }

  Exceptions::MeshNotFound("Reader::GetMesh", name);

  return nullptr;
}

Meshes Reader::GetMeshes() { return meshes_; }

size_t Reader::GetNumberOfMeshes() { return meshes_.size(); }

size_t Reader::AddMesh(std::shared_ptr<Mesh> mesh) {
  meshes_.push_back(mesh);

  return meshes_.size();
}

void Reader::SetMeshes(Meshes meshes) { meshes_ = meshes; }
}
}

#include <fstream>
#include <sstream>
#include <streambuf>

namespace CADMesh {

namespace File {

Lexer::Lexer(std::string filepath, State *initial_state) {
  std::ifstream file(filepath);
  input_ = std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

  if (initial_state) {
    Run(initial_state);
  }
}

std::string Lexer::String() {
  return input_.substr(start_, position_ - start_);
}

void Lexer::Run(State *initial_state, size_t lines) {
  parent_item_ = new Item{ParentToken, position_,          line_, "", "",
                          nullptr,     std::vector<Item>()};

  state_ = initial_state;

  end_line_ = 0;

  if (lines > 0)
    end_line_ = line_ + lines;

  while (state_) {
    state_ = (*state_)(this);
  }
}

Items Lexer::GetItems() { return parent_item_->children; }

void Lexer::Backup() {
  position_ -= width_;

  auto next = input_.substr(position_, 1);

  if (next == "\n") {
    line_--;
  }
}

void Lexer::BackupTo(int position) {
  auto s = input_.substr(position, position_ - position);
  line_ -= std::count(s.begin(), s.end(), '\n');

  position_ = position;
}

std::string Lexer::Next() {
  if (position_ >= input_.length()) {
    return "";
  }

  auto next = input_.substr(position_, 1);

  width_ = 1;
  position_ += width_;

  if (next == "\n")
    line_++;

  return next;
}

std::string Lexer::Peek() {
  auto next = Next();

  if (next != "")
    Backup();

  return next;
}

void Lexer::Skip() { start_ = position_; }

Item *Lexer::ThisIsA(Token token, std::string error) {
  if (dry_run_)
    return nullptr;

  auto item =
      Item{token, position_, line_, String(), error, parent_item_, Items()};
  Skip();

  if (parent_item_) {
    PrintItem(item);

    parent_item_->children.push_back(item);
    return &(parent_item_->children.back());
  }

  else {
    depth_++;
    PrintItem(item);

    items_.push_back(item);
    return &(items_.back());
  }
}

Item *Lexer::StartOfA(Token token, std::string error) {
  if (dry_run_)
    return nullptr;

  parent_item_ = ThisIsA(token, error);

  depth_++;

  return parent_item_;
}

Item *Lexer::EndOfA(Token token, std::string /*error*/) {
  if (dry_run_)
    return nullptr;

  depth_--;

  PrintItem(*parent_item_);

  if (parent_item_->token.name != token.name) {
    Exceptions::LexerError("Lexer::EndOfA",
                           "Trying to end a '" + parent_item_->token.name +
                               "' with a '" + token.name + "' token.");
  }

  if (parent_item_) {
    parent_item_ = parent_item_->parent;
  }

  return nullptr;
}

Item *Lexer::MaybeEndOfA(Token token, std::string error) {
  if (parent_item_->token.name == token.name) {
    return EndOfA(token, error);
  }

  else {
    return nullptr;
  }
}

bool Lexer::OneOf(std::string possibles) {
  auto peek = Peek();

  size_t position = possibles.find(Peek());

  if (position != std::string::npos) {
    auto next = Next();
    return next != "";
  }

  return false;
}

bool Lexer::ManyOf(std::string possibles) {
  bool has = false;

  while (OneOf(possibles)) {
    has = true;
  }

  return has;
}

bool Lexer::Until(std::string match) {
  while (!OneOf(match)) {
    if (Next() == "")
      return false;
  }

  return true;
}

bool Lexer::MatchExactly(std::string match) {
  auto start_position = position_;

  for (auto m : match) {
    if (!OneOf(std::string(1, m))) {
      BackupTo(start_position);
      return false;
    }
  }

  return true;
}

bool Lexer::OneDigit() { return OneOf("0123456789"); }

bool Lexer::ManyDigits() { return ManyOf("0123456789"); }

bool Lexer::OneLetter() {
  return OneOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

bool Lexer::ManyLetters() {
  return ManyOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

bool Lexer::ManyCharacters() {
  return ManyOf("!\"#$%&\\\'()*+,-./"
                "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[|]^_`"
                "abcdefghijklmnopqrstuvwxyz{}~");
}

bool Lexer::Integer() {
  auto start_position = position_;

  OneOf("+-");

  if (!ManyDigits()) {
    BackupTo(start_position);
    return false;
  }

  return true;
}

bool Lexer::Float() {
  auto start_position = position_;

  OneOf("+-");

  bool has_integer = Integer();

  if (!OneOf(".")) {
    BackupTo(start_position);
    return false;
  }

  bool has_decimal = ManyDigits();

  if (!has_integer || !has_decimal) {
    BackupTo(start_position);
    return false;
  }

  return true;
}

bool Lexer::Number() {
  if (!Float()) {
    if (!Integer()) {
      return false;
    }
  }

  auto start_position = position_;

  if (OneOf("eE")) {
    if (!Float()) {
      if (!Integer()) {
        BackupTo(start_position);
      }
    }
  }

  return true;
}

bool Lexer::SkipWhiteSpace() {
  if (!ManyOf(" \t\r")) {
    Skip();
    return false;
  }

  Skip();
  return true;
}

bool Lexer::SkipLineBreak() {
  if (!OneOf("\n")) {
    return false;
  }

  Skip();
  return true;
}

bool Lexer::SkipLineBreaks() {
  if (!ManyOf("\n")) {
    return false;
  }

  Skip();
  return true;
}

bool Lexer::SkipLine() {
  if (!Until("\n")) {
    return false;
  }

  Skip();
  return true;
}

State *Lexer::Error(std::string message) {

  std::stringstream error;
  error << "Error around line " << line_ << ": " << message << std::endl;

  last_error_ = error.str();

  if (dry_run_)
    return nullptr;

  Item item{ErrorToken,  position_,    line_,  "",
            error.str(), parent_item_, Items()};
  items_.push_back(item);

  Exceptions::LexerError("Lexer", error.str());

  return nullptr;
}

State *Lexer::LastError() {
  if (last_error_ == "") {
    Exceptions::LexerError("Lexer", "Something went wrong.");
  }

  else {
    Exceptions::LexerError("Lexer", last_error_);
  }

  return nullptr;
}

bool Lexer::TestState(State *state) {
  if (dry_run_)
    return false;

  if (end_line_ > 0 && line_ > end_line_)
    return false;

  auto start_position = position_;

  dry_run_ = true;

  bool state_transition = ((*state)(this) != nullptr);

  dry_run_ = false;

  BackupTo(start_position);

  return state_transition;
}

bool Lexer::IsDryRun() { return dry_run_; }

size_t Lexer::LineNumber() { return line_; }

#ifdef CADMESH_LEXER_VERBOSE
void Lexer::PrintMessage(std::string name, std::string message) {
  std::cout << "Lexer::" << name << " : " << message << std::endl;
}
#else
void Lexer::PrintMessage(std::string, std::string) {}
#endif

#ifdef CADMESH_LEXER_VERBOSE
void Lexer::PrintItem(Item item) {
  auto depth = std::max(0, depth_) * 2;
  std::cout << std::string(depth, ' ') << item.token.name << ": " << item.value
            << std::endl;
}
#else
void Lexer::PrintItem(Item) {}
#endif
}
}

namespace CADMesh {

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate() : CADMeshTemplate<T>("") {}

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name)
    : CADMeshTemplate<T>(file_name, File::Unknown) {}

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name, File::Type file_type)
    : CADMeshTemplate<T>(file_name, file_type, File::CADMESH_DEFAULT_READER()) {
}

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(std::shared_ptr<File::Reader> reader)
    : CADMeshTemplate<T>("", reader) {}

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name,
                                    std::shared_ptr<File::Reader> reader)
    : CADMeshTemplate<T>(file_name, File::Unknown, reader) {}

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name, File::Type file_type,
                                    std::shared_ptr<File::Reader> reader) {
  if (!reader->CanRead(file_type)) {
    Exceptions::ReaderCantReadError(reader->GetName(), file_type, file_name);
  }

  file_name_ = file_name;
  file_type_ = file_type;

  scale_ = 1.0;

  offset_ = G4ThreeVector();

  verbose_ = 0;

  reader_ = reader;

  reader_->Read(file_name_);
}

template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::From(G4String file_name) {
  return std::make_shared<T>(file_name);
}

template <typename T>
std::shared_ptr<T>
CADMeshTemplate<T>::From(G4String file_name,
                         std::shared_ptr<File::Reader> reader) {
  return std::make_shared<T>(file_name, reader);
}

template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromPLY(G4String file_name) {
  return std::make_shared<T>(file_name, File::PLY);
}

template <typename T>
std::shared_ptr<T>
CADMeshTemplate<T>::FromPLY(G4String file_name,
                            std::shared_ptr<File::Reader> reader) {
  return std::make_shared<T>(file_name, File::PLY, reader);
}

template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromSTL(G4String file_name) {
  return std::make_shared<T>(file_name, File::STL);
}

template <typename T>
std::shared_ptr<T>
CADMeshTemplate<T>::FromSTL(G4String file_name,
                            std::shared_ptr<File::Reader> reader) {
  return std::make_shared<T>(file_name, File::STL, reader);
}

template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromOBJ(G4String file_name) {
  return std::make_shared<T>(file_name, File::OBJ);
}

template <typename T>
std::shared_ptr<T>
CADMeshTemplate<T>::FromOBJ(G4String file_name,
                            std::shared_ptr<File::Reader> reader) {
  return std::make_shared<T>(file_name, File::OBJ, reader);
}

template <typename T> CADMeshTemplate<T>::~CADMeshTemplate() {}

template <typename T> bool CADMeshTemplate<T>::IsValidForNavigation() {
  return reader_->GetMesh()->IsValidForNavigation();
}

template <typename T> G4String CADMeshTemplate<T>::GetFileName() {
  return file_name_;
}

template <typename T> File::Type CADMeshTemplate<T>::GetFileType() {
  return file_type_;
}

template <typename T> void CADMeshTemplate<T>::SetVerbose(G4int verbose) {
  verbose_ = verbose;
}

template <typename T> G4int CADMeshTemplate<T>::GetVerbose() {
  return verbose_;
}

template <typename T> void CADMeshTemplate<T>::SetScale(G4double scale) {
  scale_ = scale;
}

template <typename T> G4double CADMeshTemplate<T>::GetScale() { return scale_; }

template <typename T>
void CADMeshTemplate<T>::SetOffset(G4double x, G4double y, G4double z) {
  SetOffset(G4ThreeVector(x, y, z));
}

template <typename T> void CADMeshTemplate<T>::SetOffset(G4ThreeVector offset) {
  offset_ = offset;
}

template <typename T> G4ThreeVector CADMeshTemplate<T>::GetOffset() {
  return offset_;
}
}

namespace CADMesh {

namespace Exceptions {

void FileNotFound(G4String origin, G4String filepath) {
  G4Exception(
      ("CADMesh in " + origin).c_str(), "FileNotFound", FatalException,
      ("\nThe file: \n\t" + filepath + "\ncould not be found.").c_str());
}

void LexerError(G4String origin, G4String message) {
  G4Exception(
      ("CADMesh in " + origin).c_str(), "LexerError", FatalException,
      ("\nThe CAD file appears to contain incorrect syntax:\n\t" + message)
          .c_str());
}

void ParserError(G4String origin, G4String message) {
  G4Exception(("CADMesh in " + origin).c_str(), "ParserError", FatalException,
              ("\nThe CAD file appears to contain invalid data:\n\t" + message)
                  .c_str());
}

void ReaderCantReadError(G4String origin, File::Type file_type,
                         G4String filepath) {
  G4Exception(
      ("CADMesh in " + origin).c_str(), "ReaderCantReadError", FatalException,
      (G4String("\nThe the reader can't read files of type '") +
       File::TypeString[file_type] +
       ".'\n\tSpecified the incorrect file type (?) for file:\n\t\t" + filepath)
          .c_str());
}

void MeshNotFound(G4String origin, size_t index) {
  std::stringstream message;
  message << "\nThe mesh with index '" << index << "' could not be found.";

  G4Exception(("CADMesh in " + origin).c_str(), "MeshNotFound", FatalException,
              message.str().c_str());
}

void MeshNotFound(G4String origin, G4String name) {
  G4Exception(
      ("CADMesh in " + origin).c_str(), "MeshNotFound", FatalException,
      ("\nThe mesh with name '" + name + "' could not be found.").c_str());
}
}
}

#include "Randomize.hh"

namespace CADMesh {

G4VSolid *TessellatedMesh::GetSolid() {
  return (G4VSolid *)GetTessellatedSolid();
}

G4VSolid *TessellatedMesh::GetSolid(G4int index) {
  return (G4VSolid *)GetTessellatedSolid(index);
}

G4VSolid *TessellatedMesh::GetSolid(G4String name, G4bool exact) {
  return (G4VSolid *)GetTessellatedSolid(name, exact);
}

std::vector<G4VSolid *> TessellatedMesh::GetSolids() {
  std::vector<G4VSolid *> solids;

  for (auto mesh : reader_->GetMeshes()) {
    solids.push_back(GetTessellatedSolid(mesh));
  }

  return solids;
}

G4AssemblyVolume *TessellatedMesh::GetAssembly() {
  if (assembly_) {
    return assembly_;
  }

  for (auto mesh : reader_->GetMeshes()) {
    auto solid = GetTessellatedSolid(mesh);

    G4Material *material = nullptr;

    auto logical =
        new G4LogicalVolume(solid, material, mesh->GetName() + "_logical");

    G4ThreeVector position = G4ThreeVector();
    G4RotationMatrix *rotation = new G4RotationMatrix();

    assembly_->AddPlacedVolume(logical, position, rotation);
  }

  return assembly_;
}

G4TessellatedSolid *TessellatedMesh::GetTessellatedSolid() {
  return GetTessellatedSolid(0);
}

G4TessellatedSolid *TessellatedMesh::GetTessellatedSolid(G4int index) {
  return GetTessellatedSolid(reader_->GetMesh(index));
}

G4TessellatedSolid *TessellatedMesh::GetTessellatedSolid(G4String name,
                                                         G4bool exact) {
  return GetTessellatedSolid(reader_->GetMesh(name, exact));
}

G4TessellatedSolid *
TessellatedMesh::GetTessellatedSolid(std::shared_ptr<Mesh> mesh) {
  auto volume_solid = new G4TessellatedSolid(mesh->GetName());

  for (auto triangle : mesh->GetTriangles()) {
    auto a = triangle->GetVertex(0) * scale_ + offset_;
    auto b = triangle->GetVertex(1) * scale_ + offset_;
    auto c = triangle->GetVertex(2) * scale_ + offset_;

    auto t = new G4TriangularFacet(a, b, c, ABSOLUTE);

    if (reverse_) {
      volume_solid->AddFacet((G4VFacet *)t->GetFlippedFacet());
    }

    else {
      volume_solid->AddFacet((G4VFacet *)t);
    }
  }

  volume_solid->SetSolidClosed(true);
  if (volume_solid->GetNumberOfFacets() == 0) {
    G4Exception("TessellatedMesh::GetTessellatedSolid",
                "The loaded mesh has 0 faces.", FatalException,
                "The file may be empty.");
    return nullptr;
  }
  return volume_solid;
}
}

#ifdef USE_CADMESH_TETGEN

namespace CADMesh {

TetrahedralMesh::TetrahedralMesh() {}

TetrahedralMesh::~TetrahedralMesh() {}

G4VSolid *TetrahedralMesh::GetSolid() { return GetSolid(0); }

G4VSolid *TetrahedralMesh::GetSolid(G4int /*index*/) { return nullptr; }

G4VSolid *TetrahedralMesh::GetSolid(G4String /*name*/, G4bool /*exact*/) {

  return nullptr;
}

std::vector<G4VSolid *> TetrahedralMesh::GetSolids() {

  return std::vector<G4VSolid *>();
}

G4AssemblyVolume *TetrahedralMesh::GetAssembly() {
  if (assembly_) {
    return assembly_;
  }

  assembly_ = new G4AssemblyVolume();

  in_ = std::make_shared<tetgenio>();
  out_ = std::make_shared<tetgenio>();

  char *fn = (char *)file_name_.c_str();

  G4bool do_tet = true;

  if (file_type_ == File::STL) {
    in_->load_stl(fn);
  }

  else if (file_type_ == File::PLY) {
    in_->load_ply(fn);
  }

  else if (file_type_ == File::TET) {
    out_->load_tetmesh(fn, 0);
    do_tet = false;
  }

  else if (file_type_ == File::OFF) {
    out_->load_off(fn);
    do_tet = false;
  }

  if (do_tet) {
    tetgenbehavior *behavior = new tetgenbehavior();
    behavior->nobisect = 1;
    behavior->plc = 1;
    behavior->quality = quality_;

    tetrahedralize(behavior, in_.get(), out_.get());
  }

  G4RotationMatrix *element_rotation = new G4RotationMatrix();
  G4ThreeVector element_position = G4ThreeVector();
  G4Transform3D assembly_transform = G4Translate3D();

  for (int i = 0; i < out_->numberoftetrahedra; i++) {
    int index_offset = i * 4;

    G4ThreeVector p1 = GetTetPoint(index_offset);
    G4ThreeVector p2 = GetTetPoint(index_offset + 1);
    G4ThreeVector p3 = GetTetPoint(index_offset + 2);
    G4ThreeVector p4 = GetTetPoint(index_offset + 3);

    G4String tet_name =
        file_name_ + G4String("_tet_") + G4UIcommand::ConvertToString(i);

    auto tet_solid =
        new G4Tet(tet_name + G4String("_solid"), p1, p2, p3, p4, 0);

    auto tet_logical = new G4LogicalVolume(
        tet_solid, material_, tet_name + G4String("_logical"), 0, 0, 0);

    assembly_->AddPlacedVolume(tet_logical, element_position, element_rotation);
  }

  return assembly_;
}

G4ThreeVector TetrahedralMesh::GetTetPoint(G4int index_offset) {
  return G4ThreeVector(
      out_->pointlist[out_->tetrahedronlist[index_offset] * 3] * scale_ -
          offset_.x(),
      out_->pointlist[out_->tetrahedronlist[index_offset] * 3 + 1] * scale_ -
          offset_.y(),
      out_->pointlist[out_->tetrahedronlist[index_offset] * 3 + 2] * scale_ -
          offset_.z());
}
}
#endif

#define CADMeshLexerToken(name)                                                \
  static Token name##Token { #name }

#define CADMeshLexerStateDefinition(name)                                      \
  struct name##State : public State {                                          \
    State *operator()(Lexer *lexer) const;                                     \
  }

#define CADMeshLexerState(name) name##State::operator()(Lexer *lexer) const

#define ThisIsA(name) lexer->ThisIsA(name##Token)
#define StartOfA(name) lexer->StartOfA(name##Token)
#define EndOfA(name) lexer->EndOfA(name##Token)
#define MaybeEndOfA(name) lexer->MaybeEndOfA(name##Token)

#define Skip() lexer->Skip()

#define Next() lexer->Peek()
#define PrintNext() std::cout << lexer->Peek() << std::endl;

#define OneOf(possibles) lexer->OneOf(possibles)
#define NotOneOf(possibles) !OneOf(possibles)

#define ManyOf(possibles) lexer->ManyOf(possibles)
#define NotManyOf(possibles) !ManyOf(possibles)

#define Until(match) lexer->Until(match)

#define RestOfLine()                                                           \
  if (Until("\n\r"))                                                           \
  lexer->Backup()

#define MatchExactly(match) lexer->MatchExactly(match)
#define DoesNotMatchExactly(match) !MatchExactly(match)

#define OneDigit() lexer->OneDigit()
#define NotOneDigit() !OneDigit()

#define ManyDigits() lexer->ManyDigits()
#define NotManyDigits() !ManyDigits()

#define OneLetter() lexer->OneLetter()
#define NotOneLetter() !OneLetter()

#define ManyLetters() lexer->ManyLetters()
#define NotManyLetters() !ManyLetters()

#define ManyCharacters() lexer->ManyCharacters()
#define NotManyCharacters() !ManyCharacters()

#define Integer() lexer->Integer()
#define NotAnInteger() !Integer()

#define Float() lexer->Float()
#define NotAFloat() !Float()

#define Number() lexer->Number()
#define NotANumber() !Number()

#define SkipWhiteSpace() lexer->SkipWhiteSpace()
#define DidNotSkipWhiteSpace() !SkipWhiteSpace()

#define SkipLineBreak() lexer->SkipLineBreak()
#define DidNotSkipLineBreak() !SkipLineBreak()

#define SkipLineBreaks() lexer->SkipLineBreaks()
#define DidNotSkipLineBreaks() !SkipLineBreaks()

#define SkipLine() lexer->SkipLine()
#define DidNotSkipLine() !SkipLine()

#define AtEndOfLine() Next() == "\n" || Next() == "\r"

#define Error(message)                                                         \
  {                                                                            \
    lexer->Error(message);                                                     \
    return nullptr;                                                            \
  }

#define NextState(next) return new next##State()
#define TestState(next) lexer->TestState(new next##State())
#define TryState(next)                                                         \
  if (TestState(next))                                                         \
  NextState(next)
#define FinalState() return new __FinalState();

#define RunLexer(filepath, start) Lexer(filepath, new start##State).GetItems()

namespace CADMesh {

namespace File {

class STLReader : public Reader {
public:
  STLReader() : Reader("STLReader"){};

  G4bool Read(G4String filepath);
  G4bool CanRead(Type file_type);

protected:
  CADMeshLexerStateDefinition(StartSolid);
  CADMeshLexerStateDefinition(EndSolid);

  CADMeshLexerStateDefinition(StartFacet);
  CADMeshLexerStateDefinition(EndFacet);

  CADMeshLexerStateDefinition(StartVertices);
  CADMeshLexerStateDefinition(EndVertices);

  CADMeshLexerStateDefinition(Vertex);

  CADMeshLexerStateDefinition(ThreeVector);

  std::shared_ptr<Mesh> ParseMesh(Items items);
  G4TriangularFacet *ParseFacet(Items items);
  G4TriangularFacet *ParseVertices(Items items);
  G4ThreeVector ParseThreeVector(Items items);
};
}
}

namespace CADMesh {

namespace File {

class OBJReader : public Reader {
public:
  OBJReader() : Reader("OBJReader"){};

  G4bool Read(G4String filepath);
  G4bool CanRead(Type file_type);

protected:
  CADMeshLexerStateDefinition(StartSolid);
  CADMeshLexerStateDefinition(EndSolid);

  CADMeshLexerStateDefinition(Ignore);
  CADMeshLexerStateDefinition(Vertex);
  CADMeshLexerStateDefinition(Facet);
  CADMeshLexerStateDefinition(Object);

  std::shared_ptr<Mesh> ParseMesh(Items items);
  G4ThreeVector ParseVertex(Items items);
  G4TriangularFacet *ParseFacet(Items items, G4bool quad);

private:
  Points vertices_;
};
}
}

namespace CADMesh {

namespace File {

CADMeshLexerToken(Header);
CADMeshLexerToken(Element);
CADMeshLexerToken(Property);

class PLYReader : public Reader {
public:
  PLYReader() : Reader("PLYReader"){};

  G4bool Read(G4String filepath);
  G4bool CanRead(Type file_type);

protected:
  CADMeshLexerStateDefinition(StartHeader);
  CADMeshLexerStateDefinition(EndHeader);

  CADMeshLexerStateDefinition(Element);
  CADMeshLexerStateDefinition(Property);
  CADMeshLexerStateDefinition(Ignore);

  CADMeshLexerStateDefinition(Vertex);
  CADMeshLexerStateDefinition(Facet);

  void ParseHeader(Items items);

  std::shared_ptr<Mesh> ParseMesh(Items vertex_items, Items face_items);
  G4ThreeVector ParseVertex(Items items);
  G4TriangularFacet *ParseFacet(Items items, Points vertices);

  size_t vertex_count_ = 0;
  size_t facet_count_ = 0;

  size_t x_index_ = 0;
  size_t y_index_ = 0;
  size_t z_index_ = 0;

  size_t facet_index_ = 0;
};
}
}

namespace CADMesh {

namespace File {

State *STLReader::CADMeshLexerState(StartSolid) {
  if (DoesNotMatchExactly("solid"))
    Error("STL files start with 'solid'. Make sure you are using an ASCII STL "
          "file.");

  SkipWhiteSpace();

  RestOfLine();

  StartOfA(Solid);

  SkipLineBreak();
  NextState(StartFacet);
}

State *STLReader::CADMeshLexerState(EndSolid) {
  SkipWhiteSpace();
  SkipLineBreaks();
  SkipWhiteSpace();

  if (DoesNotMatchExactly("endsolid"))
    Error("STL files end with 'endsolid'.");

  Skip();
  EndOfA(Solid);

  FinalState();
}

State *STLReader::CADMeshLexerState(StartFacet) {
  SkipWhiteSpace();
  SkipLineBreaks();
  SkipWhiteSpace();

  if (DoesNotMatchExactly("facet normal"))
    Error("Facets are indicated by the tag 'facet normal'.");

  SkipWhiteSpace();

  StartOfA(Facet);

  SkipLine();

  NextState(StartVertices);
}

State *STLReader::CADMeshLexerState(EndFacet) {
  SkipWhiteSpace();
  SkipLineBreaks();
  SkipWhiteSpace();

  if (DoesNotMatchExactly("endfacet"))
    Error("The end of a facets is indicated by the tag 'endfacet'.");

  SkipWhiteSpace();
  SkipLineBreak();

  EndOfA(Facet);

  TryState(StartFacet);

  NextState(EndSolid);
}

State *STLReader::CADMeshLexerState(StartVertices) {
  SkipWhiteSpace();
  SkipLineBreaks();
  SkipWhiteSpace();

  if (DoesNotMatchExactly("outer loop"))
    Error("The start of the vertices is indicated by the tag 'outer loop'.");

  SkipWhiteSpace();
  SkipLineBreak();

  StartOfA(Vertices);

  NextState(Vertex);
}

State *STLReader::CADMeshLexerState(EndVertices) {
  SkipWhiteSpace();
  SkipLineBreaks();
  SkipWhiteSpace();

  if (DoesNotMatchExactly("endloop"))
    Error("The end of the vertices is indicated by the tag 'endloop'.");

  SkipWhiteSpace();
  SkipLineBreak();

  EndOfA(Vertices);

  NextState(EndFacet);
}

State *STLReader::CADMeshLexerState(Vertex) {
  SkipWhiteSpace();
  SkipLineBreaks();
  SkipWhiteSpace();

  if (DoesNotMatchExactly("vertex"))
    Error("A vertex is indicated by the tag 'vertex'.");

  SkipWhiteSpace();

  NextState(ThreeVector);
}

State *STLReader::CADMeshLexerState(ThreeVector) {
  SkipWhiteSpace();

  StartOfA(ThreeVector);

  if (NotANumber())
    Error("First number in three vector not found.");

  ThisIsA(Number);
  SkipWhiteSpace();

  if (NotANumber())
    Error("Second number in three vector not found.");

  ThisIsA(Number);
  SkipWhiteSpace();

  if (NotANumber())
    Error("Third number in three vector not found.");

  ThisIsA(Number);
  EndOfA(ThreeVector);

  SkipWhiteSpace();

  if (DidNotSkipLineBreak())
    Error("Expecting a new line at the end of a three vector.");

  TryState(StartVertices);

  TryState(Vertex);

  NextState(EndVertices);
}

G4bool STLReader::Read(G4String filepath) {
  auto items = RunLexer(filepath, StartSolid);

  if (items.size() == 0) {
    Exceptions::ParserError("STLReader::Read",
                            "The STL file appears to be empty.");
  }

  for (auto item : items) {
    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The mesh appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("STLReader::Read", error.str());
    }

    auto mesh = ParseMesh(item.children);

    auto name = G4String(item.value);
    AddMesh(Mesh::New(mesh, name));
  }

  return true;
}

G4bool STLReader::CanRead(Type file_type) { return (file_type == STL); }

std::shared_ptr<Mesh> STLReader::ParseMesh(Items items) {
  Triangles triangles;

  for (auto item : items) {
    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The facet appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("STLReader::Mesh", error.str());
    }

    triangles.push_back(ParseFacet(item.children));
  }

  return Mesh::New(triangles);
}

G4TriangularFacet *STLReader::ParseFacet(Items items) {
  Triangles triangles;

  for (auto item : items) {
    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The vertex appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("STLReader::ParseFacet", error.str());
    }

    triangles.push_back(ParseVertices(item.children));
  }

  if (triangles.size() != 1) {
    std::stringstream error;
    error << "STL files expect exactly 1 triangle per facet.";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("STLReader::ParseFacet", error.str());
  }

  return triangles[0];
}

G4TriangularFacet *STLReader::ParseVertices(Items items) {
  std::vector<G4ThreeVector> vertices;

  for (auto item : items) {
    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The vertex appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("STLReader::ParseVertices", error.str());
    }

    vertices.push_back(ParseThreeVector(item.children));
  }

  if (vertices.size() != 3) {
    std::stringstream error;
    error << "STL files expect exactly 3 vertices for a triangular facet. ";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("STLReader::ParseVertices", error.str());
  }

  return new G4TriangularFacet(vertices[0], vertices[1], vertices[2], ABSOLUTE);
}

G4ThreeVector STLReader::ParseThreeVector(Items items) {
  std::vector<double> numbers;

  for (auto item : items) {
    numbers.push_back((double)atof(item.value.c_str()));
  }

  if (numbers.size() != 3) {
    std::stringstream error;
    error << "Three vectors in STL files require exactly 3 numbers";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("STLReader::ParseThreeVector", error.str());
  }

  return G4ThreeVector(numbers[0], numbers[1], numbers[2]);
}
}
}

namespace CADMesh {

namespace File {

State *OBJReader::CADMeshLexerState(StartSolid) {
  StartOfA(Solid);

  TryState(Object);
  TryState(Vertex);
  TryState(Ignore);

  Error("Invalid element tag.");
}

State *OBJReader::CADMeshLexerState(EndSolid) {
  if (Next() != "")
    lexer->LastError();

  EndOfA(Solid);
  FinalState();
}

State *OBJReader::CADMeshLexerState(Ignore) {
  if (DidNotSkipLine())
    NextState(EndSolid);

  TryState(Object);
  TryState(Vertex);
  TryState(Facet);
  TryState(Ignore);

  NextState(EndSolid);
}

State *OBJReader::CADMeshLexerState(Vertex) {
  SkipLineBreaks();

  if (DoesNotMatchExactly("v "))
    Error("A vertex is indicated by the tag 'v'.");

  SkipWhiteSpace();
  StartOfA(Vertex);

  if (NotANumber())
    Error("First number in three vector not found.");

  ThisIsA(Number);
  SkipWhiteSpace();

  if (NotANumber())
    Error("Second number in three vector not found.");

  ThisIsA(Number);
  SkipWhiteSpace();

  if (NotANumber())
    Error("Third number in three vector not found.");

  ThisIsA(Number);

  EndOfA(Vertex);

  SkipLine();

  TryState(Vertex);
  TryState(Object);
  TryState(Facet);
  TryState(Ignore);

  NextState(EndSolid);
}

State *OBJReader::CADMeshLexerState(Facet) {
  SkipLineBreaks();

  if (DoesNotMatchExactly("f "))
    Error("A facet is indicated by the tag 'f'.");

  SkipWhiteSpace();
  StartOfA(Facet);

  if (NotANumber())
    Error("First number in three vector not found.");

  ThisIsA(Number);

  OneOf("/");
  Number();
  OneOf("/");
  Number();
  SkipWhiteSpace();

  if (NotANumber())
    Error("Second number in three vector not found.");

  ThisIsA(Number);

  OneOf("/");
  Number();
  OneOf("/");
  Number();
  SkipWhiteSpace();

  if (NotANumber())
    Error("Third number in three vector not found.");

  ThisIsA(Number);

  OneOf("/");
  Number();
  OneOf("/");
  Number();
  SkipWhiteSpace();

  if (Number())
    ThisIsA(Number);

  EndOfA(Facet);

  SkipLine();

  TryState(Facet);
  TryState(Vertex);
  TryState(Object);
  TryState(Ignore);

  NextState(EndSolid);
}

State *OBJReader::CADMeshLexerState(Object) {
  SkipLineBreaks();

  if (DoesNotMatchExactly("o "))
    Error("An object is indicated by the tag 'o'.");

  EndOfA(Solid);

  SkipWhiteSpace();
  StartOfA(Solid);

  ManyCharacters();
  ThisIsA(Word);
  SkipWhiteSpace();

  TryState(Vertex);
  TryState(Facet);
  TryState(Object);
  TryState(Ignore);

  NextState(EndSolid);
}

G4bool OBJReader::Read(G4String filepath) {
  auto items = RunLexer(filepath, StartSolid);

  if (items.size() == 0) {
    Exceptions::ParserError("OBJReader::Read",
                            "The OBJ file appears to be empty.");
  }

  for (auto item : items) {
    if (item.children.size() == 0) {
      continue;
    }

    auto mesh = ParseMesh(item.children);

    if (mesh->GetTriangles().size() == 0) {
      continue;
    }

    G4String name;

    if (item.children[0].token == WordToken) {
      name = item.children[0].value;
    }

    AddMesh(Mesh::New(mesh, name));
  }

  return true;
}

G4bool OBJReader::CanRead(Type file_type) { return (file_type == OBJ); }

std::shared_ptr<Mesh> OBJReader::ParseMesh(Items items) {
  Triangles facets;

  for (auto item : items) {
    if (item.token != VertexToken) {
      continue;
    }

    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The vertex appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("OBJReader::Mesh", error.str());
    }

    vertices_.push_back(ParseVertex(item.children));
  }

  for (auto item : items) {
    if (item.token != FacetToken) {
      continue;
    }

    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The facet appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("OBJReader::Mesh", error.str());
    }

    facets.push_back(ParseFacet(item.children, false));

    if (item.children.size() == 4) {
      facets.push_back(ParseFacet(item.children, true));
    }
  }

  return Mesh::New(facets);
}

G4ThreeVector OBJReader::ParseVertex(Items items) {
  std::vector<double> numbers;

  for (auto item : items) {
    numbers.push_back((double)atof(item.value.c_str()));
  }

  if (numbers.size() != 3) {
    std::stringstream error;
    error << "Three vectors in OBJ files require exactly 3 numbers";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("OBJReader::ParseThreeVector", error.str());
  }

  return G4ThreeVector(numbers[0], numbers[1], numbers[2]);
}

G4TriangularFacet *OBJReader::ParseFacet(Items items, G4bool quad) {
  std::vector<int> indices;

  for (auto item : items) {
    indices.push_back((int)atoi(item.value.c_str()));
  }

  if (indices.size() < 3) {
    std::stringstream error;
    error << "Facets in OBJ files require at least 3 indicies";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("OBJReader::ParseFacet", error.str());
  }

  if (quad && indices.size() != 4) {
    std::stringstream error;
    error << "Trying to triangle-ify a facet that isn't a quad";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("OBJReader::ParseFacet", error.str());
  }

  if (quad) {
    return new G4TriangularFacet(vertices_[indices[0] - 1],
                                 vertices_[indices[2] - 1],
                                 vertices_[indices[3] - 1], ABSOLUTE);
  }

  else {
    return new G4TriangularFacet(vertices_[indices[0] - 1],
                                 vertices_[indices[1] - 1],
                                 vertices_[indices[2] - 1], ABSOLUTE);
  }
}
}
}

namespace CADMesh {

namespace File {

State *PLYReader::CADMeshLexerState(StartHeader) {
  if (DoesNotMatchExactly("ply"))
    Error("PLY files start with 'ply'.");

  StartOfA(Header);

  SkipLine();

  TryState(Element);
  TryState(Ignore);

  Error("Invalid header tag.");
}

State *PLYReader::CADMeshLexerState(EndHeader) {
  if (DoesNotMatchExactly("end_header"))
    Error("PLY file headers end with 'end_header'.");

  MaybeEndOfA(Element);

  EndOfA(Header);
  FinalState();
}

State *PLYReader::CADMeshLexerState(Element) {
  if (DoesNotMatchExactly("element "))
    Error("An element is indicated by the tag 'element'.");

  MaybeEndOfA(Element);

  SkipWhiteSpace();
  StartOfA(Element);

  if (NotManyCharacters())
    Error("Element type not found.");

  ThisIsA(Word);
  SkipWhiteSpace();

  if (NotANumber())
    Error("Element count not found.");

  ThisIsA(Number);
  SkipLine();

  TryState(Property);
  TryState(Ignore);

  NextState(EndHeader);
}

State *PLYReader::CADMeshLexerState(Property) {
  if (DoesNotMatchExactly("property "))
    Error("An property is indicated by the tag 'property'.");

  SkipWhiteSpace();
  StartOfA(Property);

  if (NotManyCharacters())
    Error("Element type not found.");

  ThisIsA(Word);
  SkipWhiteSpace();

  RestOfLine();
  ThisIsA(Word);

  EndOfA(Property);

  SkipLineBreak();

  TryState(Property);
  TryState(Element);
  TryState(EndHeader);
  TryState(Ignore);

  NextState(EndHeader);
}

State *PLYReader::CADMeshLexerState(Ignore) {
  if (DidNotSkipLine())
    NextState(EndHeader);

  TryState(Element);
  TryState(Property);
  TryState(EndHeader);

  NextState(Ignore);
}

State *PLYReader::CADMeshLexerState(Vertex) {
  SkipLineBreaks();
  SkipWhiteSpace();
  SkipLineBreaks();

  StartOfA(Vertex);

  size_t i = 0;

  while (i < 32) {
    if (AtEndOfLine())
      break;

    SkipWhiteSpace();

    if (NotANumber())
      Error("Expecting only numbers in the vertex specification.");

    ThisIsA(Number);
    SkipWhiteSpace();

    i++;
  }

  EndOfA(Vertex);

  SkipLine();

  TryState(Vertex);

  FinalState();
}

State *PLYReader::CADMeshLexerState(Facet) {
  SkipLineBreaks();
  SkipWhiteSpace();
  SkipLineBreaks();

  StartOfA(Facet);

  size_t i = 0;

  while (i < 32) {
    if (AtEndOfLine())
      break;

    SkipWhiteSpace();

    if (NotANumber())
      Error("Expecting only numbers in the facet specification.");

    ThisIsA(Number);
    SkipWhiteSpace();

    i++;
  }

  EndOfA(Facet);

  SkipLine();

  TryState(Facet);

  FinalState();
}

G4bool PLYReader::Read(G4String filepath) {
  auto lexer = Lexer(filepath, new StartHeaderState);
  auto items = lexer.GetItems();

  if (items.size() == 0) {
    std::stringstream error;
    error << "The header appears to be empty.";

    Exceptions::ParserError("PLYReader::Read", error.str());
  }

  ParseHeader(items);

  lexer.Run(new VertexState, vertex_count_);
  auto vertex_items = lexer.GetItems();

  if (vertex_items.size() == 0) {
    Exceptions::ParserError("PLYReader::Read",
                            "The PLY file appears to have no vertices.");
  }

  if (vertex_items.size() != vertex_count_) {
    Exceptions::ParserError("PLYReader::Read",
                            "The PLY file appears to be missing vertices.");
  }

  lexer.Run(new FacetState, facet_count_);
  auto face_items = lexer.GetItems();

  if (face_items.size() == 0) {
    Exceptions::ParserError("PLYReader::Read",
                            "The PLY file appears to have no facets.");
  }

  if (face_items.size() != facet_count_) {
    Exceptions::ParserError("PLYReader::Read",
                            "The PLY file appears to be missing facets");
  }

  auto mesh = ParseMesh(vertex_items, face_items);
  AddMesh(Mesh::New(mesh));

  return true;
}

G4bool PLYReader::CanRead(Type file_type) { return (file_type == PLY); }

void PLYReader::ParseHeader(Items items) {
  if (items.size() != 1) {
    std::stringstream error;
    error << "The header appears to be invalid or missing."
          << "Error around line 1.";

    Exceptions::ParserError("PLYReader::ParseHeader", error.str());
  }

  for (auto item : items[0].children) {
    if (item.token == ElementToken) {
      if (item.children.size() < 2) {
        std::stringstream error;
        error << "Invalid element information in header. Expecting 'vertex' or "
                 "'face' and a number."
              << "Error around line " << item.line << ".";

        Exceptions::ParserError("PLYReader::ParseHeader", error.str());
      }

      if (item.children[0].token == WordToken &&
          item.children[1].token == NumberToken) {
        if (item.children[0].value == "vertex") {
          vertex_count_ = atoi(item.children[1].value.c_str());

          for (size_t i = 2; i < item.children.size(); i++) {
            auto property = item.children[i];

            if (property.children.size() > 1) {
              if (property.children[1].token == WordToken) {
                if (property.children[1].value == "x") {
                  x_index_ = i - 2;
                }

                if (property.children[1].value == "y") {
                  y_index_ = i - 2;
                }

                if (property.children[1].value == "z") {
                  z_index_ = i - 2;
                }
              }
            }
          }
        }

        else if (item.children[0].value == "face") {
          facet_count_ = atoi(item.children[1].value.c_str());

          for (size_t i = 2; i < item.children.size(); i++) {
            auto property = item.children[i];

            if (property.children.size() > 1) {
              if (property.children[1].token == WordToken) {
                if (property.children[1].value == "uchar int vertex_indices") {
                  facet_index_ = i - 2;
                }
              }
            }
          }
        }
      }
    }
  }

  if (vertex_count_ == 0) {
    std::stringstream error;
    error << "The number of vertices was not found in the header.";

    Exceptions::ParserError("PLYReader::ParseHeader", error.str());
  }

  if (facet_count_ == 0) {
    std::stringstream error;
    error << "The number of faces was not found in the header.";

    Exceptions::ParserError("PLYReader::ParseHeader", error.str());
  }

  if ((x_index_ == y_index_) || (y_index_ == z_index_) ||
      (x_index_ == z_index_)) {
    std::stringstream error;
    error << "The vertex x, y, z indices were not found in the header.";

    Exceptions::ParserError("PLYReader::ParseHeader", error.str());
  }
}

std::shared_ptr<Mesh> PLYReader::ParseMesh(Items vertex_items,
                                           Items face_items) {
  Points vertices;
  Triangles facets;

  for (auto item : vertex_items) {
    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The vertex appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("PLYReader::ParseMesh", error.str());
    }

    if (item.token == VertexToken) {
      vertices.push_back(ParseVertex(item.children));
    }
  }

  for (auto item : face_items) {
    if (item.children.size() == 0) {
      std::stringstream error;
      error << "The facet appears to be empty."
            << "Error around line " << item.line << ".";

      Exceptions::ParserError("PLYReader::Mesh", error.str());
    }

    if (item.token == FacetToken) {
      facets.push_back(ParseFacet(item.children, vertices));
    }
  }

  return Mesh::New(facets);
}

G4ThreeVector PLYReader::ParseVertex(Items items) {
  std::vector<double> numbers;

  for (auto item : items) {
    numbers.push_back((double)atof(item.value.c_str()));
  }

  if (numbers.size() < 3) {
    std::stringstream error;
    error << "Vertices in PLY files require atleast 3 numbers. ";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("PLYReader::ParseVertex", error.str());
  }

  return G4ThreeVector(numbers[x_index_], numbers[y_index_], numbers[z_index_]);
}

G4TriangularFacet *PLYReader::ParseFacet(Items items, Points vertices) {
  std::vector<int> indices;

  for (auto item : items) {
    indices.push_back((int)atoi(item.value.c_str()));
  }

  if (indices.size() < 4) {
    std::stringstream error;
    error << "Facets in PLY files require 3 indicies";

    if (items.size() != 0) {
      error << "Error around line " << items[0].line << ".";
    }

    Exceptions::ParserError("PLYReader::ParseFacet", error.str());
  }

  return new G4TriangularFacet(vertices[indices[1 + facet_index_]],
                               vertices[indices[2 + facet_index_]],
                               vertices[indices[3 + facet_index_]], ABSOLUTE);
}
}
}

#ifdef USE_CADMESH_ASSIMP_READER

namespace CADMesh {

namespace File {

ASSIMPReader::ASSIMPReader() : Reader("ASSIMPReader") {
  importer_ = new Assimp::Importer();
}

ASSIMPReader::~ASSIMPReader() { delete importer_; }

G4bool ASSIMPReader::Read(G4String filepath) {
  auto scene = importer_->ReadFile(filepath.c_str(),
                                   aiProcess_Triangulate |
                                       aiProcess_JoinIdenticalVertices |
                                       aiProcess_CalcTangentSpace);

  if (!scene) {
    Exceptions::FileNotFound("ASSIMP::Read", filepath);
    return false;
  }

  for (unsigned int index = 0; index < scene->mNumMeshes; index++) {
    aiMesh *mesh = scene->mMeshes[index];
    auto name = mesh->mName.C_Str();

    Triangles triangles;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      const aiFace &face = mesh->mFaces[i];

      G4ThreeVector a(mesh->mVertices[face.mIndices[0]].x,
                      mesh->mVertices[face.mIndices[0]].y,
                      mesh->mVertices[face.mIndices[0]].z);

      G4ThreeVector b(mesh->mVertices[face.mIndices[1]].x,
                      mesh->mVertices[face.mIndices[1]].y,
                      mesh->mVertices[face.mIndices[1]].z);

      G4ThreeVector c(mesh->mVertices[face.mIndices[2]].x,
                      mesh->mVertices[face.mIndices[2]].y,
                      mesh->mVertices[face.mIndices[2]].z);

      triangles.push_back(new G4TriangularFacet(a, b, c, ABSOLUTE));
    }

    AddMesh(Mesh::New(triangles, name));
  }

  return true;
}

G4bool ASSIMPReader::CanRead(Type /*file_type*/) { return true; }

std::shared_ptr<ASSIMPReader> ASSIMP() {
  return std::make_shared<ASSIMPReader>();
}
}
}
#endif

namespace CADMesh {

namespace File {

G4bool BuiltInReader::Read(G4String filepath) {
  File::Reader *reader = nullptr;

  auto type = TypeFromName(filepath);

  if (type == STL) {
    reader = new File::STLReader();
  }

  else if (type == OBJ) {
    reader = new File::OBJReader();
  }

  else if (type == PLY) {
    reader = new File::PLYReader();
  }

  else {
    Exceptions::ReaderCantReadError("BuildInReader::Read", type, filepath);
  }

  if (!reader->Read(filepath)) {
    return false;
  }

  SetMeshes(reader->GetMeshes());
  return true;
}

G4bool BuiltInReader::CanRead(Type type) {
  return type == STL || type == OBJ || type == PLY;
}

std::shared_ptr<BuiltInReader> BuiltIn() {
  return std::make_shared<BuiltInReader>();
}
}
}
