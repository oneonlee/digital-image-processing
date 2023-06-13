#include <iostream>
#include <vector>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include "opencv2/objdetect.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/stitching.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace cv;

void ex_panorama_simple();

int main()
{
    ex_panorama_simple();

    return 0;
}

void ex_panorama_simple()
{
    Mat img;
    vector<Mat> imgs;
    img = imread("img1/pano_left.jpg", IMREAD_COLOR);
    imgs.push_back(img);
    img = imread("img1/pano_center.jpg", IMREAD_COLOR);
    imgs.push_back(img);

    Mat result;
    Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA, false);
    Stitcher::Status status = stitcher->stitch(imgs, result);
    if (status != Stitcher::OK)
    {
        // 공통되는 영역이 너무 적으면 error code 1을 출력
        cout << "Can't stitch images, error code = " << int(status) << endl;
        exit(-1);
    }

    imshow("ex_panorama_simple_result", result);
    imwrite("result/ex_panorama_simple_result.png", result);
    waitKey();
    destroyAllWindows();
}