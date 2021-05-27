//
// Created by 16337 on 2021/5/25 0025.
//
#include "bmpFile.h"


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
    fseek(fp, FileHeader.bfOffBits, SEEK_SET);
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
    fseek(fp, FileHeader.bfOffBits, SEEK_SET); //bfOffBytes
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

void rgb2gry1(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pEnd = pImg + width * height * 3;
    BYTE *pRes = pResImg;
    for (pCur = pImg; pCur < pEnd; pCur += 3, pRes++)
    {
        *pRes = (*pCur) * 0.299 + (*(pCur + 1)) * 0.587 + (*(pCur + 2)) * 0.114;
    }
}

void rgb2gry2(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pEnd = pImg + width * height * 3;
    BYTE *pRes = pResImg;
    int r = int(0.299 * 1024), g = int(0.587 * 1024), b = int(0.114 * 1024);
    for (pCur = pImg; pCur < pEnd; pCur += 3, pRes++)
    {
        *pRes = ((*pCur) * r + (*(pCur + 1)) * g + *(pCur + 2) * b) >> 10;
    }
}

void meanFilterBySumCol(BYTE *pImg, int width, int height, int m, int n, BYTE *pResImg)
{
    BYTE *pAdd, *pDel, *pRes;
    int halfx, halfy;
    int x, y;
    int sum, c;
    int sumCol[4096];
    memset(sumCol, 0, sizeof(int) * width);
    m = m / 2 * 2 + 1;
    n = n / 2 * 2 + 1;
    halfx = m / 2;
    halfy = n / 2;
    c = (1 << 23) / (m * n);
    for (y = 0, pAdd = pImg; y < n; y++)
    {
        for (x = 0; x < width; x++, pAdd++)
        {
            sumCol[x] += *pAdd;
        }
    }
    for (y = halfy, pRes = pResImg + y * width, pDel = pImg; y < height - halfy; y++)
    {

        for (sum = 0, x = 0; x < m; x++)
        {
            sum += sumCol[x];
        }
        pRes += halfx;
        for (x = halfx; x < width - halfx; x++, pRes++)
        {
            *pRes = (sum * c) >> 23;
            sum -= sumCol[x - halfx];
            sum += sumCol[x + halfx + 1];
        }
        pRes += halfx;
        for (x = 0; x < width; x++, pDel++, pAdd++)
        {
            sumCol[x] -= *pDel;
            sumCol[x] += *pAdd;
        }
    }
}

void meanFilterBySumCol(BYTE *pImg, int width, int height, int m, BYTE *pResImg)
{
    // 滤波邻域 m列1行
    BYTE *pRes, *pNew;
    int halfx, x, y, sum, c;
    int sumCol[4096];   // 1维
    memset(sumCol, 0, sizeof(int) * width);
    m = m / 2 * 2 + 1;
    halfx = m / 2;
    c = (1 << 23) / m;
    for (x = 0, pNew = pImg; x < width; x++, pNew++)
    {
        sumCol[x] = *pNew;
    }
    for (y = 0, pRes = pResImg; y < height; y++)
    {
        for (sum = 0, x = 0; x < m ; x++)
        {
            sum += sumCol[x];
        }
        pRes += halfx;
        for (x = halfx; x < width - halfx; x++, pRes++)
        {
            *pRes = (sum * c) >> 23;
            sum -= sumCol[x - halfx];
            sum += sumCol[x + halfx + 1];
        }
        pRes += halfx;
        for (x = 0; x < width; x++, pNew++)
        {
            sumCol[x] = *pNew;
        }
    }
}

void get2DIntegral(BYTE *pImg, int width, int height, int *pIntegral)
{
    BYTE *pCur;
    int *pRes;
    int x, y;
    int sumCol[4096];
    memset(sumCol, 0, sizeof(int) * width);
    for (y = 0, pCur = pImg, pRes = pIntegral; y < height; y++)
    {
        sumCol[0] += *(pCur++);
        *(pRes++) = sumCol[0];
        for (x = 1; x < width; x++, pCur++, pRes++)
        {
            sumCol[x] += *(pCur);
            *pRes = *(pRes - 1) + sumCol[x];
        }
    }
}

void get1DIntegral(BYTE *pImg, int width, int height, int *pIntegral)
{
    BYTE *pCur;
    int *pRes;
    int x, y;
    int sumCol[4096];
    memset(sumCol, 0, sizeof(int) * width);
    for (y = 0, pCur = pImg, pRes = pIntegral; y < height; y++)
    {
        sumCol[0] = *(pCur++);
        *(pRes++) = sumCol[0];
        for (x = 1; x < width; x++, pCur++, pRes++)
        {
            sumCol[x] = *(pCur);
            *pRes = *(pRes - 1) + sumCol[x];
        }
    }
}

void meanFilterBy2DIntegral(int *pIntegral, int width, int height, int m, int n, BYTE *pResImg)
{   //没有对边界上邻域不完整的像素进行处理，可以采用变窗口的策略
    BYTE *pRes;
    int *pY1, *pY2;
    int halfx, halfy, x, y, x1, x2, sum, c;
    m = m / 2 * 2 + 1;
    n = n / 2 * 2 + 1;
    halfx = m / 2;
    halfy = n / 2;
    c = (1 << 23) / (m * n);
    for (y = halfy + 1, pRes = pResImg + y * width, pY1 = pIntegral, pY2 = pIntegral + n * width;
         y < height - halfy;
         y++, pY1 += width, pY2 += width)
    {
        pRes += halfx + 1;
        for (x = halfx + 1, x1 = 0, x2 = m; x < width - halfx; x++, x1++, x2++, pRes++)
        {
            sum = *(pY2 + x2) - *(pY2 + x1) - *(pY1 + x2) + *(pY1 + x1);
            *pRes = (sum * c) >> 23;
        }
        pRes += halfx;
    }
}

