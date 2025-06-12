//
// Created by michael on 12.06.25.
//

#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <vector>

namespace goo {
    /// Compares two strings and returns true if both strings
    /// are equal, ignoring the particular case of each string.
    bool compareStringsCaseInsensitive(std::string s1, std::string s2);

    /// Strips the whitespace of an input string `s` on both sides.
    std::string stripWhitespace(const std::string &s);

    /// Splits an input string by line breaks and adds each line to a
    /// vector that is returned afterward. Empty lines are also added to
    /// the vector.
    std::vector<std::string> splitIntoLines(const std::string &s);

    /// Repeat a given string count-times.
    std::string repeatString(const std::string &s, int count);
} // goo

#endif //UTIL_H
