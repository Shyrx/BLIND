#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <unistd.h>

#include "process.hh"
#include "serial.hh"

void process_image(const std::filesystem::path &input,
                   const std::filesystem::path &output)
{
    auto img = cv::imread(input.string());

    img = blind::draw_traject(img);

    cv::imwrite(output.string(), img);
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

    cap.open(0, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "Unable to open camera\n";
        return;
    }

    while (true)
    {
        cap.read(frame);
        if (frame.empty())
        {
            std::cerr << "Blank frame, aborting\n";
            return;
        }

        // faire du processing sur `frame` ici

        if (cv::waitKey(5) >= 0)
            break;
    }
}

int communicate_serial()
{
    serial::SerialCommunicator s;
    int ret;

    // alternate between sending 'L' and 'R' every 2 secs to serial
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

int main(int argc, char *argv[])
{
    const std::string process_mode(argv[1]);

    if (argc != 4 && process_mode != "camera")
    {
        std::cerr << "Usage:\n"
                  << "\t" << argv[0] << " image input output\n"
                  << "\t" << argv[0] << " video input output\n"
                  << "\t" << argv[0] << " camera\n";
        return 1;
    }

    if (process_mode == "image")
    {
        process_image(argv[2], argv[3]);
    }
    else if (process_mode == "video")
    {
        process_video(argv[2], argv[3]);
    }
    else if (process_mode == "camera")
    {
        capture_camera();
    }

    return 0;
}
