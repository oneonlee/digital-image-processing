// < Mean-Shift Clustering >

#include <iostream>
using namespace std;

#include "opencv2/core.hpp"    // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;

// Low-level 구현
class Point5D; // Mean shift 구현을 위한 전용 포인트(픽셀) 클래스
class MeanShift;
void exMyMeanShift();

// OpenCV를 이용한 구현
void exCvMeanShift();

int main()
{
    exCvMeanShift();
    exMyMeanShift();

    return 0;
}

// Low-level 구현
// 구현의 편의성을 위해 전용 Point class 정의
class Point5D // Mean shift 구현을 위한 전용 포인트(픽셀) 클래스
{
public:
    float x, y, l, u, v; // 포인트의 좌표와 LUV 값

    Point5D()
    {
        x = -1;
        y = -1;
    }
    ~Point5D(){};

    void accumPt(Point5D);                         // 포인트 축적
    void copyPt(Point5D);                          // 포인트 복사
    float getColorDist(Point5D);                   // 색상 거리 계산
    float getSpatialDist(Point5D);                 // 좌표 거리 계산
    void scalePt(float);                           // 포인트 스케일링 함수 (평균용)
    void setPt(float, float, float, float, float); // 포인트값 설정함수
    void printPt();
};
void Point5D::accumPt(Point5D Pt)
{
    x += Pt.x;
    y += Pt.y;
    l += Pt.l;
    u += Pt.u;
    v += Pt.v;
}
void Point5D::copyPt(Point5D Pt)
{
    x = Pt.x;
    y = Pt.y;
    l = Pt.l;
    u = Pt.u;
    v = Pt.v;
}
float Point5D::getColorDist(Point5D Pt)
{
    return sqrt(pow(l - Pt.l, 2) +
                pow(u - Pt.u, 2) +
                pow(v - Pt.v, 2));
}
float Point5D::getSpatialDist(Point5D Pt)
{
    return sqrt(pow(x - Pt.x, 2) + pow(y - Pt.y, 2));
}
void Point5D::scalePt(float scale)
{
    x *= scale;
    y *= scale;
    l *= scale;
    u *= scale;
    v *= scale;
}
void Point5D::setPt(float px, float py, float pl, float pa, float pb)
{
    x = px;
    y = py;
    l = pl;
    u = pa;
    v = pb;
}
void Point5D::printPt()
{
    cout << x << " " << y << " " << l << " " << u << " " << v << endl;
}
// Mean-Shift Clustering 클래스 정의
class MeanShift
{ /* Mean shift 클래스 */
public:
    float bw_spatial = 8;                       // Spatial bandwidth
    float bw_color = 16;                        // Color bandwidth
    float min_shift_color = 0.1;                // 최소 컬러변화
    float min_shift_spatial = 0.1;              // 최소 워치변화
    int max_steps = 10;                         // 최대 반복횟수
    vector<Mat> img_split;                      // 채널별로 분할되는 Mat
    MeanShift(float, float, float, float, int); // Bandwidth 설정을 위한 생성자
    void doFiltering(Mat &);                    // Mean shift filtering 함수
};
MeanShift::MeanShift(float bs, float bc, float msc, float mss, int ms)
{ /* 생성자 */
    bw_spatial = bs;
    bw_color = bc;
    max_steps = ms;
    min_shift_color = msc;
    min_shift_spatial = mss;
}
void MeanShift::doFiltering(Mat &img)
{
    int height = img.rows;
    int width = img.cols;
    split(img, img_split);

    Point5D pt, pt_prev, pt_cur, pt_sum;

    int pad_left, pad_right, pad_top, pad_bottom;
    size_t n_pt, step;

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            pad_left = (col - bw_spatial) > 0 ? (col - bw_spatial) : 0;
            pad_right = (col + bw_spatial) < width ? (col + bw_spatial) : width;
            pad_top = (row - bw_spatial) > 0 ? (row - bw_spatial) : 0;
            pad_bottom = (row + bw_spatial) < height ? (row + bw_spatial) : height;

            // < 현재픽실 세팅 >
            pt_cur.setPt(row, col,
                         (float)img_split[0].at<uchar>(row, col),
                         (float)img_split[1].at<uchar>(row, col),
                         (float)img_split[2].at<uchar>(row, col));

            //< 주변픽셀 탐색 >
            step = 0;
            do
            {
                pt_prev.copyPt(pt_cur);
                pt_sum.setPt(0, 0, 0, 0, 0);
                n_pt = 0;
                for (int hx = pad_top; hx < pad_bottom; hx++)
                {
                    for (int hy = pad_left; hy < pad_right; hy++)
                    {
                        pt.setPt(hx, hy,
                                 (float)img_split[0].at<uchar>(hx, hy),
                                 (float)img_split[1].at<uchar>(hx, hy),
                                 (float)img_split[2].at<uchar>(hx, hy));

                        if (pt.getColorDist(pt_cur) < bw_color)
                        { // < Color bandwidth 안에서 축적 >
                            pt_sum.accumPt(pt);
                            n_pt++;
                        }
                    }
                }

                // < 축적결과를 기반으로 현재픽셀 갱신 >
                pt_sum.scalePt(1.0 / n_pt); // 축적결과 평균
                pt_cur.copyPt(pt_sum);
                step++;
            }

            while ((pt_cur.getColorDist(pt_prev) > min_shift_color) &&
                   (pt_cur.getSpatialDist(pt_prev) > min_shift_spatial) &&
                   (step < max_steps));
            // 변화량 최소조건을 만족할 때 까지 반복
            // 최대 반복횟수 조건도 포함

            img.at<Vec3b>(row, col) = Vec3b(pt_cur.l, pt_cur.u, pt_cur.v);
        }
    }
}
void exMyMeanShift()
{
    Mat img = imread("fruits.png");
    if (img.empty())
        exit(-1);
    cout << "----- exMyMeanShift() -----" << endl;

    resize(img, img, Size(256, 256), 0, 0, CV_INTER_AREA);
    imshow("Src", img);
    imwrite("exMyMeanShift_src.jpg", img);

    cvtColor(img, img, CV_RGB2Luv);

    MeanShift MSProc(8, 16, 0.1, 0.1, 10);
    MSProc.doFiltering(img);

    cvtColor(img, img, CV_Luv2RGB);

    imshow("Dst", img);
    waitKey();
    destroyAllWindows();
    imwrite("exMyMeanShift_dst.jpg", img);
}

// OpenCV를 이용한 구현
void exCvMeanShift()
{
    Mat img = imread("fruits.png");
    if (img.empty())
        exit(-1);
    cout << "----- exCvMeanShift () -----" << endl;
    resize(img, img, Size(256, 256), 0, 0, CV_INTER_AREA);
    imshow("Src", img);
    imwrite("exCvMeanShift_src.png", img);

    pyrMeanShiftFiltering(img, img, 8, 16);

    imshow("Dst", img);

    waitKey();
    destroyAllWindows();
    imwrite("exCvMeanShift_dst.png", img);
}