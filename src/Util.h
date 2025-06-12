//
// Created by michael on 12.06.25.
//

#ifndef UTIL_H
#define UTIL_H
#include <string>

namespace goo {
    /// Compares two strings and returns true if both strings
    /// are equal, ignoring the particular case of each string.
    bool compareStringsCaseInsensitive(std::string s1, std::string s2);

    /// Strips the whitespace of an input string `s` on both sides.
    std::string stripWhitespace(const std::string& s);

} // goo

#endif //UTIL_H
