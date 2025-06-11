//
// Created by michael on 05.06.25.
//

#include "Token.h"

namespace brainlove {

    std::string Token::toString() const {
        std::string lexeme;
        switch (type) {
            case INC_BYTE:
                lexeme = "+";
                break;
            case DEC_BYTE:
                lexeme = "-";
                break;
            case INC_PTR:
                lexeme = ">";
                break;
            case DEC_PTR:
                lexeme = "<";
                break;
            case OUT:
                lexeme = ".";
                break;
            case IN:
                lexeme = ",";
                break;
            case IF:
                lexeme = "[";
                break;
            case FI:
                lexeme = "]";
                break;
            case DEBUG:
                lexeme = "!";
                break;
            case EOF_:
            default:
                lexeme = " ";
                break;
        }

        return lexeme + " " + std::to_string(column) + ":" + std::to_string(line);
    }

} // brainlove