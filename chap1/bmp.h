#ifndef CHAP1_BMP_H
#define CHAP1_BMP_H

struct BmpFileHeader
{
//    文件信息描述
//    unsigned short bfType;  // 0x4d42 19778 表示其是.bmp文件
    unsigned int bfSize; // 文件大小
    unsigned short bfReserved1; //  保留
    unsigned short bfReserved2; //  保留
    unsigned int bfOffBits; //  文件头到像素数据的偏移
};

struct BmpInfoHeader
{
//    数据信息描述
    unsigned int biSize;    //  此结构体大小
    long biWidth;   //  图像宽
    long biHeight;  //  图像高
    unsigned short biPlanes;    //  bmp平面属, 为1
    unsigned short biBitCount;  //  一像素所占大小
    unsigned int biCompression; //  压缩类型
    unsigned int biSizeImage;   // 像素数据所占大小
    long biXPixelsPerMeter;   //    水平分辨率
    long biYPixelsPerMeter; //  垂直分辨率
    unsigned int biClrUsed; //  颜色表中的颜色索引数
    unsigned int biClrImportant;    //  对图像显示有重要影响的颜色索引数目
};

struct BmpRGBQuad
{
//    调色板
//    可以没有此信息
    unsigned char rgbRed;   //  该颜色的蓝色分量 0-255
    unsigned char rgbGreen; //  该颜色的绿色分量 0-255
    unsigned char rgbBlue;  //  该颜色的红色分量 0-255
    unsigned char rgbReserved;  //  保留, 必须为0
};

#endif //CHAP1_BMP_H
