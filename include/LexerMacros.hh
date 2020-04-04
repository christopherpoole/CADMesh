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

#define CADMeshLexerToken(name) \
    static Token name##Token{ #name }


#define CADMeshLexerStateDefinition(name) \
    struct name##State : public State { State* operator()(Lexer* lexer) const ; } 

#define CADMeshLexerState(name) name##State::operator()(Lexer* lexer) const

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

#define RestOfLine() if (Until("\n\r")) lexer->Backup()

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

#define Error(message) { lexer->Error(message); return nullptr; }

#define NextState(next) return new next##State()
#define TestState(next) lexer->TestState(new next##State())
#define TryState(next) if (TestState(next)) NextState(next)
#define FinalState() return new __FinalState();

#define RunLexer(filepath, start) Lexer(filepath, new start##State).GetItems()

