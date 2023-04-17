#include <iostream>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

// < Frequency Domain >
Mat mySobelFilterInFrequencyDomain(Mat srcImg);
Mat doIdft(Mat complexImg);
Mat setComplex(Mat magImg, Mat phaImg);
Mat centralize(Mat complex);
Mat myNormalize(Mat src);
Mat getPhase(Mat complexImg);
Mat getMagnitude(Mat complexImg);
Mat doDft(Mat srcImg);
Mat padding(Mat img);

// < Spatial Domain >
Mat mySobelFilterInSpatialDomain(Mat srcImg);
int myKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height);

int main()
{
    Mat srcImg = imread("img2.jpg", 0);
    Mat frequencyDstImg = mySobelFilterInFrequencyDomain(srcImg);
    Mat spatialDstImg = mySobelFilterInSpatialDomain(srcImg);

    imshow("Original Image", srcImg);
    imshow("Sobel Filter in Frequency Domain", frequencyDstImg);
    imshow("Sobel Filter in Spatial Domain", spatialDstImg);

    waitKey(0);
    destroyAllWindows();

    return 0;
}

Mat mySobelFilterInFrequencyDomain(Mat srcImg)
{
    Mat padImg = padding(srcImg);
    Mat complexImg = doDft(padImg);
    Mat centerComplexImg = centralize(complexImg);
    Mat magImg = getMagnitude(centerComplexImg);
    Mat phaImg = getPhase(centerComplexImg);

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(magImg, &minVal, &maxVal, &minLoc, &maxLoc);
    magImg /= maxVal; // normalize the magnitude image

    Mat sobelX = Mat::zeros(srcImg.size(), CV_32F);
    sobelX.at<float>(0, 0) = -1;
    sobelX.at<float>(0, 2) = 1;
    sobelX.at<float>(1, 0) = -2;
    sobelX.at<float>(1, 2) = 2;
    sobelX.at<float>(2, 0) = -1;
    sobelX.at<float>(2, 2) = 1;
    Mat padImg_sobelX = padding(sobelX);
    Mat complexImg_sobelX = doDft(padImg_sobelX);
    Mat centerComplexImg_sobelX = centralize(complexImg_sobelX);
    Mat magImg_sobelX = getMagnitude(centerComplexImg_sobelX);
    Mat phaImg_sobelX = getPhase(centerComplexImg_sobelX);
    magImg_sobelX /= maxVal; // normalize the magnitude image

    Mat sobelY = Mat::zeros(srcImg.size(), CV_32F);
    sobelY.at<float>(0, 0) = -1;
    sobelY.at<float>(0, 1) = -2;
    sobelY.at<float>(0, 2) = -1;
    sobelY.at<float>(2, 0) = 1;
    sobelY.at<float>(2, 1) = 2;
    sobelY.at<float>(2, 2) = 1;
    Mat padImg_sobelY = padding(sobelY);
    Mat complexImg_sobelY = doDft(padImg_sobelY);
    Mat centerComplexImg_sobelY = centralize(complexImg_sobelY);
    Mat magImg_sobelY = getMagnitude(centerComplexImg_sobelY);
    Mat phaImg_sobelY = getPhase(centerComplexImg_sobelY);
    magImg_sobelY /= maxVal; // normalize the magnitude image

    Mat magImgX;
    multiply(magImg, magImg_sobelX, magImgX);
    Mat magImgXY;
    multiply(magImgX, magImg_sobelY, magImgXY);

    Mat complexImgXY = setComplex(magImgXY, phaImg);
    Mat dstImg = doIdft(complexImgXY);

    return myNormalize(dstImg);
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

Mat mySobelFilterInSpatialDomain(Mat srcImg)
{
    int kernelX[3][3] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1};
    int kernelY[3][3] = {-1, -2, -1,
                         0, 0, 0,
                         1, 2, 1};

    Mat dstImg(srcImg.size(), CV_8UC1);
    uchar *srcData = srcImg.data;
    uchar *dstData = dstImg.data;
    int width = srcImg.cols;
    int height = srcImg.rows;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernelX, x, y, width, height)) +
                                      abs(myKernelConv3x3(srcData, kernelY, x, y, width, height))) /
                                     2;
        }
    }

    return dstImg;
}
int myKernelConv3x3(uchar *arr, int kernel[3][3], int x, int y, int width, int height)
{
    int sum = 0;
    int sumKernel = 0;

    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width)
            {
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
            }
        }
    }
    if (sumKernel != 0)
    {
        return sum / sumKernel;
    }
    else
    {
        return sum;
    }
}