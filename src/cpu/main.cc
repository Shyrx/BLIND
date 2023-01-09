#include "main.hh"

#include <filesystem>

#include "../common/blob.hh"
#include "../common/generate_kernel.hh"
#include "../common/output.hh"
#include "ccl.hh"
#include "morphological.hh"
#include "operations.hh"

namespace cpu
{
    cv::Mat process_image(cv::Mat &img)
    {
        auto img_gray = grayscale(img);
        img_gray = blur(img_gray);

        int closing_kern_size = 31; // TODO compute from input size
        int opening_kern_size = 51;

        auto closing_kern = generate_circle_kernel(closing_kern_size);
        auto opening_kern = generate_circle_kernel(opening_kern_size);

        img_gray = closing(img_gray, closing_kern);
        img_gray = opening(img_gray, opening_kern);

        const auto blobs = ccl(img_gray);

        common::draw_contours(img, blobs);
        return img;
    }
} // namespace cpu
