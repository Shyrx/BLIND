#include "arg-parser.hh"

namespace blind
{
    ArgParser::ArgParser(int argc, const char **argv)
        : argc_(argc)
        , argv_(argv)
        , desc_("options")
    {
        // clang-format off
        desc_.add_options()
            ("help,h", "print this help and exit")
            ("mode, -k", po::value<std::string>()->value_name("value"),
                         "'image', 'video' or 'camera'")
            ("input,i", po::value<std::string>()->value_name("path"),
                        "path to the input image or video")
            ("output,o", po::value<std::string>()->value_name("path"),
                        "path to the output image or video")
            ("interval,t", po::value<int>()->value_name("time"),
                        "time between each iteration of results")
            ;
        // clang-format on

        pos_.add("mode", 1);
        pos_.add("input", 1);
        pos_.add("output", 1);
    }

    void ArgParser::run()
    {
        try
        {
            po::store(po::command_line_parser(argc_, argv_)
                          .options(desc_)
                          .positional(pos_)
                          .run(),
                      vm_);
            po::notify(vm_);
        }
        catch (const po::error &e)
        {
            error_ = std::string(e.what());
        }

        if (vm_.contains("help"))
            return;

        if (!vm_.contains("mode"))
        {
            error_ = "missing required argument 'mode'";
            return;
        }

        const auto mode = vm_["mode"].as<std::string>();
        if (mode == "camera")
            mode_ = Mode::CAMERA;
        else
        {
            mode_ = mode == "image" ? Mode::IMAGE : Mode::VIDEO;

            if (!vm_.contains("input") or !vm_.contains("output"))
                error_ = "missing required argument 'input' or/and 'output'";
        }
    }
} // namespace blind
