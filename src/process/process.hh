#pragma once

#include <opencv2/core.hpp>

namespace blind
{
    cv::Mat draw_traject(cv::Mat &img);
    int get_angle(cv::Mat &img);
} // namespace blind
