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


Lexer::Lexer(std::string filepath, State* initial_state)
{
    std::ifstream file(filepath);
    input_ = std::string( (std::istreambuf_iterator<char>(file))
                        , std::istreambuf_iterator<char>());

    if (initial_state)
    {
        Run(initial_state);
    }
}


std::string Lexer::String()
{
    return input_.substr(start_, position_ - start_);
}


void Lexer::Run(State* initial_state, size_t lines)
{
    parent_item_ = new Item { ParentToken, position_, line_, "", "", nullptr, std::vector<Item>() };

    state_ = initial_state;

    end_line_ = 0;

    if (lines > 0)
        end_line_ = line_ + lines;

    while (state_)
    {
        state_ = (*state_)(this);
    }
}


Items Lexer::GetItems()
{
    return parent_item_->children;
}


void Lexer::Backup()
{
    position_ -= width_;

    auto next = input_.substr(position_, 1);

    if (next == "\n")
    {
        line_--;
    }
}


void Lexer::BackupTo(int position)
{
    auto s = input_.substr(position, position_ - position);
    line_ -= std::count(s.begin(), s.end(), '\n');

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

    if (next == "\n")
        line_++;
 
    return next;
}


std::string Lexer::Peek()
{
    auto next = Next();

    if (next != "")
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

    auto item = Item { token, position_, line_, String(), error, parent_item_, Items() };
    Skip();
    
    if (parent_item_)
    { 
        PrintItem(item);

        parent_item_->children.push_back(item);
        return &(parent_item_->children.back());
    }

    else
    {
        depth_++;
        PrintItem(item);

        items_.push_back(item);
        return &(items_.back());
    }
}


Item* Lexer::StartOfA(Token token, std::string error)
{
    if (dry_run_) return nullptr;

    parent_item_ = ThisIsA(token, error);

    depth_ ++;

    return parent_item_;
}


Item* Lexer::EndOfA(Token token, std::string /*error*/)
{
    if (dry_run_) return nullptr;
    
    depth_--;
    
    PrintItem(*parent_item_);

    if (parent_item_->token.name != token.name)
    {
        Exceptions::LexerError("Lexer::EndOfA", "Trying to end a '"
                                              + parent_item_->token.name
                                              + "' with a '"
                                              + token.name
                                              + "' token.");
    }

    if (parent_item_)
    {
        parent_item_ = parent_item_->parent;
    }

    return nullptr;
}


Item* Lexer::MaybeEndOfA(Token token, std::string error)
{
    if (parent_item_->token.name == token.name)
    {
        return EndOfA(token, error);
    }
    
    else
    {
        return nullptr;
    }
}


bool Lexer::OneOf(std::string possibles)
{
    auto peek = Peek();

    size_t position = possibles.find(Peek());

    if (position != std::string::npos)
    {
        auto next = Next();
        return next != "";
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


bool Lexer::Until(std::string match)
{
    while (!OneOf(match))
    {
        if (Next() == "")
            return false;
    }

    return true;
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


bool Lexer::ManyCharacters()
{
    return ManyOf("!\"#$%&\\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[|]^_`abcdefghijklmnopqrstuvwxyz{}~");
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
    if (!ManyOf(" \t\r"))
    {
        Skip();
        return false;
    }

    Skip();
    return true;
}


bool Lexer::SkipLineBreak()
{
    if (!OneOf("\n"))
    {
        return false;
    }

    Skip();
    return true;
}


bool Lexer::SkipLineBreaks()
{
    if (!ManyOf("\n"))
    {
        return false;
    }

    Skip();
    return true;
}


bool Lexer::SkipLine()
{
    if (!Until("\n"))
    {
        return false;
    }

    Skip();
    return true;
}


State* Lexer::Error(std::string message)
{

    std::stringstream error;
    error << "Error around line " << line_  << ": " << message << std::endl;
   
    last_error_ = error.str();

    if (dry_run_) return nullptr;
    
    Item item { ErrorToken, position_, line_, "", error.str(), parent_item_, Items()};
    items_.push_back(item);

    Exceptions::LexerError("Lexer", error.str());

    return nullptr;
}


State* Lexer::LastError()
{
    if (last_error_ == "")
    {
        Exceptions::LexerError("Lexer", "Something went wrong.");
    }

    else
    {
        Exceptions::LexerError("Lexer", last_error_);
    }
        
    return nullptr;
}


bool Lexer::TestState(State* state)
{
    // Short circuit if we are already dry running.
    if (dry_run_) return false;
  
    // Check if an end line has been reached. 
    if (end_line_ > 0 && line_ > end_line_) return false;

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


bool Lexer::IsDryRun()
{
    return dry_run_;
}


size_t Lexer::LineNumber()
{
    return line_;
}


# ifdef CADMESH_LEXER_VERBOSE
void Lexer::PrintMessage(std::string name, std::string message)
{
    std::cout << "Lexer::" << name << " : " << message << std::endl;
}
# else
void Lexer::PrintMessage(std::string, std::string)
{
}
#endif

# ifdef CADMESH_LEXER_VERBOSE
void Lexer::PrintItem(Item item)
{
    auto depth = std::max(0, depth_) * 2;
    std::cout << std::string(depth, ' ') << item.token.name << ": " << item.value << std::endl;
}
# else
void Lexer::PrintItem(Item)
{
}
#endif

}

}

