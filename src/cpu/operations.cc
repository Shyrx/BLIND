#include "operations.hh"

#include "../common/kernels.hh"

namespace cpu
{
    namespace
    {
        unsigned char apply(const cv::Mat &mat, int x, int y,
                            const double *kernel, int kernel_size)
        {
            double val = 0;
            for (auto i = 0; i < kernel_size; i++)
                for (auto j = 0; j < kernel_size; j++)
                {
                    auto act_x = x + j - kernel_size / 2;
                    if (act_x < 0)
                        act_x = -act_x;
                    else if (act_x >= mat.cols)
                        act_x = 2 * mat.cols - act_x - 1;

                    auto act_y = y + i - kernel_size / 2;
                    if (act_y < 0)
                        act_y = -act_y;
                    else if (act_y >= mat.rows)
                        act_y = 2 * mat.rows - act_y - 1;

                    val += kernel[i * kernel_size + j]
                        * mat.at<unsigned char>(act_y, act_x);
                }

            return val;
        }
    } // namespace

    cv::Mat blur(const cv::Mat &mat)
    {
        cv::Mat out(mat.rows, mat.cols, mat.type());
        for (auto y = 0; y < mat.rows; y++)
            for (auto x = 0; x < mat.cols; x++)
            {
                out.at<unsigned char>(y, x) =
                    apply(mat, x, y,
                          reinterpret_cast<const double *>(
                              common::gaussian_kernel_11x11),
                          11);
            }

        return out;
    }

    cv::Mat matrices_diff_abs(const cv::Mat &mat_bg, const cv::Mat &mat)
    {
        cv::Mat out(mat.rows, mat.cols, mat.type());
        for (auto y = 0; y < mat.rows; y++)
            for (auto x = 0; x < mat.cols; x++)
            {
                int res = mat.at<unsigned char>(y, x)
                    - mat_bg.at<unsigned char>(y, x);
                out.at<unsigned char>(y, x) = res < 0 ? -res : res;
            }
        return out;
    }

    cv::Mat grayscale(const cv::Mat &mat)
    {
        cv::Mat out(mat.rows, mat.cols, CV_8UC1);
        for (auto y = 0; y < mat.rows; y++)
            for (auto x = 0; x < mat.cols; x++)
            {
                const cv::Point p(x, y);
                const auto v = mat.at<cv::Vec3b>(p);
                out.at<unsigned char>(p) =
                    (0.299 * v[0] + 0.587 * v[1] + 0.114 * v[2]);
            }

        return out;
    }
} // namespace cpu
