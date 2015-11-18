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

double calculate_SNR(double* source_data, double* comparison_data, int count, int bits){
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