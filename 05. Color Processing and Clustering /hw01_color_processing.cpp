#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

Mat MyBgr2Hsv(Mat src_img)
{
    double b, g, r, h, s, v;
    Mat dst_img(src_img.size(), src_img.type());
    for (int y = 0; y < src_img.rows; y++)
    {
        for (int x = 0; x < src_img.cols; x++)
        {
            // conversion RGB to HSV

            b = (double)src_img.at<Vec3b>(y, x)[0];
            g = (double)src_img.at<Vec3b>(y, x)[1];
            r = (double)src_img.at<Vec3b>(y, x)[2];

            vector<double> vec = {r, g, b};
            double min_value = *min_element(vec.begin(), vec.end());
            double max_value = *max_element(vec.begin(), vec.end());

            v = max_value;

            if (v == 0)
                s = 0;
            else
                s = (max_value - min_value) / max_value;

            if (max_value == r)
                h = 0 + (g - b) / (max_value - min_value);
            else if (max_value == g)
                h = 2 + (b - r) / (max_value - min_value);
            else
                h = 4 + (r - g) / (max_value - min_value);
            h *= 60;

            if (h < 0)
                h += 360;

            h /= 2;
            s *= 255;

            h = std::max(0.0, std::min(255.0, h));
            s = std::max(0.0, std::min(255.0, s));
            v = std::max(0.0, std::min(255.0, v));

            dst_img.at<Vec3b>(y, x)[0] = (uchar)h;
            dst_img.at<Vec3b>(y, x)[1] = (uchar)s;
            dst_img.at<Vec3b>(y, x)[2] = (uchar)v;
            // cout << dst_img.at<Vec3b>(y, x) << endl;
        }
    }
    return dst_img;
}