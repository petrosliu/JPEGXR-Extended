//*******************************************************************
// File: jxrencoderdecoder/PSNR.c
// Author: YD Liu
// Created: September 8 2015
// Comment: snr main
//*******************************************************************

#include "snr.h"
#define PSNROUTPUT 0
#define MBOUTPUT 1
#define SIGMA2OUTPUT 2

void InvalidArgument(char* argv){
    fprintf(stderr, "A program to calculate SNR and PSNR between raw images. Especially, the program\n");
    fprintf(stderr, "supports floating image and calculating microblock-SNR seperately. The usage is:\n");
    fprintf(stderr, "%s -o [Original File] -c [Compressed File] -w [Width] -h [Height] -b [Bits] <Options>\n", argv);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-b:\t\t8|16|32|32f\n");
    fprintf(stderr, "\t-p:\t\tPSNR\n");
    fprintf(stderr, "\t-s:\t\tSNR\n");
    fprintf(stderr, "\t-m:\t\tSNR for MBs\n");
    fprintf(stderr, "\tdefault:\tint=PSNR float=SNR\n");
    exit(1);
}


int main(int argc, char *argv[])
{
    int width=0;
    int height=0;
    int bits=32;
    int isFloat=0;
    int option=-1;
    char* ofile=NULL;
    char* cfile=NULL;
    int i=1;
  	char c;
    while (i < argc && argv[i][0] == '-') {
		switch ((c = argv[i][1])) {
		case 'p':
            if(option==-1)option=PSNROUTPUT;
            else{
                InvalidArgument(argv[0]);
            }
			break;
		case 's':
            if(option==-1)option=SIGMA2OUTPUT;
            else{
                InvalidArgument(argv[0]);
            }
			break;
        case 'm':
            if(option==-1)option=MBOUTPUT;
            else{
                InvalidArgument(argv[0]);
            }
			break;
		default:
			i++;
			if (i == argc || argv[i][0] == '-'){
                InvalidArgument(argv[0]);
            }
			switch (c) {
			case 'o':
				ofile = argv[i];
				break;
  			case 'c':
				cfile = argv[i];
				break;
            case 'w':
				width = (unsigned int)atoi(argv[i]);
				break;
            case 'h':
				height = (unsigned int)atoi(argv[i]);
				break;
            case 'b':
               if(strcmp(argv[i],"32f")==0){
					bits = 32;
					isFloat = 1;
				}
				else{
					bits = atoi(argv[i]);
                }
				break;
			default:
				InvalidArgument(argv[0]);
			}
		}
		i++;
	}
    int size=width*height;
    if(size<=0||ofile==NULL||cfile==NULL){
        InvalidArgument(argv[0]);
    }
    double *image_o=(double *)calloc(size, sizeof(double));
    double *image_c=(double *)calloc(size, sizeof(double));
    {
        unsigned char *image1, *image2;
        unsigned short *image3, *image4;
        int32_t *image5, *image6;
        float *image7, *image8;
        FILE *source_fp=fopen(ofile,"r");
        FILE *comparison_fp=fopen(cfile,"r");
        
        if (isFloat){
            image7=(float *)calloc(size, sizeof(float));
            image8=(float *)calloc(size, sizeof(float));
            fread(image7,sizeof(float),size,source_fp);
            fread(image8,sizeof(float),size,comparison_fp);
            for(i=0;i<size;i++){
                image_o[i]=(double)image7[i];
                image_c[i]=(double)image8[i];
            }
        }
        else if (bits<9){
            image1=(char *)malloc(size);
            image2=(char *)malloc(size);
            fread(image1,sizeof(unsigned char),size,source_fp);
            fread(image2,sizeof(unsigned char),size,comparison_fp);
            for(i=0;i<size;i++){
                image_o[i]=(double)image1[i];
                image_c[i]=(double)image2[i];
            }
        }
        else if (bits<17){
            image3=(unsigned short *)calloc(size, sizeof(unsigned short));
            image4=(unsigned short *)calloc(size, sizeof(unsigned short));
            fread(image3,sizeof(unsigned short),size,source_fp);
            fread(image4,sizeof(unsigned short),size,comparison_fp);
            for(i=0;i<size;i++){
                image_o[i]=(double)image3[i];
                image_c[i]=(double)image4[i];
            }
        }
        else {
            image5=(int32_t *)calloc(size, sizeof(int32_t));
            image6=(int32_t *)calloc(size, sizeof(int32_t));
            fread(image5,sizeof(int32_t),size,source_fp);
            fread(image6,sizeof(int32_t),size,comparison_fp);
            for(i=0;i<size;i++){
                image_o[i]=(double)image5[i];
                image_c[i]=(double)image6[i];
            }
        }
        fclose(source_fp); fclose(comparison_fp);     
    }
    
    switch(option){
        case MBOUTPUT:{
            int mbheight=(height-1)/16+1;
            int mbwidth=(width-1)/16+1;
            int i,j;
            for(i=0;i<mbheight;i++){
                for(j=0;j<mbwidth;j++){
                    int hei=(i==mbheight-1)?height-i*16:16;
                    int wid=(j==mbwidth-1)?width-j*16:16;
                    double patch_o[16*16]={0.0};
                    double patch_c[16*16]={0.0};
                    int k,l,m=0;
                    for(k=0;k<hei;k++){
                        for(l=0;l<wid;l++){
                            patch_o[m]=image_o[(i*16+k)*width+j*16+l];
                            patch_c[m]=image_c[(i*16+k)*width+j*16+l];
                            m++;
                        }
                    }
                    if(isFloat){
                        double PSNR=calculate_PSNR(patch_o,patch_c,wid*hei,bits);
                        printf("%f\n",PSNR);
                    }else{
                        double SNR=calculate_SNR(patch_o,patch_c,wid*hei,bits);
                        printf("%f\n",SNR);
                    }
                }
            }
            break;
        }
        case SIGMA2OUTPUT:{
            double SNR=calculate_SNR(image_o,image_c,width*height,bits);
            printf("%f ",SNR);
            break;
        }
        default:
        {
            if(isFloat){
                double SNR=calculate_SNR(image_o,image_c,width*height,bits);
                printf("%f ",SNR);
            }else{
                double PSNR=calculate_PSNR(image_o,image_c,width*height,bits);
                printf("%f ",PSNR);
            }
        }
    }
    return 0;
}
