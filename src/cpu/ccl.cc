#include "ccl.hh"

#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <queue>
#include <vector>

#include "../common/constants.hh"

namespace cpu
{
    namespace
    {
        bool marc(const cv::Mat &img, cv::Mat &marks, common::Blob &bob,
                  const cv::Point &current)
        {
            if (!current.inside(
                    cv::Rect(cv::Point(0, 0), cv::Point(img.cols, img.rows))))
                return false;

            auto is_marc = marks.at<unsigned char>(current);
            if (is_marc)
                return false;

            auto value = img.at<unsigned char>(current);
            if (value < common::fg_threshold)
                return false;

            if (value > bob.peak)
                bob.peak = value;

            if (current.x < bob.top_left.x)
                bob.top_left.x = current.x;
            else if (current.x > bob.bottom_right.x)
                bob.bottom_right.x = current.x;

            if (current.y < bob.top_left.y)
                bob.top_left.y = current.y;
            else if (current.y > bob.bottom_right.y)
                bob.bottom_right.y = current.y;

            marks.at<unsigned char>(current) = 1;

            return true;
        }

        common::Blob expend(const cv::Mat &img, cv::Mat &marks,
                            const cv::Point &current)
        {
            marks.at<unsigned char>(current) = 1;

            std::queue<cv::Point> queue;
            queue.push(current);

            common::Blob bob{
                current,
                current,
                img.at<unsigned char>(current),
            };

            while (!queue.empty())
            {
                const auto point = queue.front();

                const auto points = {
                    cv::Point(point.x - 1, point.y),
                    cv::Point(point.x + 1, point.y),
                    cv::Point(point.x, point.y - 1),
                    cv::Point(point.x, point.y + 1),
                };

                for (const auto &p : points)
                    if (marc(img, marks, bob, p))
                        queue.push(p);

                queue.pop();
            }
            return bob;
        }
    } // namespace

    std::vector<common::Blob> ccl(const cv::Mat &img)
    {
        std::vector<common::Blob> blobs;
        cv::Mat marks = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);

        for (auto row = 0; row < img.rows; row++)
            for (auto col = 0; col < img.cols; col++)
            {
                cv::Point coords(col, row);

                auto point = img.at<unsigned char>(coords);
                auto value = marks.at<unsigned char>(coords);

                if (point < common::fg_threshold or value != 0)
                    continue;

                const auto bob = expend(img, marks, coords);
                if (bob.peak > common::peak_threshold)
                    blobs.push_back(bob);
            }

        return blobs;
    }
} // namespace cpu
