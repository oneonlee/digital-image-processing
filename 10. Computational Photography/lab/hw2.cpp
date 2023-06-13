#include <iostream>
#include <vector>
using namespace std;

#include "opencv2/core.hpp"      // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp"   // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp"   // 각종 이미지 처리 함수를 포함하는 헤더
#include <opencv2/photo.hpp>     // HDR 관련 함수를 포함하는 헤더
#include <opencv2/imgcodecs.hpp> // 이미지 코덱 관련 함수를 포함하는 헤더
using namespace cv;

void hw2();
void readImagesAndTimes(vector<Mat> &images, vector<float> &times);

int main()
{
    hw2();
    return 0;
}

void readImagesAndTimes(vector<Mat> &images, vector<float> &times)
{
    int numImages = 6;
    static const float timesArray[] = {1.0f, 1 / 3.0f, 1 / 6.0f, 1 / 30.0f, 1 / 195.0f, 1 / 1018.0f};
    times.assign(timesArray, timesArray + numImages);
    static const char *fileNames[] = {"src/1_1.JPG", "src/1_3.JPG", "src/1_6.JPG", "src/1_30.JPG", "src/1_195.JPG", "src/1_1018.JPG"};
    for (int i = 0; i < numImages; i++)
    {
        Mat im = imread(fileNames[i]);
        images.push_back(im);
    }
}

void hw2()
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
    cout << "----- CRF -----" << endl;
    cout << responseDebevec << endl;

    // [ 24bit 표현 범위로 이미지 병합 ]
    // CRF 기반 실제 밝기(radiance) 스케일에 대응되도록 영상 병합
    // 이를 위해 매우 넓은 표현 범위를 가지는 24bit 타입을 사용
    // 어두운 영역은 보다 노출이 큰 영상을 참고하고 밝은영역은 보다 노출이 작은 영역을 참고하는 형태를 기반으로 다양한 알고리즘 존재
    cout << "Merging images to one HDR image ..." << endl;
    Mat hdrDebevec;
    Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
    mergeDebevec->process(images, hdrDebevec, times, responseDebevec);
    imwrite("hdrDebevec.hdr", hdrDebevec);
    cout << "saved hdrDebvec.hdr" << endl;
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
    Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0f, 0.7f, 0.85f);
    tonemapDrago->process(hdrDebevec, IdrDrago);
    IdrDrago = 3 * IdrDrago;
    imwrite("Idr-Drago.jpg", IdrDrago * 255);
    cout << "Saved Idr-Drago.jpg" << endl;

    // < Reinhard 톤맵 >
    cout << "Tonemapping using Reinhard's method ..." << endl;
    Mat IdrReinhard;
    Ptr<TonemapReinhard> tonemapReinhard = createTonemapReinhard(1.5f, 0.0f, 0.0f, 0.0f);
    tonemapReinhard->process(hdrDebevec, IdrReinhard);
    imwrite("Idr-Reinhard.jpg", IdrReinhard * 255);
    cout << "Saved Idr-Reinhard.jpg" << endl;

    // < Mantiuk 톤맵 >
    cout << "Tonemapping using Mantiuk's method ..." << endl;
    Mat IdrMantiuk;
    Ptr<TonemapMantiuk> tonemapMantiuk = createTonemapMantiuk(2.2f, 0.85f, 1.2f);
    tonemapMantiuk->process(hdrDebevec, IdrMantiuk);
    IdrMantiuk = 3 * IdrMantiuk;
    imwrite("Idr-Mantiuk.jpg", IdrMantiuk * 255);
    cout << "Saved Idr-Mantiuk.jpg" << endl;
}