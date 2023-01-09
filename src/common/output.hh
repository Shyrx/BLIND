#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "blob.hh"

namespace common
{
    void draw_contours(cv::Mat &mat, const std::vector<Blob> &points);
} // namespace common
