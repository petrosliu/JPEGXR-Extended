//*******************************************************************
// File: image/sys/snr.h
// Author: YD Liu
// Created: September 8 2015
// Comment: kit of snr
//*******************************************************************


#ifndef PSNR_YD
#define PSNR_YD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "strRateControl.h"

int quickQP(double PSNR, int bits);
double quickPSNR(int qp, int bits);
double calculate_PSNR(int* source_data, int* comparison_data, int count, int bits);

#endif