//*******************************************************************
// File: image/sys/snr.h
// Author: YD Liu
// Created: September 8 2015
// Comment: kit of snr
//*******************************************************************


#ifndef SNR_YD
#define SNR_YD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define CPSNR 0
#define CSNR 1
#define CRHO 2
#define CSIGMA 3

#define PALL 0
#define PMB 1

int quickQP(double PSNR, int bits);
double quickPSNR(int qp, int bits);
double calculate_SIGMA(double* source_data, int count);
double calculate_PSNR(double* source_data, double* comparison_data, int count, int bits);
double calculate_SNR(double* source_data, double* comparison_data, int count);
double calculate_RHO(double* source_data, double* comparison_data, int w, int h, int bits, int isFloat);
double calculate_suite(double* source_data, double* comparison_data, int w, int h, int bits, int isFloat, int method);
#endif