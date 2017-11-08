
#define CADMeshLexerToken(name) \
    static Token name##Token{ #name "Token" }; \

#define CADMeshLexerStateDefinition(name) \
    struct name##State : public State { State* operator()(Lexer* lexer) const ; } 

#define CADMeshLexerState(name) State* name##State::operator()(Lexer* lexer) const

#define ThisIsA(name) lexer->ThisIsA(name##Token)
#define StartOfA(name) lexer->StartOfA(name##Token)
#define EndOfA(name) lexer->EndOfA(name##Token)

#define Skip() lexer->Skip()

#define OneOf(possibles) lexer->OneOf(possibles)
#define NotOneOf(possibles) !OneOf(possibles)

#define ManyOf(possibles) lexer->ManyOf(possibles)
#define NotManyOf(possibles) !ManyOf(possibles)

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

#define Error(message) { lexer->Error(message); return nullptr; }

#define NextState(next) return new next##State()
#define TestState(next) lexer->TestState(new next##State())
#define TryState(next) if (TestState(next)) NextState(next)
#define FinalState() return nullptr

