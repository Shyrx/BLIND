#pragma once

#include <opencv2/imgcodecs.hpp>

namespace cpu
{
    cv::Mat generate_circle_kernel(int size);
    cv::Mat closing(const cv::Mat &mat, const cv::Mat &kernel);
    cv::Mat opening(const cv::Mat &mat, const cv::Mat &kernel);
} // namespace cpu
