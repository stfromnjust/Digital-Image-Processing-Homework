//
// Created by 16337 on 2021/6/21 0021.
//
#include "bmpFile.h"
#include <string>

using namespace std;

const int MAX_POINT_NUM = 512 * 512;
const double PI = acos(-1);

void t2_1()
{
    int width, height, N, maxRho;
    int bstTheta, bstRho;
    double A, B, C;
    BYTE *pImg = read8BitBmp2Img("../resource/H0602Bin.bmp", &width, &height);
    BYTE *pResImg = read8BitBmp2Img("../resource/H0602Bin.bmp", &width, &height);
    BYTE *pCountImg;
    int *xIdx, *yIdx;
    int *pCount;
    // 申请边缘点空间
    xIdx = new int[MAX_POINT_NUM];
    yIdx = new int[MAX_POINT_NUM];
    // 得到maxRho, 霍夫计数器空间大小
    maxRho = int(sqrt(1.0 * width * width + height * height)) + 1;
    // 申请霍夫计数器空间
    pCount = new int[(maxRho * 2 + 1) * 180]();
    pCountImg = new BYTE[(maxRho * 2 + 1) * 180]();
    for (int i = 0; i < 4; i++)
    {
        // 清空边缘点空间
        memset(xIdx, 0, sizeof(int) * MAX_POINT_NUM);
        memset(yIdx, 0, sizeof(int) * MAX_POINT_NUM);
        // 获取边缘点
        N = collectEdgePoints_H(pImg, width, height, xIdx, yIdx);
        // 计数
        getHoughCount_Line(xIdx, yIdx, N, maxRho, pCount);
#ifdef MY_DEBUG
        // 调试 输出计数器图片
        getCountImg(pCount, maxRho * 2 + 1, 180, pCountImg);
        write8BitImg2Bmp(pCountImg, maxRho * 2 + 1, 180, ("../output/t2_1/CountImg"+ to_string(i) + ".bmp").c_str());
#endif
        // 取霍夫计数器中最大值对应直线
        getBstHoughLine(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho);
        A = cos(bstTheta * PI / 180);
        B = sin(bstTheta * PI / 180);
        C = -bstRho;
        // 画出直线
        drawABCLine(pResImg, width, height, A, B, C, 200 + i);
        write8BitImg2BmpMark(pResImg, width, height, ("../output/t2_1/H0602Bin_step" + to_string(i) + ".bmp").c_str());
        // 擦去该直线附近的点
        eraseABCLine(pImg, width, height, A, B, C, 255);
    }
}

void t2_2()
{
    int width, height, N, maxRho;
    int bstTheta, bstRho;
    double A, B, C;
    BYTE *pImg = read8BitBmp2Img("../resource/H0602Bin.bmp", &width, &height);
    BYTE *pCountImg;
    int *xIdx, *yIdx, *pCount;
    // 申请边缘点空间
    xIdx = new int[MAX_POINT_NUM]();
    yIdx = new int[MAX_POINT_NUM]();
    // 得到maxRho, 霍夫计数器空间大小
    maxRho = int(sqrt(1.0 * width * width + height * height)) + 1;
    // 申请霍夫计数器空间
    pCount = new int[(maxRho * 2 + 1) * 180]();
    pCountImg = new BYTE[(maxRho * 2 + 1) * 180]();
    // 获取边缘点
    N = collectEdgePoints_H(pImg, width, height, xIdx, yIdx);
    // 计数
    getHoughCount_Line(xIdx, yIdx, N, maxRho, pCount);
    for (int i = 0; i < 4; i++)
    {
#ifdef MY_DEBUG
        // 调试 输出计数器图片
        getCountImg(pCount, maxRho * 2 + 1, 180, pCountImg);
        write8BitImg2Bmp(pCountImg, maxRho * 2 + 1, 180, ("../output/t2_2/CountImg"+ to_string(i) + ".bmp").c_str());
#endif
        // 取霍夫计数器中最大值对应直线
        getBstHoughLine(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho);
        A = cos(bstTheta * PI / 180);
        B = sin(bstTheta * PI / 180);
        C = -bstRho;
        // 画出直线
        drawABCLine(pImg, width, height, A, B, C, 200 + i);
        // 霍夫计数器周围清0
        eraseCountImg(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho, 10);
        if (bstTheta <= 10)
        {
            bstTheta = 180 - bstTheta;
            bstRho = -bstRho;
            eraseCountImg(pCount, maxRho * 2 + 1, 180, &bstTheta, &bstRho, 10);
        }
        write8BitImg2BmpMark(pImg, width, height, ("../output/t2_2/H0602Bin_step" + to_string(i) + ".bmp").c_str());
    }
}

void t2_3()
{
    int width, height, N, maxRho;
    int bstTheta, bstRho;

    double A, B, C;


}

void t3_1()
{
    int width, height;
    int circleX, circleY;
    BYTE *pImg = read8BitBmp2Img("../resource/H0603Bin.bmp", &width, &height);
    BYTE *pCountXImg, *pCountYImg;
    int *pCountX, *pCountY;
    // 获取X/Y直方图空间
    pCountX = new int[width]();
    pCountY = new int[height]();
    pCountXImg = new BYTE[width * 100];
    pCountYImg = new BYTE[height * 100];
    // 计数
    circleX = getCircleX(pImg, width, height, pCountX);
    circleY = getCircleY(pImg, width, height, pCountY);
#ifdef MY_DEBUG
    cout << "circleX: " << circleX << "\tcircleY: " << circleY << endl;
    getCountImg(pCountX, width, 1.0, pCountXImg, width, 100);
    getCountImg(pCountY, height, 1.0, pCountYImg, height, 100);
    write8BitImg2Bmp(pCountXImg, width, 100, "../output/t3_1/H0603Bin_CountXImg.bmp");
    write8BitImg2Bmp(pCountYImg, height, 100, "../output/t3_1/H0603Bin_CountYImg.bmp");
#endif

}

int main(void)
{
//    t2_1();
//    t2_2();
    t3_1();
    return 0;
}

