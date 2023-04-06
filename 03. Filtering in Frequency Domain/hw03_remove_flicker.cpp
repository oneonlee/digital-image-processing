#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

Mat doIdft(Mat complexImg);
Mat setComplex(Mat magImg, Mat phaImg);
Mat centralize(Mat complex);
Mat myNormalize(Mat src);
Mat getPhase(Mat complexImg);
Mat getMagnitude(Mat complexImg);
Mat doDft(Mat srcImg);
Mat padding(Mat img);

void showMagnitude(Mat srcImg);
Mat removeFlicker(Mat srcImg);

int main()
{
    Mat srcImg = imread("img3.jpg", 0);
    imshow("Source Image", srcImg);
    showMagnitude(srcImg);

    Mat dstImg = removeFlicker(srcImg);
    imshow("Flicker Removed Image", dstImg);

    waitKey(0);
    destroyAllWindows();
}

Mat removeFlicker(Mat srcImg)
{
    Mat padImg = padding(srcImg);
    Mat complexImg = doDft(padImg);
    Mat centerComplexImg = centralize(complexImg);
    Mat magImg = getMagnitude(centerComplexImg);
    Mat phaImg = getPhase(centerComplexImg);

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(magImg, &minVal, &maxVal, &minLoc, &maxLoc);
    normalize(magImg, magImg, 0, 1, NORM_MINMAX);

    Mat maskImg = Mat::ones(magImg.size(), CV_32F);
    rectangle(maskImg, Rect(Point(maskImg.cols / 2 - 20, 0), Point(maskImg.cols / 2 + 20, maskImg.rows)), Scalar::all(0), -1);
    circle(maskImg, Point(maskImg.cols / 2, maskImg.rows / 2), 5, Scalar::all(1), -1, -1, 0);
    imshow("Mask Image", maskImg);

    Mat maskedMagImg;
    multiply(magImg, maskImg, maskedMagImg);
    imshow("Masked Magnitude Image", maskedMagImg);

    normalize(maskedMagImg, maskedMagImg, (float)minVal, (float)maxVal, NORM_MINMAX);
    Mat complexImg2 = setComplex(maskedMagImg, phaImg);
    Mat dstImg = doIdft(complexImg2);
    return myNormalize(dstImg);
}

void showMagnitude(Mat srcImg)
{
    Mat padImg = padding(srcImg);
    Mat complexImg = doDft(padImg);
    Mat centerComplexImg = centralize(complexImg);
    Mat magImg = getMagnitude(centerComplexImg);
    Mat phaImg = getPhase(centerComplexImg);

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(magImg, &minVal, &maxVal, &minLoc, &maxLoc);
    normalize(magImg, magImg, 0, 1, NORM_MINMAX);
    imshow("Magnitude Image", magImg);
}

Mat padding(Mat img)
{
    int dftRows = getOptimalDFTSize(img.rows);
    int dftCols = getOptimalDFTSize(img.cols);

    Mat padded;
    copyMakeBorder(img, padded, 0, dftRows - img.rows, 0, dftCols - img.cols, BORDER_CONSTANT, Scalar::all(0));
    return padded;
}
Mat doDft(Mat srcImg)
{
    Mat floatImg;
    srcImg.convertTo(floatImg, CV_32F);

    Mat complexImg;
    dft(floatImg, complexImg, DFT_COMPLEX_OUTPUT);

    return complexImg;
}
Mat getMagnitude(Mat complexImg)
{
    Mat planes[2];
    split(complexImg, planes);

    Mat magImg;
    magnitude(planes[0], planes[1], magImg);
    magImg += Scalar::all(1);
    log(magImg, magImg);

    return magImg;
}
Mat getPhase(Mat complexImg)
{
    Mat planes[2];
    split(complexImg, planes);

    Mat phaImg;
    phase(planes[0], planes[1], phaImg);

    return phaImg;
}
Mat myNormalize(Mat src)
{
    Mat dst;
    src.copyTo(dst);
    normalize(dst, dst, 0, 255, NORM_MINMAX);
    dst.convertTo(dst, CV_8UC1);
    return dst;
}
Mat centralize(Mat complex)
{
    Mat planes[2];
    split(complex, planes);
    int cx = planes[0].cols / 2;
    int cy = planes[1].rows / 2;

    Mat qORe(planes[0], Rect(0, 0, cx, cy));
    Mat q1Re(planes[0], Rect(cx, 0, cx, cy));
    Mat q2Re(planes[0], Rect(0, cy, cx, cy));
    Mat q3Re(planes[0], Rect(cx, cy, cx, cy));

    Mat tmp;
    qORe.copyTo(tmp);
    q3Re.copyTo(qORe);
    tmp.copyTo(q3Re);
    q1Re.copyTo(tmp);
    q2Re.copyTo(q1Re);
    tmp.copyTo(q2Re);

    Mat q0Im(planes[1], Rect(0, 0, cx, cy));
    Mat q1Im(planes[1], Rect(cx, 0, cx, cy));
    Mat q2Im(planes[1], Rect(0, cy, cx, cy));
    Mat q3Im(planes[1], Rect(cx, cy, cx, cy));

    q0Im.copyTo(tmp);
    q3Im.copyTo(q0Im);
    tmp.copyTo(q3Im);
    q1Im.copyTo(tmp);
    q2Im.copyTo(q1Im);
    tmp.copyTo(q2Im);

    Mat centerComplex;
    merge(planes, 2, centerComplex);

    return centerComplex;
}
Mat setComplex(Mat magImg, Mat phaImg)
{
    exp(magImg, magImg);
    magImg -= Scalar::all(1);

    Mat planes[2];
    polarToCart(magImg, phaImg, planes[0], planes[1]);

    Mat complexImg;
    merge(planes, 2, complexImg);

    return complexImg;
}
Mat doIdft(Mat complexImg)
{
    Mat idftcvt;
    idft(complexImg, idftcvt);

    Mat planes[2];
    split(idftcvt, planes);

    Mat dstImg;
    magnitude(planes[0], planes[1], dstImg);
    normalize(dstImg, dstImg, 255, 0, NORM_MINMAX);
    dstImg.convertTo(dstImg, CV_8UC1);

    return dstImg;
}