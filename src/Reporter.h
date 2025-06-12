//
// Created by michael on 12.06.25.
//

#ifndef ERRORTRACKER_H
#define ERRORTRACKER_H

#include <string>
#include <utility>
#include <vector>

namespace goo {
    /// A utility class that tracks any syntactical or semantical errors, to report them to the user afterward.
    /// The class provides two report methods ::error and ::warning. Use ::error to signal that execution is not
    /// recommended (for example, in case of a syntax error). Use ::warning to signal a soft issue with the core
    /// (for example a wrap-around of the tape pointer). Calling either method sets internal flags, which can be
    /// checked via ::hasError and ::hasWarning.
    ///
    /// To report the tracked issues to the user, use ::print. This method prints the errors and warnings to
    /// standard out. Afterward use ::clear to reset the state of the reporter for further use.
    class Reporter {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;

        std::string filename;
        std::vector<std::string> codeLines;

    public:
        /// Creates a new reporter instance, adding an optional filename that is
        /// prepended to every error and warning line.
        /// @param filename An optional filename that is added to every error and warning line.
        explicit Reporter(std::string filename = ""): filename(std::move(filename) + ": ") {}

        /// Sets the code that is translated or interpreted, to report any errors showing the specific location
        /// in the code.
        /// @param code The code to report any errors or warnings about.
        void setCode(const std::string &code);

        /// Prints any errors and warnings to standard out, in this order. Note, that this method doesn't clear the
        /// internal list of errors and warnings.
        void print() const;

        /// Report a new generic error, adding it to the list of errors, to be reported afterward with ::print.
        /// @param message An error message, containing as much information as is needed by the user to fix the problem.
        void error(const std::string& message);

        /// Report a new error at a specific location in the core, adding it to the list of errors, to be
        /// reported afterward with ::print.
        /// @param line The line in code where the error occurred.
        /// @param column The column in the line in the code where the error occurred.
        /// @param message An error message, containing as much information as is needed by the user to fix the problem.
        void error(int line, int column, const std::string& message);

        /// Report a new warning, adding it to the list of warnings, to be reported afterward with ::print.
        /// @param message A warning message, containing as much information as is needed by the user to possibly fix
        /// the problem.
        void warning(const std::string& message);

        /// Report a new warning at a specific location in the core, adding it to the list of warnings, to be
        /// reported afterward with ::print.
        /// @param line The line in code where the warning occurred.
        /// @param column The column in the line in the code where the warning occurred.
        /// @param message An error message, containing as much information as is needed by the user to fix the problem.
        void warning(int line, int column, const std::string& message);

        [[nodiscard]] bool hasError() const { return !errors.empty(); }

        [[nodiscard]] bool hasWarnings() const { return !warnings.empty(); }

        /// Resets the internal state of the reports, clearing any error or warning flags, as well as messages.
        void reset();
    };
} // goo

#endif //ERRORTRACKER_H
