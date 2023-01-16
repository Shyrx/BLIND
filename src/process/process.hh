#pragma once

#include <opencv2/core.hpp>

namespace blind
{
    constexpr const auto no_angle = 360;

    cv::Mat draw_traject(cv::Mat &img);
    int get_angle(cv::Mat &img);
} // namespace blind
