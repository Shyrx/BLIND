#include "process.hh"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace blind
{
    namespace
    {
        constexpr const auto closing_kern_size = 21;
        constexpr const auto opening_kern_size = 31;

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
    } // namespace

    cv::Mat draw_traject(cv::Mat &img)
    {
        // 1 - Use HSV to filter red and yellow cone
        cv::Mat red_cones, yellow_cones;

        cvtColor(img, img, cv::COLOR_BGR2HSV);
        // get all white cone (white == yellow == turn right)
        cv::inRange(img, cv::Scalar(170, 150, 150), cv::Scalar(190, 255, 255),
                    yellow_cones);
        // get all yellow cone (yellow == red == turn left)
        cv::inRange(img, cv::Scalar(30, 100, 150), cv::Scalar(50, 255, 255),
                    red_cones);

        // 2 - Get left and right cones
        cv::Mat all_cones;
        cv::max(yellow_cones, red_cones, all_cones);

        opening(all_cones);
        closing(all_cones);

        cv::Mat labels, stats, centroids;
        const auto nb_labels = cv::connectedComponentsWithStats(
            all_cones, labels, stats, centroids);

        std::cout << "components: " << nb_labels << '\n';

        const auto res = draw_boxes(all_cones, stats, nb_labels);

        return res;

        // 3 - Match cones together
        // 4 - Draw traject
    }
} // namespace blind
