#ifndef SIMPLE_PARSER_TOKENIZER_HPP_jf24id
#define SIMPLE_PARSER_TOKENIZER_HPP_jf24id

#include <vector>
#include <string>

namespace SimplePaser
{
    enum class TokenType
    {
        WHITESPACE,
        IDENTIFIER,
        INTEGER_LITERAL,
        DOUBLE_LITERAL,
        STRING_LITERAL,
        OPERATOR,
        STRING_ESCAPE_SEQUENCE,
        POTENTIAL_DOUBLE,
        POTENTIAL_COMMENT,
        COMMENT
    };

    class Token
    {
    public:
        Token();

        void debugPrint() const;

    public:
        std::string _text;
        TokenType _type;
        size_t _startOffset;
        size_t _endOffset;
        size_t _lineNumber;
    };

    class Tokenizer
    {
    public:
        std::vector<Token> parse(const std::string &inProgram);

    private:
        void endToken(Token &tok, std::vector<Token> &tokens);
    };
}

#endif /* end of include guard : SIMPLE_PARSER_TOKENIZER_HPP_jf24id */