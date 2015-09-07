#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

double calculate_PSNR(int* source_data, int* comparison_data, int count, int bits) {
    int i;
    double MSE = 0.0, PSNR = 0.0;
    double peak = pow(2.0,(double)bits)-1.0;
    for(i = 0; i < count; i++) {
        MSE += pow((double)(source_data[i] - comparison_data[i]), 2.0);
    }

    MSE /= (double)(count);
    PSNR = 20.0 * log10( peak / sqrt(MSE));
    return PSNR;
}

int main(int argc, char *argv[])
{
    if(argc != 6 && argc !=7) {
        fprintf(stderr, "Usage: %s [Original Image] [Comparison Image] [Width] [Height] [Bits] [MBoutput]\n", argv[0]);
        return EOF;
    }
    
    int width=atoi(argv[3]);
    int height=atoi(argv[4]);
    int bits=atoi(argv[5]);
    int mb=(argc==7)?atoi(argv[6]):0;
    
    FILE *source_fp, *comparison_fp;
    if(bits!=8 && bits!=16 && bits!=32) return EOF;
        
    if((source_fp = fopen(argv[1], "rb")) == NULL) {
        perror(argv[1]); return EOF;
    }
    if((comparison_fp = fopen(argv[2], "rb")) == NULL) {
        perror(argv[2]); fclose(source_fp); return EOF;
    }
        
    int* image_o = (int*)malloc(width*height*sizeof(int));
    int* image_c = (int*)malloc(width*height*sizeof(int));
    
    {
        int temp;
        int i;
        for(i=0;i<width*height;i++){    
            fread(&temp, bits/8, 1, source_fp);
            image_o[i]=temp;
            fread(&temp, bits/8, 1, comparison_fp);
            image_c[i]=temp;
        }
    }
    
    fclose(source_fp); fclose(comparison_fp);     
    
    if(mb){
        int mbheight=(height-1)/16+1;
        int mbwidth=(width-1)/16+1;
        int i,j;
        for(i=0;i<mbheight;i++){
            for(j=0;j<mbwidth;j++){
                int hei=(i==mbheight-1)?height-i*16:16;
                int wid=(j==mbwidth-1)?width-j*16:16;
                int patch_o[16*16]={0.0};
                int patch_c[16*16]={0.0};
                int k,l,m=0;
                for(k=0;k<hei;k++){
                    for(l=0;l<wid;l++){
                        patch_o[m]=image_o[(i*16+k)*width+j*16+l];
                        patch_c[m]=image_c[(i*16+k)*width+j*16+l];
                        m++;
                    }
                }
                double PSNR=calculate_PSNR(patch_o,patch_c,wid*hei,bits);
                printf("%f\n",PSNR);
            }
        }
    }else{
        double PSNR=calculate_PSNR(image_o,image_c,width*height,bits);
        printf("%f\n",PSNR);
    }    
    return 0;
}
