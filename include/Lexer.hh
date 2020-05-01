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

#pragma once


// STL //
#include <iostream>
#include <string>
#include <vector>


namespace CADMesh
{

namespace File
{


struct Token 
{
    std::string name;

    bool operator==(Token other) { return name == other.name; };
    bool operator!=(Token other) { return name != other.name; };
};

// Error tokens.
static Token ErrorToken { "ErrorToken" };
static Token EnfOfFileToken { "EndOfFileToken" };

// Generic tokens.
static Token ParentToken { "Parent" };
static Token WordToken { "Word" };
static Token NumberToken { "Number" };
static Token ThreeVectorToken { "ThreeVector" };
static Token VertexToken { "Vertex" };
static Token VerticesToken { "Vertices" };
static Token FacetToken { "Facet" };
static Token LineToken { "Line" };
static Token NormalToken { "Normal" };
static Token TextureToken { "Texture" };
static Token SolidToken { "Solid" };
static Token ObjectToken { "Object" };
static Token CommentToken { "Comment" };
static Token BlankLineToken { "BlankLine" };


struct Item
{
    Token       token;

    size_t      position;
    size_t      line;

    std::string value;
    std::string error;

    Item*       parent;

    std::vector<Item> children;
};


typedef std::vector<Item> Items;
typedef Items::iterator ItemsIterator;


class Lexer;


struct State
{
    virtual State* operator()(Lexer*) const = 0;
};


struct __FinalState : public State
{
    State* operator()(Lexer*) const
    {
        return nullptr;
    }
};


class Lexer
{
  public:
    Lexer(std::string filepath, State* initial_state = nullptr);

  public:
    std::string String();

    void Run(State* initial_state, size_t lines = 0);
    Items GetItems();

    void Backup();
    void BackupTo(int position);

    std::string Next();
    std::string Peek();

    void Skip();

    Item* ThisIsA(Token token, std::string error = "");
    Item* StartOfA(Token token, std::string error = "");
    Item* EndOfA(Token token, std::string error = "");
    Item* MaybeEndOfA(Token token, std::string error = "");

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

    State* Error(std::string message);
    State* LastError();

    bool TestState(State* state);

    bool IsDryRun();

    void PrintMessage(std::string name, std::string message);
    void PrintItem(Item item);

    size_t LineNumber();

  private:
    State* state_;

    Item* parent_item_ = nullptr; 
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


