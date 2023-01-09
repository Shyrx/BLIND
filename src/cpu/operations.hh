#pragma once

#include <opencv2/core.hpp>

namespace cpu
{
    cv::Mat grayscale(const cv::Mat &mat);
    cv::Mat blur(const cv::Mat &mat);
    cv::Mat matrices_diff_abs(const cv::Mat &mat_bg, const cv::Mat &mat);
} // namespace cpu
