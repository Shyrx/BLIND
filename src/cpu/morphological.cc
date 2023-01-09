#include "morphological.hh"

namespace cpu
{
    namespace
    {
        unsigned char get_errode_value(const cv::Mat &mat, int x, int y,
                                       const cv::Mat &kernel)
        {
            unsigned char res = 255;
            for (auto i = 0; i < kernel.rows; i++)
                for (auto j = 0; j < kernel.rows; j++)
                {
                    if (kernel.at<unsigned char>(i, j))
                    {
                        auto act_x = x + j - kernel.rows / 2;
                        if (act_x < 0 || act_x >= mat.cols)
                            continue;

                        auto act_y = y + i - kernel.rows / 2;
                        if (act_y < 0 || act_y >= mat.rows)
                            continue;

                        if (mat.at<unsigned char>(act_y, act_x) < res)
                            res = mat.at<unsigned char>(act_y, act_x);
                    }
                }
            return res;
        }

        unsigned char get_dilate_value(const cv::Mat &mat, int x, int y,
                                       const cv::Mat &kernel)
        {
            unsigned char res = 0;
            for (auto i = 0; i < kernel.rows; i++)
                for (auto j = 0; j < kernel.rows; j++)
                {
                    if (kernel.at<unsigned char>(i, j))
                    {
                        auto act_x = x + j - kernel.rows / 2;
                        if (act_x < 0 || act_x >= mat.cols)
                            continue;

                        auto act_y = y + i - kernel.rows / 2;
                        if (act_y < 0 || act_y >= mat.rows)
                            continue;

                        if (mat.at<unsigned char>(act_y, act_x) > res)
                            res = mat.at<unsigned char>(act_y, act_x);
                    }
                }
            return res;
        }

        int distance_to_the_square(int x, int y, int x2, int y2)
        {
            return ((x - x2) * (x - x2) + (y - y2) * (y - y2));
        }

    } // namespace
    cv::Mat erosion(const cv::Mat &mat, const cv::Mat &kernel)
    {
        cv::Mat out(mat.rows, mat.cols, mat.type());
        for (auto y = 0; y < mat.rows; y++)
            for (auto x = 0; x < mat.cols; x++)
            {
                out.at<unsigned char>(y, x) =
                    get_errode_value(mat, x, y, kernel);
            }
        return out;
    }

    cv::Mat dilation(const cv::Mat &mat, const cv::Mat &kernel)
    {
        cv::Mat out(mat.rows, mat.cols, mat.type());
        for (auto y = 0; y < mat.rows; y++)
            for (auto x = 0; x < mat.cols; x++)
            {
                out.at<unsigned char>(y, x) =
                    get_dilate_value(mat, x, y, kernel);
            }
        return out;
    }

    cv::Mat generate_circle_kernel(int size)
    {
        if (size % 2 == 0)
            ++size;
        cv::Mat out = cv::Mat::zeros(size, size, CV_8UC1);
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

    cv::Mat closing(const cv::Mat &mat, const cv::Mat &kernel)
    {
        return erosion(dilation(mat, kernel), kernel);
    }

    cv::Mat opening(const cv::Mat &mat, const cv::Mat &kernel)
    {
        return dilation(erosion(mat, kernel), kernel);
    }

} // namespace cpu
