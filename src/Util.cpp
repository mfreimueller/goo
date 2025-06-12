//
// Created by michael on 12.06.25.
//

#include "Util.h"

#include <algorithm>
#include <regex>

namespace goo {

    bool compareStringsCaseInsensitive(std::string s1, std::string s2) {
        std::ranges::transform(s1, s1.begin(), ::tolower);
        std::ranges::transform(s2, s2.begin(), ::tolower);
        return s1 == s2;
    }

    std::string stripWhitespace(const std::string& s) {
        return std::regex_replace(s, std::regex("(^[ ]+)|([ ]+$)"),"");
    }

    std::vector<std::string> splitIntoLines(const std::string &s) {
        std::vector<std::string> lines;
        std::stringstream ss(s);
        std::string line;

        while (std::getline(ss, line)) {
            lines.push_back(line);
        }

        return lines;
    }

    std::string repeatString(const std::string& s, const int count) {
        std::string repeated = s;
        for (int idx = 1; idx < count; idx++) {
            repeated += s;
        }

        return repeated;
    }

} // goo