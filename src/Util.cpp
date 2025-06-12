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


} // goo