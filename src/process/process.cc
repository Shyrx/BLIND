#include "process.hh"

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
        constexpr const auto closing_kern_size = 5;
        constexpr const auto opening_kern_size = 15;

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

        cv::Point getDirection(const cv::Point &base, const int nb_labels,
                               cv::Mat &centroids, cv::Mat &yellow_cones)
        {
            cv::Point red(0, 0);
            cv::Point yellow(0, 0);
            for (int i = 1; i < nb_labels; i++)
            {
                cv::Point p(centroids.at<double>(i, 0),
                            centroids.at<double>(i, 1));
                auto val = yellow_cones.at<unsigned char>(p);

                if (val != 0)
                {
                    if (yellow.dot(base) < p.dot(base))
                        yellow = p;
                }
                else
                {
                    if (red.dot(base) < p.dot(base))
                        red = p;
                }
            }
            cv::Point mid =
                cv::Point((red.x + yellow.x) / 2, (red.y + yellow.y) / 2);
            if (mid.x == 0 && mid.y == 0)
                mid = cv::Point(yellow_cones.cols / 2, yellow_cones.rows / 2);

            return mid;
        }
    } // namespace

    cv::Scalar white_low = cv::Scalar(170, 150, 150);
    cv::Scalar white_high = cv::Scalar(190, 255, 255);

    cv::Scalar yellow_low = cv::Scalar(30, 100, 150);
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

        cv::Point base(img.cols / 2, img.rows);
        cv::Point mid = getDirection(base, nb_labels, centroids, yellow_cones);

        cvtColor(all_cones, all_cones, cv::COLOR_GRAY2BGR);
        cv::arrowedLine(all_cones, base, mid, cv::Scalar(255, 255, 255));

        return all_cones;

        // 3 - Match cones together
        // 4 - Draw traject
    }
} // namespace blind
