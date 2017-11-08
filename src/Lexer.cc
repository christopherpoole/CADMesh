
// CADMesh //
#include "Lexer.hh"
#include "Exceptions.hh"

// STL //
#include <iostream>
#include <sstream>
#include <fstream>
#include <streambuf>


namespace CADMesh
{

namespace File
{


Lexer::Lexer(std::string filepath)
{
    std::ifstream file(filepath);
    input_ = std::string( (std::istreambuf_iterator<char>(file))
                        , std::istreambuf_iterator<char>());
}


std::string Lexer::String()
{
    return input_.substr(start_, position_ - start_);
}


void Lexer::Run(State* initial_state)
{
    state_ = initial_state;

    while (state_)
    {
        state_ = (*state_)(this);
    }
}


Items Lexer::GetItems()
{
    return items_;
}


void Lexer::Backup()
{
    position_ -= width_;
}


void Lexer::BackupTo(int position)
{
    position_ = position;
}


std::string Lexer::Next()
{
    if (position_ >= input_.length())
    {
        return "";
    }

    // TODO: assuming width of 1.
    auto next = input_.substr(position_, 1);

    width_ = 1;
    position_ += width_;

    return next; 
}


std::string Lexer::Peek()
{
    auto next = Next();
    Backup();

    return next;
}


void Lexer::Skip()
{
    start_ = position_;
}


Item* Lexer::ThisIsA(Token token, std::string error)
{
    if (dry_run_) return nullptr;

    auto item = Item { token, position_, line_, String(), error, parent_item_ };
    Skip();

    if (parent_item_)
    {
        parent_item_->children.push_back(item);
        return &(parent_item_->children.back());
    }

    else
    {
        items_.push_back(item);
        return &(items_.back());
    }
}


Item* Lexer::StartOfA(Token token, std::string error)
{
    if (dry_run_) return nullptr;

    parent_item_ = ThisIsA(token, error);

    return parent_item_;
}


Item* Lexer::EndOfA(Token /*token*/, std::string /*error*/)
{
    // TODO: Check that we are ending a token of the right kind.

    if (dry_run_) return nullptr;

    if (parent_item_)
    {
        parent_item_ = parent_item_->parent;
    }

    return nullptr;
}


bool Lexer::OneOf(std::string possibles)
{
    size_t position = possibles.find(Peek());

    if (position != std::string::npos)
    {
        Next();
        return true;
    }

    return false;
}


bool Lexer::ManyOf(std::string possibles)
{
    bool has = false;

    while (OneOf(possibles))
    {
        has = true;
    }

    return has;
}


bool Lexer::MatchExactly(std::string match)
{
    auto start_position = position_;

    for (auto m : match)
    {
        if (!OneOf(std::string(1, m)))
        {
            BackupTo(start_position);
            return false;
        }
    }

    return true;
}


bool Lexer::OneDigit()
{
    return OneOf("0123456789");
}


bool Lexer::ManyDigits()
{
    return ManyOf("0123456789");
}


bool Lexer::OneLetter()
{
    return OneOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}


bool Lexer::ManyLetters()
{
    return ManyOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}


bool Lexer::Integer()
{
    auto start_position = position_;

    // The optional sign.
    OneOf("+-");

    // Try and consume some numbers, otherwise backup.
    if (!ManyDigits())
    {
        BackupTo(start_position);
        return false;
    }

    return true;
}


bool Lexer::Float()
{
    auto start_position = position_;

    // The optional sign;
    OneOf("+-");

    // The integer part.
    bool has_integer = Integer();

    // A demcimal point is required for a float.
    if (!OneOf("."))
    {
        BackupTo(start_position);
        return false;
    }

    // The decimal part.
    bool has_decimal = ManyDigits();

    if (!has_integer || !has_decimal)
    {
        BackupTo(start_position);
        return false;
    }

    return true;
}


bool Lexer::Number()
{
    // Try for a float or int.
    if (!Float())
    {
        if (!Integer())
        {
            return false;
        }
    }

    auto start_position = position_;
    
    // We might have scientific notation.
    if (OneOf("eE"))
    {
        // Try and consume a flot or int again.
        if (!Float())
        {
            if (!Integer())
            {
                // An e or E follow a number, but it isn't an exponent.
                BackupTo(start_position);
            }
        }
    }

    return true;
}


bool Lexer::SkipWhiteSpace()
{
    if (!ManyOf(" \t"))
    {
        return false;
    }

    Skip();
    return true;
}


bool Lexer::SkipLineBreak()
{
    if (!OneOf("\n\r"))
    {
        return false;
    }

    line_++;
        
    Skip();
    return true;
}


State* Lexer::Error(std::string message)
{
    if (dry_run_) return nullptr;

    std::stringstream error;
    error << "Error on line " << line_  << ": " << message << std::endl;
    
    Item item { ErrorToken, position_, line_, "", error.str(), parent_item_ };
    items_.push_back(item);

    Exceptions::LexerError("Lexer", error.str());

    return nullptr;
}


bool Lexer::TestState(State* state)
{
    auto start_position = position_;

    // Disable emiting anything.
    dry_run_ = true;

    // True if the state under test transitions to another valid state.
    bool state_transition = ((*state)(this) != nullptr);

    // Reenable emiting tokens.
    dry_run_ = false;
   
    // Back up to before the dry run. 
    BackupTo(start_position);

    return state_transition;
}

}

}

