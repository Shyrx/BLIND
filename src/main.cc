#include "cpu/main.hh"

#include <filesystem>
#include <iostream>
#include <opencv2/videoio.hpp>
#include <string>

void process_image(std::string input_path, std::string output_path)
{
    std::cout << "start read" << std::endl;
    const std::filesystem::path input(input_path);
    auto img_color = cv::imread(input);

    std::cout << "start process" << std::endl;
    img_color = cpu::process_image(img_color);

    std::cout << "read output" << std::endl;
    const std::filesystem::path output(output_path);
    std::cout << "write output" << std::endl;
    cv::imwrite(output, img_color);
    std::cout << "done" << std::endl;
}

void process_video(std::string video_path, std::string output_path)
{
    const std::filesystem::path input_path(video_path);
    cv::VideoCapture video(input_path);

    if (!video.isOpened())
    {
        std::cerr << "Could not open input video: " << video_path << std::endl;
        exit(3);
    }
    cv::Mat img_bg_color;
    cv::Mat img_color;

    video >> img_bg_color;
    cv::Size S((int)video.get(cv::CAP_PROP_FRAME_WIDTH), // Acquire input size
               (int)video.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::VideoWriter outputVideo; // Open the output
    outputVideo.open(output_path,
                     static_cast<int>(video.get(cv::CAP_PROP_FOURCC)),
                     video.get(cv::CAP_PROP_FPS),
                     cv::Size(img_bg_color.cols, img_bg_color.rows), true);
    if (!outputVideo.isOpened())
    {
        std::cerr << "Could not open the output video for write: "
                  << output_path << std::endl;
        exit(3);
    }

    video >> img_color;
    int i = 1;
    while (!img_color.empty())
    {
        std::cout << "processing frame: " << i << std::endl;
        img_color = cpu::process_image(img_color);

        outputVideo << img_color;

        video >> img_color;
        ++i;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage:\n"
                  << "\t" << argv[0] << "image input output\n"
                  << "\t" << argv[0] << "video input output\n";
        return 1;
    }

    const std::string process_mode(argv[1]);

    if (process_mode == "image")
    {
        std::cout << "here" << std::endl;
        process_image(argv[2], argv[3]);
    }
    else
    {
        process_video(argv[2], argv[3]);
    }

    return 0;
}
