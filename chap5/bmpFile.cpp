//
// Created by 16337 on 2021/6/10 0010.
//
#include "bmpFile.h"

//#define Debug
#define PI 3.1415926

using namespace std;

BYTE *read8BitBmp2Img(const char *filename, int *width, int *height)
{   //BmpHeader记录的真实的宽度,
    //读出的是真实的宽度,去掉了4对齐的扩展.
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    BYTE *pImg;
    unsigned int size;
    int Suc = 1, w, h;
    int k, extend, tmp;

    // Open File
    *width = *height = 0;
    if ((fp = fopen(filename, "rb")) == nullptr) return nullptr;
    // Read Struct Info
    if (fread((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = -1;
    if (fread((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = -1;
    if ((Suc == -1) ||
        (FileHeader.bfOffBits < sizeof(FileHeader) + sizeof(BmpHeader))
            )
    {
        fclose(fp);
        return nullptr;
    }
    // Read Image Data
    *width = w = BmpHeader.biWidth;
    *height = h = BmpHeader.biHeight;
    extend = (w + 3) / 4 * 4 - w;
    size = BmpHeader.biWidth * BmpHeader.biHeight;
    fseek(fp, (long) FileHeader.bfOffBits, SEEK_SET);
    if ((pImg = new BYTE[size]) != nullptr)
    {
        for (int i = 0; i < h; i++)  // 0,1,2,3,4(5): 400-499
        {
            if ((int) fread(pImg + (h - 1 - i) * w, sizeof(BYTE), w, fp) != w)
            {
                fclose(fp);
                delete pImg;
                pImg = nullptr;
                return nullptr;
            }
            //读掉扩充的数据
            for (k = 0; k < extend; k++)
            {
                if (fread(&tmp, 1, 1, fp) != 1)
                {
                    fclose(fp);
                    delete pImg;
                    pImg = nullptr;
                    return nullptr;

                }
            }
        }
    }
    fclose(fp);
    return pImg;
}

bool write8BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename)
{   //BmpHeader记录的真实的宽度
    //当每行字节个数不是4的倍数时,需要进行4对齐
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i, extend;
    bool Suc = true;
    BYTE p[4], *pCur;

    // Open File
    if ((fp = fopen(filename, "w+b")) == nullptr)
    { return false; }
    // Fill the FileHeader
    FileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4L;
    FileHeader.bfSize = FileHeader.bfOffBits + width * height;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    if (fwrite((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = false;
    // Fill the ImgHeader
    BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
    BmpHeader.biHeight = height;
    BmpHeader.biPlanes = 1;
    BmpHeader.biBitCount = 8;
    BmpHeader.biCompression = 0;
    BmpHeader.biSizeImage = 0;
    BmpHeader.biXPelsPerMeter = 0;
    BmpHeader.biYPelsPerMeter = 0;
    BmpHeader.biClrUsed = 0;
    BmpHeader.biClrImportant = 0;
    if (fwrite((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = false;
    // write Palette
    for (i = 0; i < 256; i++)
    {
        p[3] = 0;
        p[0] = p[1] = p[2] = i; //blue,green,red
        if (fwrite((void *) p, 1, 4, fp) != 4)
        {
            Suc = false;
            break;
        }
    }
    // write image data
    extend = (width + 3) / 4 * 4 - width; //每行字节个数需要按4的倍数对齐
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false; //真实的数据
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * width; pCur >= pImg; pCur -= width)
        {
            if (fwrite((void *) pCur, 1, width, fp) != (unsigned int) width) Suc = false; //真实的数据
            for (i = 0; i < extend; i++) //每行按4字节对齐进行扩充
            {
                if (fwrite((void *) (pCur + width - 1), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

BYTE *read24BitBmp2Img(const char *filename, int *width, int *height)
{   //BmpHeader记录的真实的宽度,
    //读出的是真实的宽度,去掉了4对齐的扩展.
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    BYTE *pImg;
    unsigned int size;
    int Suc = 1, w, h, k, extend, tmp;

    // Open File
    *width = *height = 0;
    if ((fp = fopen(filename, "rb")) == nullptr) return nullptr;
    // Read Struct Info
    if (fread((void *) &FileHeader, 1, sizeof(FileHeader), fp) != sizeof(FileHeader)) Suc = -1;
    if (fread((void *) &BmpHeader, 1, sizeof(BmpHeader), fp) != sizeof(BmpHeader)) Suc = -1;
    if ((Suc == -1) ||
        (FileHeader.bfOffBits < sizeof(FileHeader) + sizeof(BmpHeader))
            )
    {
        fclose(fp);
        return nullptr;
    }
    // Read Image Data
    *width = w = BmpHeader.biWidth;
    *height = h = BmpHeader.biHeight;
    extend = ((w * 3 + 3) / 4 * 4) - w * 3; //每行字节个数需要按4的倍数对齐
    size = (*width) * (*height) * 3;
    fseek(fp, (long) FileHeader.bfOffBits, SEEK_SET); //bfOffBytes
    if ((pImg = new BYTE[size]) != nullptr)
    {
        for (int i = 0; i < h; i++)
        {
            if ((int) fread(pImg + (h - 1 - i) * w * 3, sizeof(BYTE), w * 3, fp) != w * 3)
            {
                fclose(fp);
                delete pImg;
                pImg = nullptr;
                return nullptr;
            }
            for (k = 0; k < extend; k++) //每行按4字节对齐进行扩充
            {
                if (fwrite((void *) &tmp, sizeof(BYTE), 1, fp) != 1) Suc = false;
            }
        }
    }
    fclose(fp);
    return pImg;
}

bool write24BitImg2Bmp(BYTE *pImg, int width, int height, const char *filename)
{   //BmpHeader记录的真实的宽度
    //当每行字节个数不是4的倍数时,需要进行4对齐
    FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    bool Suc = true;
    int i, extend;
    BYTE *pCur;
    // Open File
    if ((fp = fopen(filename, "w+b")) == nullptr)
    {
        return false;
    }
    // Fill the FileHeader
    FileHeader.bfType = ((WORD) ('M' << 8) | 'B');
    FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    FileHeader.bfSize = FileHeader.bfOffBits + width * height * 3L;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    if (fwrite((void *) &FileHeader, 1, sizeof(BITMAPFILEHEADER), fp) != sizeof(BITMAPFILEHEADER)) Suc = false;
    // Fill the ImgHeader
    BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
    BmpHeader.biHeight = height;
    BmpHeader.biPlanes = 1;
    BmpHeader.biBitCount = 24;
    BmpHeader.biCompression = 0;
    BmpHeader.biSizeImage = 0;
    BmpHeader.biXPelsPerMeter = 0;
    BmpHeader.biYPelsPerMeter = 0;
    BmpHeader.biClrUsed = 0;
    BmpHeader.biClrImportant = 0;
    if (fwrite((void *) &BmpHeader, 1, sizeof(BITMAPINFOHEADER), fp) != sizeof(BITMAPINFOHEADER)) Suc = false;
    // write image data
    extend = ((width * 3 + 3) / 4 * 4) - width * 3; //每行字节个数需要按4的倍数对齐
    if (extend == 0)
    {
        for (pCur = pImg + (height - 1) * 3 * width; pCur >= pImg; pCur -= 3 * width)
        {
            if (fwrite((void *) pCur, 1, width * 3, fp) != (unsigned int) (3 * width)) Suc = false; //真实的数据
        }
    }
    else
    {
        for (pCur = pImg + (height - 1) * 3 * width; pCur >= pImg; pCur -= 3 * width)
        {
            if (fwrite((void *) pCur, 1, width * 3, fp) != (unsigned int) (3 * width)) Suc = false; //真实的数据
            for (i = 0; i < extend; i++) //扩充的数据
            {
                if (fwrite((void *) (pCur + 3 * (width - 1) + 0), 1, 1, fp) != 1) Suc = false;
            }
        }
    }
    // return;
    fclose(fp);
    return Suc;
}

void getHistogram(BYTE *pImg, int width, int height, int *pHistogram)
{
    BYTE *pCur, *pEnd = pImg + width * height;
    memset(pHistogram, 0, sizeof(int) * 256);
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        pHistogram[*pCur]++;
    }
}

void histogramSmooth(int *pHistogram, int m, int nSize, int *pResHistogram)
{
    int x, sum;
    int *pRes;
    m = m / 2 * 2 + 1;
    int halfx = m / 2;
    int c = (1 << 10) / m;
    for (x = 0, sum = 0; x < m; x++)
    {
        sum += pHistogram[x];
    }
    for (x = 0; x < halfx; x++)
    {
        pResHistogram[x] = pHistogram[x];
    }
    for (x = halfx, pRes = pResHistogram + halfx; x < nSize - halfx; x++, pRes++)
    {
//        *pRes = sum / m;
        *pRes = (sum * c) >> 10;
        sum -= pHistogram[x - halfx];
        sum += pHistogram[x + halfx + 1];
    }
    for (x = nSize - halfx; x < nSize; x++)
    {
        pResHistogram[x] = pHistogram[x];
    }
}

void drawHistogram2Img(int *pHistogram, int histSize,
                       double maxScale,
                       BYTE *pHistImg, int width, int height)
{
    int i, maxV;
    int y1, x1, x2, x, y;
//    1. 初始化
    memset(pHistImg, 255, width * height);
//    2. 最大值
    maxV = 0;
    for (i = 0; i < histSize; i++)
    {
        maxV = max(pHistogram[i], maxV);
    }
    maxV = (int) (maxV * maxScale);
    //    3. 像素填充
    for (i = 0; i < histSize; i++)
    {
        y1 = height - 1 - pHistogram[i] * (height - 1) / (maxV + 1);
        y1 = max(0, y1);
        x1 = i * width / histSize;
        x2 = (i + 1) * width / histSize;
        for (x = x1; x < x2; x++)
        {
            for (y = y1; y < height - 1; y++)
            {
                pHistImg[y * width + x] = 0;
            }
        }
    }
}

void thresholdBin(BYTE *pImg, int width, int height, int threshold)
{
    BYTE *pCur, *pEnd = pImg + width * height;
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *pCur = (*(pCur) >= threshold) * 255;
    }
}

int getOtsuThreshold(int *pHistogram, int nSize)
{
    int thre;
    int i, gmin, gmax;
    int s1, n1, s2, n2, n; // 两类目标
    double dist, f, maxf = -1;
//    1. 确定左边界, 右边界
    gmin = 0;
    while(pHistogram[gmin] == 0 && gmin < nSize)
    {
        gmin++;
    }
    gmax = nSize - 1;
    while(pHistogram[gmax] == 0 && gmax >= 0)
    {
        gmax--;
    }
    if (gmin == gmax) return gmin;
    if (gmin == nSize || gmax == -1) return 0; //TODO check
//    2. 初始化
    s1 = n1 = 0;
    for (s2 = n2 = 0, i = gmin; i <= gmax; i++)
    {
        s2 += pHistogram[i] * i;
        n2 += pHistogram[i];
    }
    n = n2; //  总像素个数
    for (i = gmin; i <= gmax; i++)  // 2类至少有一个
    {
        if (!pHistogram[i]) continue;   // 加速
        s1 += pHistogram[i] * i;
        n1 += pHistogram[i];
        s2 -= pHistogram[i] * i;
        n2 -= pHistogram[i];
        dist = s1 * 1.0 / n1 - s2 * 1.0 / n2;   //  u1 - u2
        f = dist * dist * ((n1 * 1.0) / n) * ((n2 * 1.0) / n);
        if (f > maxf)
        {
            maxf = f;
            thre = i;
        }
    }
    return thre + 1;
}

void reduceGrayscale(BYTE *pImg, int width, int height, int offset)
{
    BYTE *pCur, *pEnd = pImg + width * height;
    for (pCur = pImg; pCur < pEnd; pCur++)
    {
        *pCur = *pCur >> offset;
    }
}

bool clusterHistogram(int *pHistogram, int nClass, double decision, double *center)
{
    int gmin, gmax;
    double step, dist;
    double sum[256];
    int num[256];
    double centerPre[256];
    double minDist;
    int i, j, type;
    bool flag = false;

    if (nClass < 2) return false;
//    1. 确定左边界, 右边界
    for (gmin = 0; gmin < 256; gmin++)
    {
        if (pHistogram[gmin]) break;
    }
    for (gmax = 255; gmax >=0; gmax--)
    {
        if (pHistogram[gmax]) break;
    }
    if (gmax <= gmin) return false;
//    2. 初始化聚类中心
    center[0] = gmin;
    step = (gmax - gmin) / (nClass - 1);
    for (i = 1; i < nClass; i++)
    {
        center[i] = center[i - 1] + step;
    }
//    3. 迭代聚类
    while(1)
    {
#ifdef MY_DEBUG
        for (i = 0; i < nClass; i++)
        {
            printf("%.3lf  ", center[i]);
        }
        printf("\n");
#endif
        for (i = 0; i < nClass; i++)
        {
            sum[i] = num[i] = 0;
            centerPre[i] = center[i];
        }
//        3.1 分配给最近的聚类中心
        for (i = 0; i < 256; i++)
        {
            if (pHistogram[i])
            {
                minDist = 256;
                type = 0;
                for (j = 0; j < nClass; j++)
                {
                    dist = fabs(center[j] - i);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        type = j;
                    }
                }
                num[type] += pHistogram[i];
                sum[type] += pHistogram[i] * i;
            }
        }
//        3.2 重新计算聚类中心
        flag = true;
        for (i = 0; i < nClass; i++)
        {
            center[i] = sum[i] / num[i];
            if (fabs(center[i] - centerPre[i]) > decision)
            {
                flag = false;
            }
        }
        if (flag) return true;
    }
}

void getGaussianFilter(double std, int *pGaussian)
{
    int halfSize = lround(3 * std);  // 上取整
    double val = 0.0;
    double sum = (1 / (sqrt(2 * PI) * std));
    double temp = sum;
    for (int i = 1; i <= halfSize; i++)
    {
        val = temp * exp((-(i * i)) / (2 * std * std));
        sum += 2 * val;
    }
    for (int i = 0; i <= halfSize; i++)
    {
        val = temp * exp((-(i * i)) / (2 * std * std));
        pGaussian[halfSize - i] = pGaussian[halfSize + i] = int(val / sum * 1024);
    }
}

void gaussianFilter1D(BYTE *pImg, int width, int height, int *pGaussian, int m, BYTE *pResImg)
{
    // 横向高斯滤波
    BYTE *pCur, *pRes;
    int *pGau = pGaussian;
    int halfx = m / 2;
    int x, y, x1, x2, i;
    for (y = 0, pCur = pImg, pRes = pResImg; y < height; y++, pCur += width)
    {
        pRes += halfx;
        for (x = halfx, x1 = 0, x2 = m; x < width - halfx; x++, x1++, x2++, pRes++)
        {
            pGau = pGaussian;
            for (i = x1; i < x2; i++, pGau++)
            {
                *pRes += (*(pCur + i) * (*pGau)) >> 10;
            }
        }
        pRes += halfx;
    }
}

void transposeImg(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pRes;
    int x, y;
    for (y = 0, pCur = pImg; y < height; y++)
    {
        pRes = pResImg + y;
        for (x = 0; x < width; x++, pCur++, pRes += height)
        {
            *pRes = *pCur;
        }
    }
}

void gaussianFilter2D(BYTE *pImg, int width, int height, double std, BYTE *pResImg)
{
    int gaussianSize = 2 * lround(3 * std) + 1;
    int *pGaussian = new int[gaussianSize];
    getGaussianFilter(std, pGaussian);
    gaussianFilter1D(pImg, width, height, pGaussian, gaussianSize, pResImg);
    transposeImg(pResImg, width, height, pImg);
    memset(pResImg, 0, width * height * sizeof(BYTE));
    gaussianFilter1D(pImg, height, width, pGaussian, gaussianSize, pResImg);
    transposeImg(pResImg, height, width, pImg);
    memcpy(pResImg, pImg, width * height);
}



