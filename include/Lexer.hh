/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      CADMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

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
    std::string name = "";

    bool operator==(Token other) { return name == other.name; };
};

static Token ErrorToken{ "ErrorToken" };
static Token EndOfFileToken{ "EndOfFileToken" };

struct Item
{
    Token       token;

    size_t      position = 0;
    size_t      line     = 0;

    std::string value    = "";
    std::string error    = "";

    Item*       parent   = nullptr;

    std::vector<Item> children = {};
};


typedef std::vector<Item> Items;
typedef Items::iterator ItemsIterator;

class Lexer;

struct State
{
    virtual State* operator()(Lexer*) const = 0;
};


class Lexer
{
  public:
    Lexer(std::string filepath);

  public:
    std::string String();

    void Run(State* initial_state);
    Items GetItems();

    void Backup();
    void BackupTo(int position);

    std::string Next();
    std::string Peek();

    void Skip();

    Item* ThisIsA(Token token, std::string error = "");
    Item* StartOfA(Token token, std::string error = "");
    Item* EndOfA(Token token, std::string error = "");

    bool OneOf(std::string possibles);
    bool ManyOf(std::string possibles);
    bool MatchExactly(std::string match);

    bool OneDigit();
    bool ManyDigits();

    bool OneLetter();
    bool ManyLetters();

    bool Integer();
    bool Float();
    bool Number();

    bool SkipWhiteSpace();
    bool SkipLineBreak();

    State* Error(std::string message);

    bool TestState(State* state);

  private:
    State* state_;

    Item* parent_item_ = nullptr; 
    Items items_;

    std::string input_;

    size_t position_ = 0;
    size_t line_ = 0;
    size_t start_ = 0;
    size_t width_ = 0;

    bool dry_run_ = false;
};

}

}

