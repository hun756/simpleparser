#include "tokenizer.hpp"
#include <stdexcept>

#include <iostream>

namespace SimplePaser
{
    const char *tokenTypeToStrings[]
    {
        "WHITESPACE",
        "IDENTIFIER",
        "INTEGER_LITERAL",
        "DOUBLE_LITERAL",
        "STRING_LITERAL",
        "OPERATOR",
        "STRING_ESCAPE_SEQUENCE",
        "POTENTIAL_DOUBLE",
        "POTENTIAL_COMMENT",
        "COMMENT"
    };

    Token::Token() : _type{TokenType::WHITESPACE}, _text{""}, _startOffset{0}, _endOffset{0}, _lineNumber{0}
    {
    }

    std::vector<Token> Tokenizer::parse(const std::string &inProgram)
    {
        std::vector<Token> tokens;
        Token currentToken;

        currentToken._lineNumber = 1;

        for (char currCh : inProgram)
        {
            if (currentToken._type == TokenType::STRING_ESCAPE_SEQUENCE)
            {
                switch (currCh)
                {
                    case 'n': {
                        currentToken._text.append(1, '\n');
                    }
                    break;

                    case 'r': {
                        currentToken._text.append(1, '\r');
                    }
                    break;

                    case 't': {
                        currentToken._text.append(1, '\t');
                    }
                    break;

                    case '\\': {
                        currentToken._text.append(1, '\\');
                    }
                    break;

                    default:
                        throw std::runtime_error(
                            std::string("unknown escape sequence: \\") 
                            + std ::string(1, currCh) 
                            + " in string on line " 
                            + std::to_string(currentToken._lineNumber) 
                            + "."
                        );
                    break;
                }
                currentToken._type = TokenType::STRING_LITERAL;
                continue;
            }
            else if (currentToken._type == TokenType::POTENTIAL_COMMENT && currCh != '/')
            {
                currentToken._type = TokenType::OPERATOR;
                endToken(currentToken, tokens);
                continue;
            }

            switch (currCh)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
                    if (currentToken._type == TokenType::WHITESPACE)
                    {
                        currentToken._type = TokenType::INTEGER_LITERAL;
                        currentToken._text.append(1, currCh);
                    }
                    else if (currentToken._type == TokenType::POTENTIAL_DOUBLE)
                    {
                        currentToken._type = TokenType::DOUBLE_LITERAL;
                        currentToken._text.append(1, currCh);
                    }
                    else
                        currentToken._text.append(1, currCh);
                }
                break;

                case '.': {
                    if (currentToken._type == TokenType::WHITESPACE)
                    {
                        currentToken._type = TokenType::POTENTIAL_DOUBLE;
                        currentToken._text.append(1, currCh);
                    }
                    else if (currentToken._type == TokenType::INTEGER_LITERAL)
                    {
                        currentToken._type = TokenType::DOUBLE_LITERAL;
                        currentToken._text.append(1, currCh);
                    }
                    else if (currentToken._type == TokenType::STRING_LITERAL)
                        currentToken._text.append(1, currCh);
                    else
                    {
                        endToken(currentToken, tokens);
                        currentToken._type = TokenType::OPERATOR;
                        currentToken._text.append(1, currCh);
                        endToken(currentToken, tokens);
                    }
                }
                break;

                case '{':
                case '}':
                case '(':
                case ')':
                case '=':
                case '+':
                case '-':
                case '*':
                case '<':
                case ';':
                case ',': {
                    if (currentToken._type != TokenType::STRING_LITERAL)
                    {
                        endToken(currentToken, tokens);
                        currentToken._type = TokenType::OPERATOR;
                        currentToken._text.append(1, currCh);
                        endToken(currentToken, tokens);
                    }
                    else
                        currentToken._text.append(1, currCh);
                }
                break;

                case ' ':
                case '\t': {
                    if (currentToken._type == TokenType::STRING_LITERAL || currentToken._type == TokenType::COMMENT)
                        currentToken._text.append(1, currCh);
                    else
                        endToken(currentToken, tokens);
                }
                break;

                case '\r':
                case '\n': {
                    endToken(currentToken, tokens);
                    ++currentToken._lineNumber;
                }
                break;

                case '"': {
                    if (currentToken._type != TokenType::STRING_LITERAL)
                    {
                        endToken(currentToken, tokens);
                        currentToken._type = TokenType::STRING_LITERAL;
                    }
                    else if (currentToken._type == TokenType::STRING_LITERAL)
                        endToken(currentToken, tokens);
                }
                break;

                case '\\': {
                    if (currentToken._type == TokenType::STRING_LITERAL)
                        currentToken._type = TokenType::STRING_ESCAPE_SEQUENCE;
                    else
                    {
                        endToken(currentToken, tokens);
                        currentToken._type = TokenType::OPERATOR;
                        currentToken._text.append(1, currCh);
                        endToken(currentToken, tokens);
                    }
                }
                break;

                case '/': {
                    if (currentToken._type == TokenType::STRING_LITERAL)
                        currentToken._text.append(1, currCh);
                    else if (currentToken._type == TokenType::POTENTIAL_COMMENT)
                    {
                        currentToken._type = TokenType::COMMENT;
                        currentToken._text.erase();
                    }
                    else
                    {
                        endToken(currentToken, tokens);
                        currentToken._type = TokenType::POTENTIAL_COMMENT;
                        currentToken._text.append(1, currCh);
                    }
                }
                break;

                default: {
                    if (
                        currentToken._type == TokenType::WHITESPACE 
                        || currentToken._type == TokenType::INTEGER_LITERAL 
                        || currentToken._type == TokenType::DOUBLE_LITERAL
                    )
                    {
                        endToken(currentToken, tokens);
                        currentToken._type = TokenType::IDENTIFIER;
                        currentToken._text.append(1, currCh);
                    }
                    else
                        currentToken._text.append(1, currCh);
                }
                break;
            }
        }

        endToken(currentToken, tokens);

        return tokens;
    }

    void Tokenizer::endToken(Token &token, std::vector<Token> &tokens)
    {
        if (token._type == TokenType::COMMENT)
        {
            std::cout << "Ignoring comment " << token._text << std::endl;
        }
        else if (token._type != TokenType::WHITESPACE)
        {
            tokens.push_back(token);
        }
        if (token._type == TokenType::POTENTIAL_DOUBLE)
        {
            if (token._text.compare(".") == 0)
                token._type = TokenType::OPERATOR;
            else
                token._type = TokenType::DOUBLE_LITERAL;
        }
        token._type = TokenType::WHITESPACE;
        token._text.erase();
    }

    void Token::debugPrint() const
    {
        std::cout << "Token(" << tokenTypeToStrings[(int)_type] << ", \"" << _text << "\", " << _lineNumber << ")" << std::endl;
    }
}
