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

    std::shared_ptr<Payload> Assembler::run(const std::shared_ptr<Payload> payload) {
        const auto stringPayload = std::static_pointer_cast<StringPayload>(payload);

        std::string tmpPath;
        if (!writeToTmpFile(stringPayload->value, tmpPath)) {
            return nullptr;
        }

        if (config.verbose) {
            std::cout << "Created tmp file at: " << tmpPath << std::endl;
        }

        std::string cmd;

        if (config.debugBuild) {
            cmd = std::format("nasm -f elf64 -g -F dwarf {} -o {}", tmpPath, config.outputFile);
        } else {
            cmd = std::format("nasm -f elf64 {} -o {}", tmpPath, config.outputFile);
        }

        if (config.verbose) {
            std::cout << "Executing: " << cmd << std::endl;
        }

        if (const int result = system(cmd.c_str()); result != 0) {
            reporter.error(std::format("Error: Failed to execute command: {}", cmd));
            return nullptr;
        }

        fs::remove(tmpPath);

        std::cout << "[Finished]" << std::endl;

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