//*******************************************************************
// File: jxrencoderdecoder/PSNR.c
// Author: YD Liu
// Created: September 8 2015
// Comment: snr main
//*******************************************************************

#include "snr.h"

void InvalidArgument(char* argv){
    fprintf(stderr, "A program to calculate SNR and PSNR between raw images. Especially, the program\n");
    fprintf(stderr, "supports floating image and calculating microblock-SNR seperately. The usage is:\n");
    fprintf(stderr, "%s -o [Original File] -c [Compressed File] -w [Width] -h [Height] -b [Bits] <Options>\n", argv);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-b:\t\t8|16|32|32f\n");
    fprintf(stderr, "\t-C:\t\tPSNR|SNR|RHO|SIGMA\n");
    fprintf(stderr, "\t-p:\t\tALL|MB\n");
    fprintf(stderr, "\tdefault:\tALL int=PSNR float=SNR\n");
    exit(1);
}


int main(int argc, char *argv[])
{
    int width=0;
    int height=0;
    int bits=32;
    int isFloat=0;
    int piece=PALL;
    int cout=-1;
    char* ofile=NULL;
    char* cfile=NULL;
    int i=1;
  	char c;
    while (i < argc && argv[i][0] == '-') {
		switch ((c = argv[i][1])) {
		default:
			i++;
			if (i == argc || argv[i][0] == '-'){
                InvalidArgument(argv[0]);
            }
            
			switch (c) {
                case 'p':
                    if(strcmp(argv[i],"ALL")==0) piece=PALL;
                    else if(strcmp(argv[i],"MB")==0) piece=PMB;
                    else{
                        InvalidArgument(argv[0]);
                    }
                    break;
                    
                case 'C':
                    if(strcmp(argv[i],"PSNR")==0) cout=CPSNR;
                    else if(strcmp(argv[i],"SNR")==0) cout=CSNR;
                    else if(strcmp(argv[i],"RHO")==0) cout=CRHO;
                    else if(strcmp(argv[i],"SIGMA")==0) cout=CSIGMA;
                    else{
                        InvalidArgument(argv[0]);
                    }
                    break;    
                
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
                        if(cout==-1)cout=CSNR;
                    }
                    else{
                        bits = atoi(argv[i]);
                        if(cout==-1)cout=CPSNR;
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
    
    switch(piece){
        case PMB:{
            double *mb_o=(double *)calloc(64, sizeof(double));
            double *mb_c=(double *)calloc(64, sizeof(double));
        }
        break;
        case PALL:
        default:
        {
            double ans=calculate_suite(image_o, image_c, width, height, bits, isFloat, cout);
            printf("%f ",ans);
        }
    }
    return 0;
}