void meanFilterBy1DIntegral(int *pIntegral, int width, int height, int m, BYTE *pResImg)
{
    //没有对边界上邻域不完整的像素进行处理，可以采用变窗口的策略
    //m列1行
    BYTE *pRes;
    int *pY1;
    int x, y, x1, x2, halfx, sum, c;
    m = m / 2 * 2 + 1;
    halfx = m / 2;
    c = (1 << 23) / m;
    for (y = 0, pRes = pResImg, pY1 = pIntegral; y < height; y++, pY1 += width)
    {
        pRes += halfx + 1;
        for (x = halfx + 1, x1 = 0, x2 = m; x < width - halfx; x++, x1++, x2++, pRes++)
        {
            sum = *(pY1 + x2) - *(pY1 + x1);
            *pRes = (sum * c) >> 23;
        }
        pRes += halfx;
    }
}


double medianFilter(BYTE *pImg, int width, int height, int m, int n, BYTE *pResImg)
{   //是每行建一个直方图,没有进行相邻行直方图递推和图像边界变窗口等优化处理
    BYTE *pCur, *pRes;
    int halfx, halfy, x, y, g, y1;
    int histogram[256];
    int wSize;
    int num, med, v;
    int dbgCmpTimes = 0;
    m = m / 2 * 2 + 1;
    n = n / 2 * 2 + 1;
    halfx = m / 2;
    halfy = n / 2;
    wSize = (halfx * 2 + 1) * (halfy * 2 + 1);
    for (y = halfy, pRes = pResImg + y * width; y < height - halfy; y++)
    {
        memset(histogram, 0, sizeof(int) * 256);
        for (y1 = y - halfy, pCur = pImg + y1 * width; y1 <= y + halfy; y1++, pCur += width)
        {
            for (x = 0; x < halfx * 2 + 1; x++)
            {
                histogram[*(pCur + x)]++;
            }
        }
        num = 0;
        for (g = 0; g < 256; g++)
        {
            num += histogram[g];
            if (num * 2 > wSize)
            {
                med = g;
                break;
            }
        }
        pRes += halfx;
        for (x = halfx; x < width - halfx; x++)
        {
            *(pRes++) = med;
            for (y1 = y - halfy, pCur = pImg + y1 * width; y1 <= y + halfy; y1++, pCur += width)
            {
                v = *(pCur + x - halfx);
                histogram[v]--;
                if (v <= med) num--;
                v = *(pCur + x + halfx + 1);
                histogram[v]++;
                if (v <= med) num++;
            }
            if (num * 2 < wSize)
            {
                for (med = med + 1; med < 256; med++)
                {
                    dbgCmpTimes += 2;
                    num += histogram[med];
                    if (num * 2 > wSize) break;
                }
                dbgCmpTimes += 1;
            }
            else
            {
                while ((num - histogram[med]) * 2 > wSize)
                {
                    dbgCmpTimes++;
                    num -= histogram[med];
                    med--;
                }
                dbgCmpTimes += 2;
            }
        }
        pRes += halfx;
    }
    return dbgCmpTimes * 1.0 / ((width - halfx * 2) * (height - halfy * 2));
}

void invertImgSSE(BYTE *pImg, int width, int height)
{
    BYTE * pCur, *pEnd = pImg + width * height;
    for (pCur = pImg; pCur < pEnd; pCur += 16)
    {
        __m128i src, dst;
        src = _mm_loadu_si128((__m128i *)pCur);
        dst = _mm_andnot_si128(src, _mm_set1_epi8(255));
        _mm_storeu_si128((__m128i *)(pCur), dst);
    }
}

void getGaussianFilter(double std, int *pGaussian)
{
    const double PI = 3.1415926;
    int halfSize = lround(3 * std);  // 上取整
    double val = 0.0;
    for (int i = 0; i <= halfSize; i++)
    {
        val = (1 / (sqrt(2 * PI) * std)) * exp((-(i*i)) / (2 * std * std));
        pGaussian[halfSize - i] = pGaussian[halfSize + i] = int(val * 1024);
    }
}

void gaussianFilter(BYTE *pImg, int width, int height, int *pGaussian, int m, BYTE *pResImg)
{
    // 横向高斯滤波
    BYTE *pCur, *pRes;
    int halfx = m / 2;
    int x, y, x1, x2, i;
    for (y = 0, pCur = pImg, pRes = pResImg; y < height; y++, pCur += width)
    {
        pRes += halfx;
        for (x = halfx, x1 = 0, x2 = m; x < width - halfx; x++, x1++, x2++, pRes++)
        {
            for (i = x1; i < x2; i++)
            {
                *pRes += (*(pCur + i) * pGaussian[i - x1]) >> 10;
            }
        }
        pRes += halfx;
    }
}

void transposeImg(BYTE *pImg, int width, int height, BYTE *pResImg)
{
    BYTE *pCur, *pEnd = pImg + width * height, *pRes;
    int x, x1;
    for (pCur = pImg, pRes = pResImg, x = 0, x1 = 0; pCur < pEnd; pCur++, x1++)
    {
        if (x1 == width)
        {
            x++;
            pRes = pResImg + x;
            x1 = 0;
        }
        *pRes = *pCur;
        pRes += width;
    }
}

