#include "process.hh"

#include <cmath>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

namespace blind
{
    namespace
    {
        constexpr const auto closing_kern_size = 30;
        constexpr const auto opening_kern_size = 16;

        void closing(cv::Mat &mat)
        {
            const auto kernel = cv::getStructuringElement(
                cv::MORPH_RECT, cv::Size(closing_kern_size, closing_kern_size));

            cv::dilate(mat, mat, kernel);
            cv::erode(mat, mat, kernel);
        }

        void opening(cv::Mat &mat)
        {
            const auto kernel = cv::getStructuringElement(
                cv::MORPH_RECT, cv::Size(opening_kern_size, opening_kern_size));

            cv::erode(mat, mat, kernel);
            cv::dilate(mat, mat, kernel);
        }

        cv::Mat draw_boxes(const cv::Mat &img, const cv::Mat &components,
                           int nb_components)
        {
            cv::Mat out;
            cv::cvtColor(img, out, cv::COLOR_GRAY2RGB);
            const auto red = cv::Scalar(0, 0, 255);
            for (auto i = 1; i < nb_components; i++)
            {
                const auto x = components.at<int>(i, cv::CC_STAT_LEFT);
                const auto y = components.at<int>(i, cv::CC_STAT_TOP);
                const auto width = components.at<int>(i, cv::CC_STAT_WIDTH);
                const auto height = components.at<int>(i, cv::CC_STAT_HEIGHT);

                cv::rectangle(out, cv::Point(x, y),
                              cv::Point(x + width, y + height), red,
                              cv::LINE_8);
            }

            return out;
        }

        cv::Mat filterOnColor(cv::Mat &img, cv::Scalar &low, cv::Scalar &high)
        {
            cv::Mat res;
            cv::inRange(img, low, high, res);
            return res;
        }

        cv::Mat fuseMat(cv::Mat &first, cv::Mat &second)
        {
            return cv::max(first, second);
        }

        void remove_noise(cv::Mat &img)
        {
            opening(img);
            closing(img);
        }

        unsigned int dist(const cv::Point &a, const cv::Point &b)
        {
            return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
        }

        cv::Point getMiddle(const cv::Point &red, const cv::Point &yellow, int max_cols,
                                            int max_rows)
        {
            cv::Point mid((red.x + yellow.x) / 2, (red.y + yellow.y) / 2);
            if (mid.x == 0 && mid.y == 0)
                mid = cv::Point(max_cols / 2, max_rows / 2);
            else if (red.x == 0 && red.y == 0)
                mid = cv::Point(0, yellow.y);
            else if (yellow.x == 0 && yellow.y == 0)
                mid = cv::Point(max_cols, red.y);

            return mid;
        }
        cv::Point getDirection(const int nb_labels, cv::Mat &centroids,
                               cv::Mat &yellow_cones, cv::Mat &stats)
        {
            cv::Point base(yellow_cones.cols / 2, yellow_cones.rows);
            cv::Point red(0, 0);
            int max_red_size = 0;
            cv::Point yellow(0, 0);
            int max_yellow_size = 0;
            for (int i = 1; i < nb_labels; i++)
            {
                cv::Point curr_p(centroids.at<double>(i, 0),
                                 centroids.at<double>(i, 1));
                auto val = yellow_cones.at<unsigned char>(curr_p);
                auto size = stats.at<int>(i, cv::CC_STAT_AREA);

                auto check_and_swap_cone =
                    [&base, &curr_p, &size](int &max_size, cv::Point &p) {
                        if (max_size < (size / 10 * 7)) // Prev too small
                        {
                            max_size = size;
                            p = curr_p;
                            return;
                        }
                        else if (size > max_size)
                            max_size = size;
                        else if (size < (max_size / 10 * 7)) // Curr too small
                            return;

                        if (dist(base, p) > dist(base, curr_p))
                            p = curr_p;
                    };

                if (val != 0)
                    check_and_swap_cone(max_yellow_size, yellow);
                else
                    check_and_swap_cone(max_red_size, red);
            }

            return getMiddle(red, yellow, yellow_cones.cols, yellow_cones.rows);
        }
    } // namespace

    cv::Scalar white_low = cv::Scalar(160, 100, 100);
    cv::Scalar white_high = cv::Scalar(180, 255, 255);

    cv::Scalar yellow_low = cv::Scalar(30, 100, 100);
    cv::Scalar yellow_high = cv::Scalar(50, 255, 255);

    cv::Mat draw_traject(cv::Mat &original)
    {
        cv::Mat img;
        // Use HSV to filter red and yellow cone
        cvtColor(original, img, cv::COLOR_BGR2HSV);
        cv::Mat yellow_cones = filterOnColor(img, yellow_low, yellow_high);
        remove_noise(yellow_cones);
        cv::Mat red_cones = filterOnColor(img, white_low, white_high);
        remove_noise(red_cones);

        cv::Mat all_cones = fuseMat(red_cones, yellow_cones);

        cv::Mat labels, stats, centroids;
        const auto nb_labels = cv::connectedComponentsWithStats(
            all_cones, labels, stats, centroids);

        const cv::Point base(img.cols / 2, img.rows);
        const auto mid = getDirection(nb_labels, centroids, yellow_cones, stats);

        cvtColor(all_cones, all_cones, cv::COLOR_GRAY2BGR);
        cv::arrowedLine(all_cones, base, mid, cv::Scalar(255, 255, 255));

        return all_cones;

        // 3 - Match cones together
        // 4 - Draw traject
    }
} // namespace blind
