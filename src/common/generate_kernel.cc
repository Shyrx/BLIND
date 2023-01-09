#include "generate_kernel.hh"

namespace common
{
    namespace
    {
        int distance_to_the_square(int x, int y, int x2, int y2)
        {
            return ((x - x2) * (x - x2) + (y - y2) * (y - y2));
        }
    } // namespace

    cv::Mat generate_circle_kernel(int size)
    {
        if (size % 2 == 0)
            ++size;
        cv::Mat out(size, size, CV_8UC1);
        auto ref_dist = (size / 2) * (size / 2);
        for (auto y = 0; y < size; y++)
        {
            for (auto x = 0; x < size; x++)
            {
                out.at<unsigned char>(y, x) =
                    ref_dist >= distance_to_the_square(x, y, size / 2, size / 2)
                    ? 1
                    : 0;
            }
        }
        return out;
    }
} // namespace common
