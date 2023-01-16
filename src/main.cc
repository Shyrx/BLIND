#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <stdexcept>
#include <unistd.h>

#include "arg-parser.hh"
#include "process.hh"
#include "serial.hh"

void process_image(const std::filesystem::path &input,
                   const std::filesystem::path &output)
{
    auto img = cv::imread(input.string());

    img = blind::draw_traject(img);

    cv::imwrite(output.string(), img);
}

void process_video(const std::filesystem::path &input,
                   const std::filesystem::path &output)
{
    cv::VideoCapture video(input);

    if (!video.isOpened())
        throw std::runtime_error("could not open input video");

    cv::Mat img_bg_color;
    cv::Mat img_color;

    video >> img_bg_color;
    cv::Size S((int)video.get(cv::CAP_PROP_FRAME_WIDTH), // Acquire input size
               (int)video.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::VideoWriter outputVideo; // Open the output
    outputVideo.open(output, static_cast<int>(video.get(cv::CAP_PROP_FOURCC)),
                     video.get(cv::CAP_PROP_FPS),
                     cv::Size(img_bg_color.cols, img_bg_color.rows), true);
    if (!outputVideo.isOpened())
        throw std::runtime_error("could not open the output video for write");

    video >> img_color;
    int i = 1;
    while (!img_color.empty())
    {
        std::cout << "processing frame: " << i << std::endl;
        img_color = blind::draw_traject(img_color);

        outputVideo << img_color;

        video >> img_color;
        ++i;
    }
}

void capture_camera()
{
    cv::Mat frame;
    cv::VideoCapture cap;
    serial::SerialCommunicator s;

    cap.open(0, cv::CAP_V4L2);
    if (!cap.isOpened())
        throw std::runtime_error("unable to open camera");

    while (true)
    {
        cap.read(frame);
        if (frame.empty())
            throw std::runtime_error("blank frame, aborting");

        int angle = blind::get_angle(frame);
        for (const char c : std::to_string(angle))
            s.send(c);
        s.send('\n');

        usleep(5000);
    }
}

int communicate_serial()
{
    serial::SerialCommunicator s;
    int ret;

    while (true)
    {
        ret = s.send('L');
        if (ret != 0)
            return ret;

        sleep(2);

        ret = s.send('R');
        if (ret != 0)
            return ret;

        sleep(2);
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    blind::ArgParser parser(argc, argv);
    parser.run();

    if (parser.error())
    {
        parser.print_error();
        return 1;
    }
    if (parser.has("help"))
    {
        parser.print_help();
        return 0;
    }

    const auto mode = parser.get_mode();
    switch (mode)
    {
    case blind::Mode::IMAGE:
        process_image(parser.get_input(), parser.get_output());
        break;
    case blind::Mode::VIDEO:
        process_video(parser.get_input(), parser.get_output());
        break;
    case blind::Mode::CAMERA:
        capture_camera();
    }

    return 0;
}
