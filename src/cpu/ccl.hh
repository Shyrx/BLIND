#pragma once

#include <opencv2/core.hpp>

#include "../common/blob.hh"

namespace cpu
{
    std::vector<common::Blob> ccl(const cv::Mat &img);
} // namespace cpu
