#include "output.hh"

#include <opencv2/imgproc.hpp>

namespace common {

void draw_contours(cv::Mat &mat, const std::vector<common::Blob> &points) {
  const cv::Scalar red(0, 0, 255); // scalar != rgb
  for (const auto &bob : points)
    cv::rectangle(mat, bob.top_left, bob.bottom_right, red, cv::LINE_8);
}
} // namespace common
