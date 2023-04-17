#include <iostream>
#include "opencv2/core/core.hpp"       // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
#include <filesystem>                  // 현재 경로를 얻기 위해 사용

using namespace cv;
using namespace std;
using std::__fs::filesystem::current_path; // 현재 경로를 얻기 위해 사용

string SRC_PATH = current_path() + "/src/";
string TRG_PATH = current_path() + "/trg/";

int main()
{
    Mat color_img = imread(SRC_PATH + "mark.jpg", 1);

    Mat gray_img;
    cvtColor(color_img, gray_img, CV_BGR2GRAY); // 이진화를 위해 color에서 grayscale 영상으로 변환

    Mat binary_img1, binary_img2;
    threshold(gray_img, binary_img1, 127, 255, THRESH_BINARY); // 임계값 지정 이진화
    threshold(gray_img, binary_img2, 0, 255, THRESH_OTSU);     // 임계값 자동 이진화 (OTSU 알고리즘)

    imshow("binary_img1", binary_img1);                   // 이미지 출력
    waitKey(0);                                           // 키 입력 대기 (0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("binary_img1");                         // 이미지 출력창 종료
    imwrite(TRG_PATH + "mark_binary_1.png", binary_img1); // 이미지 쓰기

    imshow("binary_img2", binary_img2);                   // 이미지 출력
    waitKey(0);                                           // 키 입력 대기 (0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("binary_img2");                         // 이미지 출력창 종료
    imwrite(TRG_PATH + "mark_binary_2.png", binary_img2); // 이미지 쓰기

    return 0;
}