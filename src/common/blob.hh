#pragma once

#include <opencv2/core.hpp>

namespace common
{
    struct Blob
    {
        cv::Point top_left;
        cv::Point bottom_right;
        unsigned char peak;
    };
} // namespace common
