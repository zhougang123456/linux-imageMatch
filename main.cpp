#include <iostream>
#include "ImageMatch.hpp"
#include <time.h>
#include <unistd.h>

typedef unsigned char  BYTE;
typedef unsigned short	WORD;
typedef unsigned int  DWORD;

#define PACKED __attribute__(( packed, aligned(2)))

typedef struct BITMAPFILEHEADER{
     WORD     bfType;        //Linux此值为固定值，0x4d42
     DWORD    bfSize;        //BMP文件的大小，包含三部分
     WORD     bfReserved1;    //置0
     WORD     bfReserved2;
     DWORD    bfOffBits;     //文件起始位置到图像像素数据的字节偏移量

}PACKED BITMAPFILEHEADER;


typedef struct BITMAPINFOHEADER{
     DWORD    biSize;          //文件信息头的大小，40
     DWORD    biWidth;         //图像宽度
     DWORD    biHeight;        //图像高度
     WORD     biPlanes;        //BMP存储RGB数据，总为1
     WORD     biBitCount;      //图像像素位数，笔者RGB位数使用24
     DWORD    biCompression;   //压缩 0：不压缩  1：RLE8 2：RLE4
     DWORD    biSizeImage;     //4字节对齐的图像数据大小
     DWORD    biXPelsPerMeter; //水平分辨率  像素/米
     DWORD    biYPelsPerMeter;  //垂直分辨率  像素/米
     DWORD    biClrUsed;        //实际使用的调色板索引数，0：使用所有的调色板索引
     DWORD    biClrImportant;
}BITMAPINFOHEADER;

static inline int get_time(void)
{
    timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    return (int)now.tv_sec * 1000 + (int)now.tv_nsec / 1000000;

}

unsigned char* readBmp()
{
    char file_str[200];
    static int bmp_id = 0;
    sprintf(file_str, "%u.bmp", ++bmp_id);
    FILE* fp;
    if ((fp = fopen(file_str, "rb")) == NULL)  //以二进制的方式打开文件
    {

        return NULL;
    }
    if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  //跳过BITMAPFILEHEADE
    {
        return NULL;
    }
    BITMAPINFOHEADER infoHead;
    fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   //从fp中读取BITMAPINFOHEADER信息到infoHead中,同时fp的指针移动
    int bmpwidth = infoHead.biWidth;

    int bmpheight = infoHead.biHeight;

    if (bmpheight < 0) {
        bmpheight *= -1;
    }
    int linebyte = bmpwidth * 4; //计算每行的字节数，24：该图片是24位的bmp图，3：确保不丢失像素

    unsigned char* pBmpBuf = new unsigned char[linebyte * bmpheight];

    fread(pBmpBuf, sizeof(char), linebyte * bmpheight, fp);
    fclose(fp);   //关闭文件
    return pBmpBuf;
}

int main()
{
    ImageMatch* match = new ImageMatch();
    while (true) {
        unsigned char* img1 = readBmp();
        unsigned char* img2 = readBmp();
        int start = (int)get_time() ;
        int vector = match->do_match((Pixel32Bit*)img1, (Pixel32Bit*)img2, 1920, 1080);
        int end = (int)get_time() ;
        printf("match vector is %d cost time %d\n", vector, end - start);
        sleep(2);
    }
}
