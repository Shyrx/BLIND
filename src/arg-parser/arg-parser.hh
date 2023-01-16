#pragma once

#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace blind
{
    enum class Mode
    {
        IMAGE,
        VIDEO,
        CAMERA,
    };

    namespace po = boost::program_options;

    class ArgParser
    {
    public:
        ArgParser(int argc, const char *argv[]);
        void run();

        void print_help() const
        {
            std::cout << "usage: " << *argv_
                      << " <image|video|camera> [input] [output]" << '\n'
                      << desc_ << '\n';
        }

        void print_error() const
        {
            if (!error_)
                return;
            std::cerr << "error: " << *error_ << '\n';
            print_help();
        }

        bool error() const
        {
            return error_.has_value();
        }

        Mode get_mode() const
        {
            return mode_;
        }

        std::filesystem::path get_input() const
        {
            return vm_["input"].as<std::string>();
        }

        std::filesystem::path get_output() const
        {
            return vm_["output"].as<std::string>();
        }

        bool has(const std::string &name) const
        {
            return vm_.contains(name);
        }

    private:
        const int argc_;
        const char **argv_;
        po::options_description desc_;
        po::positional_options_description pos_;
        po::variables_map vm_;
        std::optional<std::string> error_;
        Mode mode_;
    };
} // namespace blind
