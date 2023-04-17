#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <filesystem> // 현재 경로를 얻기 위해 사용

using namespace cv;
using namespace std;
using std::__fs::filesystem::current_path; // 현재 경로를 얻기 위해 사용

string SRC_PATH = current_path() + "/src/";

void spreadRGBSalts(Mat img, int num[]);
void countRGBSalts(Mat img);

int main()
{
    Mat src_img = imread(SRC_PATH + "img1.jpg", 1);

    int num[3];
    num[0] = 200;
    num[1] = 200;
    num[2] = 200;

    spreadRGBSalts(src_img, num);
    countRGBSalts(src_img);

    imshow("hw01 - 12181879 이동건", src_img);
    waitKey(0);
    destroyWindow("hw01 - 12181879 이동건");

    return 0;
}

void spreadRGBSalts(Mat img, int num[])
{
    for (int color = 0; color < 3; color++)
    {

        for (int i = 0; i < num[color]; i++)
        {
            int x = rand() % img.cols;
            int y = rand() % img.rows;

            if (color == 0)
            {
                img.at<Vec3b>(y, x)[0] = 255; // blue
                img.at<Vec3b>(y, x)[1] = 0;
                img.at<Vec3b>(y, x)[2] = 0;
            }

            else if (color == 1)
            {
                img.at<Vec3b>(y, x)[0] = 0;
                img.at<Vec3b>(y, x)[1] = 255; // green
                img.at<Vec3b>(y, x)[2] = 0;
            }

            else if (color == 2)
            {
                img.at<Vec3b>(y, x)[0] = 0;
                img.at<Vec3b>(y, x)[1] = 0;
                img.at<Vec3b>(y, x)[2] = 255; // red
            }
        }
    }
}

void countRGBSalts(Mat img)
{
    int num[3] = {};

    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            if (img.at<Vec3b>(i, j)[0] == 255 && img.at<Vec3b>(i, j)[1] == 0 && img.at<Vec3b>(i, j)[2] == 0) // blue
            {
                num[0]++;
            }

            else if (img.at<Vec3b>(i, j)[0] == 0 && img.at<Vec3b>(i, j)[1] == 255 && img.at<Vec3b>(i, j)[2] == 0) // green
            {
                num[1]++;
            }
            else if (img.at<Vec3b>(i, j)[0] == 0 && img.at<Vec3b>(i, j)[1] == 0 && img.at<Vec3b>(i, j)[2] == 255) // red
            {
                num[2]++;
            }
        }
    }

    cout << "Count of  BLUE dots : " << num[0] << endl;
    cout << "Count of GREEN dots : " << num[1] << endl;
    cout << "Count of   RED dots : " << num[2] << endl;
}
