//
// Created by michael on 13.06.25.
//

#include "Assembler.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "Reporter.h"

namespace fs = std::filesystem;

namespace goo {

    std::shared_ptr<Payload> Assembler::run(std::shared_ptr<Payload> payload) {
        const auto stringPayload = std::static_pointer_cast<StringPayload>(payload);

        std::string tmpPath;
        if (!writeToTmpFile(stringPayload->value, tmpPath)) {
            return nullptr;
        }

        std::string cmd;
        bool debugBuild = true; // TODO: pass options!
        std::string output = "out.o";

        if (debugBuild) {
            cmd = "nasm -f elf64 -g -F dwarf " + tmpPath + " -o " + output;
        } else {
            cmd = "nasm -f elf64 " + tmpPath + " -o " + output;
        }

        std::cout << cmd << std::endl;

        if (const int result = system(cmd.c_str()); result != 0) {
            reporter.error(std::format("Error: Failed to execute command: {}", cmd));
            return nullptr;
        }

        std::cout << "Created object file " << output << std::endl;
        fs::remove(tmpPath);

        return nullptr;
    }

    bool Assembler::writeToTmpFile(const std::string& content, std::string &fileName) const {
        const fs::path tmpDir = fs::temp_directory_path();

        // We need to add the XXXXXX to the filepath, because mkstemps replaces this with random characters.
        const fs::path tmpFile = tmpDir / fs::path("bf_tmpXXXXXX.asm");

        std::string tmpPath = tmpFile.string();
        const int fd = mkstemps(tmpPath.data(), 4);
        if (fd == -1) {
            reporter.error(std::format("Failed to create temporary file: {}", tmpPath));
            return false;
        }

        close(fd);

        fileName = tmpFile.string();

        std::ofstream out(tmpPath);
        out << content;
        out.close();

        return true;
    }



} // goo