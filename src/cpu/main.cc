#include "main.hh"

#include <filesystem>

#include "../common/blob.hh"
#include "../common/generate_kernel.hh"
#include "../common/output.hh"
#include "ccl.hh"
#include "morphological.hh"
#include "operations.hh"

namespace cpu
{
    cv::Mat process_image(cv::Mat &img)
    {
        cv::Mat red_cone_filtered;
        cv::Mat yellow_cone_filtered;
        cvtColor(img, img, cv::COLOR_BGR2HSV);
        // get all white cone (white == yellow == turn right)
        cv::inRange(img, cv::Scalar(45, 165, 130), cv::Scalar(180, 255, 200), yellow_cone_filtered);
        // get all yellow cone (yellow == red == turn left)
        cv::inRange(img, cv::Scalar(30, 100, 100), cv::Scalar(50, 255, 255), red_cone_filtered);
        return yellow_cone_filtered;
    }
} // namespace cpu
