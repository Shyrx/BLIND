#pragma once

#include <opencv2/imgcodecs.hpp>
#include <string>

namespace cpu
{
    cv::Mat process_image(cv::Mat &img);
} // namespace cpu
