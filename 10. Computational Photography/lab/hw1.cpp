#include <iostream>
#include <vector>
using namespace std;

#include "opencv2/core.hpp"      // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp"   // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp"   // 각종 이미지 처리 함수를 포함하는 헤더
#include <opencv2/photo.hpp>     // HDR 관련 함수를 포함하는 헤더
#include <opencv2/imgcodecs.hpp> // 이미지 코덱 관련 함수를 포함하는 헤더
using namespace cv;

void hw1();
void readImagesAndTimes(vector<Mat> &images, vector<float> &times);
Mat getHistogram(Mat src);

int main()
{
    imshow("hist_0.25", getHistogram(imread("src/img_0.25.jpg", IMREAD_COLOR)));
    imshow("hist_0.033", getHistogram(imread("src/img_0.033.jpg", IMREAD_COLOR)));
    imshow("hist_2.5", getHistogram(imread("src/img_2.5.jpg", IMREAD_COLOR)));
    imshow("hist_15", getHistogram(imread("src/img_15.jpg", IMREAD_COLOR)));
    waitKey(0);
    destroyAllWindows();

    hw1();
    return 0;
}

Mat getHistogram(Mat src)
{
    Mat histogram;
    const int *channel_numbers = {0};
    float channel_range[] = {0.0, 255.0};
    const float *channel_ranges = channel_range;
    int number_bins = 255;

    cvtColor(src, src, CV_RGB2GRAY); // gray scale로 변환

    // 히스토그램 계산
    calcHist(&src, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);

    // 히스토그램 plot
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / number_bins);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
    normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat()); // 정규화

    for (int i = 1; i < number_bins; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(histogram.at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0); // 값과 값을 잇는 선을 그리는 방식으로 plot
    }

    return histImage;
}

void readImagesAndTimes(vector<Mat> &images, vector<float> &times)
{
    int numImages = 4;
    static const float timesArray[] = {1 / 30.0f, 0.25f, 2.5f, 15.0f};
    times.assign(timesArray, timesArray + numImages);
    static const char *fileNames[] = {"src/img_0.033.jpg", "src/img_0.25.jpg", "src/img_2.5.jpg", "src/img_15.jpg"};
    for (int i = 0; i < numImages; i++)
    {
        Mat im = imread(fileNames[i]);
        images.push_back(im);
    }
}

void hw1()
{
    // [ 다양한 노출의 영상과 노출 시간 읽기 ]
    // 다양한 노출의 영상들을 Mat vector에 저장
    // 동일한 순서로 각 노출 영상의 노출 시간을 char vector에 저장
    cout << "Reading images and exposure times ..." << endl;
    vector<Mat> images;
    vector<float> times;
    readImagesAndTimes(images, times);
    cout << "Finished" << endl;

    // [ 영상 정렬 ]
    // 영상을 완전히 정렬해 정확한 밝기 병합이 가능하도록 하는 과정
    // 삼각대 등을 이용해 완전히 고정된 상태에서 영상을 취득하는 것이 더 중요
    // 영상의 평균 밝기보다 작으면 0, 크면 1로 나타내는 median threshold bitmaps(MTB)를 이용해 마스크를 구하고 정렬을 수행하도록 구현
    cout << "Aligning images ..." << endl;
    Ptr<AlignMTB> alignMTB = createAlignMTB();
    alignMTB->process(images, images);

    // [ Camera response function (CRF) 복원 ]
    // 픽셀 값은 실제 밝기(radiance)와 달리 선형적으로 비교할 수 없으므로 이에 대한 함수를 복원 (로그 스케일을 가짐)
    cout << "Calculating Camera Response Function (CRF) ..." << endl;
    Mat responseDebevec;
    Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
    calibrateDebevec->process(images, responseDebevec, times);
    // cout << "----- CRF -----" << endl;
    // cout << responseDebevec << endl;

    // [ 24bit 표현 범위로 이미지 병합 ]
    // CRF 기반 실제 밝기(radiance) 스케일에 대응되도록 영상 병합
    // 이를 위해 매우 넓은 표현 범위를 가지는 24bit 타입을 사용
    // 어두운 영역은 보다 노출이 큰 영상을 참고하고 밝은영역은 보다 노출이 작은 영역을 참고하는 형태를 기반으로 다양한 알고리즘 존재
    cout << "Merging images to one HDR image ..." << endl;
    Mat hdrDebevec;
    Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
    mergeDebevec->process(images, hdrDebevec, times, responseDebevec);
    // imwrite("hdrDebevec.hdr", hdrDebevec);
    // cout << "saved hdrDebvec.hdr" << endl;
    /*
    cout <<"Meging using Exposure Fusion ..." <<endl;
    Mat hdrDebevec;
    Ptr<MergeMertens> mergeMertens = createMergeMertens();
    mergeMertens->process(images, hdrDebevec);
    */

    // [ 24bit → 8bit 톤 매핑 ]
    // 24bit 타입은 컴퓨터가 출력할 수 없으므로 이를 다시 8bit로 표현
    // 제한된 표현범위 안에서 시각적으로 괜찮은 결과를 얻기 위한 다양한 톤 매핑 기법들이 존재
    // 주로 감마, 채도, 대비 등을 매개변수로 결정

    // < Drago 톤맵 >
    cout << "Tonemapping using Drago's method ..." << endl;
    Mat IdrDrago;
    float bias = 0.8f; // 일반적으로 0.7 ~ 0.9 사이의 값이 좋은 결과를 가짐
    Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0f, 0.7f, bias);
    tonemapDrago->process(hdrDebevec, IdrDrago);
    IdrDrago = 3 * IdrDrago;
    // imwrite("Idr-Drago.jpg", IdrDrago * 255);
    // cout << "Saved Idr-Drago.jpg" << endl;

    Mat histDrago = getHistogram(IdrDrago * 255);
    imshow("hist_Drago", histDrago);

    waitKey(0);
}