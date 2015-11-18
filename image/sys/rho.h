//*******************************************************************
// File: image/sys/rho.h
// Author: YD Liu
// Created: September 21 2015
// Comment: kit of rho domain
//*******************************************************************


#ifndef RHO_YD
#define RHO_YD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "strRateControl.h"
#include "strAdaptiveQP.h"
#include "snr.h"

double getQnz(int* image, int mb, int dcqp, int lpqp, int hpqp);
double getQz(int* image, int mb, int dcqp, int lpqp, int hpqp);
double QP2RHO(int qp, int bits);
int RHO2QP(double rho, int bits);
double getRho(int* image, int mb, int dcqp, int lpqp, int hpqp);
double evaluatePatch(CWMImageStrCodec * pSC, int c, int w, int h, double crt);
int calculate_QPfromSNR(CWMImageStrCodec * pSC, double snr);
int calculate_QPfromPSNR(CWMImageStrCodec * pSC, double psnr);
#endif