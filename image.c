#include <stdio.h>
#include <stdlib.h>


//將 unsigned 的 char、short、int重新取名
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;

//BMP格式的Header
#pragma pack(1)
typedef struct{
    char CharB;
    char CharM;
    UINT32 Size;
    UINT32 Reserved;
    UINT32 ImageOffset;

    UINT32 HeaderSize;
    UINT32 PixelWidth;
    UINT32 PixelHight;
    UINT16 Planes;
    UINT16 BitPerPixel;
    UINT32 CompressionType;
    UINT32 ImageSize;
    UINT32 XPixelPerMeter;
    UINT32 YPixelPerMeter;
    UINT32 NumberOfColor;
    UINT32 ImportantColor;

}Header;

typedef struct{
    UINT8 R,G,B;

}Pixel;

typedef struct{
    Header *ImageHeader;
    UINT8 header[14];
    UINT8 headerInfo[40];
    UINT8 *data;

}IP1Image;
#pragma pack()

IP1Image *LoadImage(char *filename){
    IP1Image *src=(IP1Image *)malloc(sizeof(IP1Image));
    FILE*file=fopen(filename,"rb");
    UINT8 *ImageHeaderBuffer=(UINT8*)malloc(sizeof(UINT8)*54);
    fread(ImageHeaderBuffer,sizeof(UINT8),sizeof(UINT8)*54,file);
    src->ImageHeader=(Header *)ImageHeaderBuffer;
    memcpy(src->header,ImageHeaderBuffer,sizeof(src->header));
    memcpy(src->headerInfo,ImageHeaderBuffer+14,sizeof(src->headerInfo));

    src->data=malloc(sizeof(UINT8)*src->ImageHeader->PixelWidth*src->ImageHeader->PixelHight*3);//*3 是因為 RGB
    fseek(file,src->ImageHeader->ImageOffset,SEEK_SET);
    fread(src->data,sizeof(UINT8),sizeof(UINT8)*src->ImageHeader->PixelWidth*src->ImageHeader->PixelHight*3,file);
    fclose(file);
    return src;

}

void size_express(IP1Image *src){
    float size=src->ImageHeader->Size;
    if(size>1000){
        //KB
        size=size/1000;
        printf("File Size = %.3f KB\n",size);
    }
    else if(size>10000){
        //MB
        size=size/10000;
        printf("File Size = %.3f MB\n",size);
    }
    else{
        //GB
        size=size/100000;
        printf("File Size = %.3f GB\n",size);
    }
}

void ShowImage(IP1Image* src){
    printf("=================Header===================\n");
    printf("Signature = %c\n",src->ImageHeader->CharB);
    printf("Signature = %c\n",src->ImageHeader->CharM);
    //printf("File Size = %d\n",src->ImageHeader->Size);
    size_express(src);
    printf("Offset = %d\n",src->ImageHeader->ImageOffset);

    printf("=================Info===================\n");
    printf("size = %d\n",src->ImageHeader->HeaderSize);
    printf("hight = %d\n",src->ImageHeader->PixelHight);
    printf("width = %d\n",src->ImageHeader->PixelWidth);
    printf("Planes = %d\n",src->ImageHeader->Planes);
    printf("BitsPerPixel = %d\n",src->ImageHeader->BitPerPixel);
    printf("Compression = %d\n",src->ImageHeader->CompressionType);
    printf("ImageSize = %ld\n",src->ImageHeader->ImageSize);
    printf("XpixelsPerM = %ld\n",src->ImageHeader->XPixelPerMeter);
    printf("YPixelsPerM = %ld\n",src->ImageHeader->YPixelPerMeter);
    printf("ColorsUsed = %ld\n",src->ImageHeader->NumberOfColor);
    printf("ColorsImportant = %ld\n",src->ImageHeader->ImportantColor);

}


void setPixel(IP1Image *src,int x,int y){
    Pixel *pixel;
    int index = y+x*src->ImageHeader->PixelWidth*3;
    pixel=(Pixel*)&src->data[index];

    UINT8 gray = pixel->R*0.3+pixel->G*0.6+pixel->B*0.11;
    pixel->R=gray;
    pixel->G=gray;
    pixel->B=gray;
}

void SetImage(IP1Image *src){

    for(int i=0 ;i<src->ImageHeader->PixelHight;i++){
        for(int j=0;j<src->ImageHeader->PixelWidth*3;j++){
                setPixel(src,i,j);
        }
    }

}



void SaveImage(IP1Image *src,char *filename){
    FILE *file = fopen(filename,"wb");
    fwrite(src->header,sizeof(UINT8),sizeof(src->header),file);
    fwrite(src->headerInfo,sizeof(UINT8),sizeof(src->headerInfo),file);
    fseek(file,src->ImageHeader->ImageOffset,SEEK_SET);
    fwrite(src->data,sizeof(UINT8),sizeof(UINT8)*src->ImageHeader->PixelWidth*src->ImageHeader->PixelHight*3,file);
    fclose(file);

}

void ReleaseImage(IP1Image **src){

    if(src!=NULL && *src!=NULL){
        free((*src)->ImageHeader);
        free((*src)->data);
        free(*src);
    }
    *src=NULL;

}


int main()
{
    //程式流程：1.載入圖片 2.顯示所載入的圖片及圖片資訊 3.調整圖片像素 4.儲存圖片 5.釋放所配置的記憶體
    IP1Image *src = LoadImage("Logo.bmp");
    ShowImage(src);
    SetImage(src);
    SaveImage(src,"ddd.bmp");
    ReleaseImage(&src);

}
