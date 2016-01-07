//*******************************************************************
// File: image/sys/snr.c
// Author: YD Liu
// Created: September 8 2015
// Comment: kit of snr
//*******************************************************************

#include "snr.h"

int quickQP(double PSNR, int bits){
    double peak = pow(2.0,(double)bits) - 1.0;
    double MSE = pow(peak,2.0)/pow(10.0,PSNR/10.0);
    int sf = (int)(sqrt(MSE*12)+0.5);
    return lookupQP(sf);
}

double quickPSNR(int qp, int bits){
    int sf = lookupSF(qp);
    double MSE = pow((double)sf,2.0) / 12.0;
    double peak = pow(2.0,(double)bits) - 1.0;
    double PSNR = 20.0 * log10( peak / sqrt(MSE));
    return PSNR;
}

double calculate_PSNR(double* source_data, double* comparison_data, int count, int bits) {
    int i;
    double MSE = 0.0, PSNR = 0.0;
    double peak = pow(2.0,(double)bits) - 1.0;
    for(i = 0; i < count; i++) {
        MSE += pow((source_data[i] - comparison_data[i]), 2.0);
    }
    MSE /= (double)(count);
    
    PSNR = 20.0 * log10( peak / sqrt(MSE));
    return PSNR;
}

double calculate_SIGMA(double* source_data, int count){
    int i;
    double mean=0.0, sigma2=0.0, MSE = 0.0, SNR = 0.0;
    for(i = 0; i < count; i++){
		sigma2+=pow(source_data[i],2);
	}
	sigma2/=(double)(count);
    return pow(sigma2,0.5);
}

double calculate_SNR(double* source_data, double* comparison_data, int count){
    int i;
    double mean=0.0, sigma2=0.0, MSE = 0.0, SNR = 0.0;
    for(i = 0; i < count; i++){
		sigma2+=pow(source_data[i],2);
	}
	sigma2/=(double)(count);
    for(i = 0; i < count; i++) {
        MSE += pow((source_data[i] - comparison_data[i]), 2.0);
    }
    MSE /= (double)(count);
    
    SNR = 10.0 * log10( sigma2 / MSE);
    return SNR;
}
double calculate_RHO(double* source_data, double* comparison_data, int w, int h, int bits, int isFloat){
    int i,j,k,l,start,index,count,cnt,zcnt;
    double step=1;
    if(isFloat){
        double max=0;
        for(i=0;i<w*h;i++)max=(fabs(source_data[i])>max)?fabs(source_data[i]):max;
        step=max/pow(2,bits-1);
    }
    
    count=0;
    for(i=0;i<h;i+=16){
        for(j=0;j<w;j+=16){
            start=i*w+j;
            cnt=0;
            zcnt=0;
            for(k=0;k<16;k++){
                for(l=0;l<16;l++){
                    if(i+k<h&&j+l<w){
                        index=start+k*w+l;
                        cnt++;
                        if(fabs(comparison_data[index])<step)zcnt++;
                        else break;
                    }
                }
                if(cnt!=zcnt)break;
            }
            if(cnt==zcnt)count+=zcnt;
        }
    }
    return (double)count/(double)w/(double)h;
}

double calculate_suite(double* source_data, double* comparison_data, int w, int h, int bits, int isFloat, int method){
    switch(method){
        case CPSNR:{
            return calculate_PSNR(source_data,comparison_data,w*h,bits);
        }
        case CSNR:{
            
            return calculate_SNR(source_data,comparison_data,w*h);
        }
        case CRHO:{
            return calculate_RHO(source_data,comparison_data, w, h, bits, isFloat);
        }
        break;
        case CSIGMA:{
            return calculate_SIGMA(source_data,w*h);
        }
        break;
        default:
            return 0;
    }
}