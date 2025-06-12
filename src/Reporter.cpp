//
// Created by michael on 12.06.25.
//

#include "Reporter.h"

#include <format>
#include <iostream>
#include <ostream>

#include "Util.h"

namespace goo {

    void Reporter::setCode(const std::string &code) {
        codeLines = splitIntoLines(code);
    }

    void Reporter::print() const {
        if (!errors.empty()) {
            for (const auto& error : errors) {
                std::cout << error << std::endl;
            }
        }

        if (!warnings.empty()) {
            for (const auto& warning : warnings) {
                std::cout << warning << std::endl;
            }
        }
    }

    void Reporter::error(const std::string &message) {
        errors.emplace_back(std::format("{}Error: {}", filename, message));
    }

    void Reporter::error(const int line, const int column, const std::string &message) {
        errors.emplace_back(std::format("{}{}:{}: Error: {}", filename, line, column, message));
        errors.emplace_back(std::format("\t{}\t|\t\t{}", line, codeLines.at(line - 1)));
        errors.emplace_back(std::format("\t\t|\t\t{}^", repeatString(" ", column - 1)));
    }

    void Reporter::warning(const std::string &message) {
        warnings.emplace_back(std::format("{}Warning: {}", filename, message));
    }

    void Reporter::warning(const int line, const int column, const std::string &message) {
        warnings.emplace_back(std::format("{}{}:{}: Warning: {}", filename, line, column, message));
        warnings.emplace_back(std::format("\t{}\t|\t\t{}", line, codeLines.at(line - 1)));
        warnings.emplace_back(std::format("\t\t|\t\t{}^", repeatString(" ", column - 1)));
    }

    void Reporter::reset() {
        errors.clear();
        warnings.clear();
    }

} // goo