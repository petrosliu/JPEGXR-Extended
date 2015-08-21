//*******************************************************************
// File: image/sys/strAdaptiveQP.h
// Author: YD Liu
// Created: Augest 1 2015
// Comment: kit of Adaptive QP
//*******************************************************************

#ifndef ADAPTIVEQP_YD
#define ADAPTIVEQP_YD

#include "windowsmediaphoto.h"
#include "strcodec.h"
#include "strTransform.h"
#include "strRateControl.h"
#include <math.h>
#include "perfTimer.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef MEM_TRACE
#define TRACE_MALLOC    1
#define TRACE_NEW       0
#define TRACE_HEAP      0
#include "memtrace.h"
#endif

typedef struct tagQPMatrix{
	float fltCRatio;
	int iMBWid;
	int iMBHei;
	int iLPNum;
	int iHPNum;
	int iDCQP;
	int* iLPQP;
	int* iHPQP;
	int* pLPQPMatrix;
	int* pHPQPMatrix;
} QPMatrix;


void printQPMatrix(QPMatrix * qpMatrix);
void defaultQPMatrix(QPMatrix * qpMatrix);
QPMatrix* createQPMatrix(CWMImageStrCodec * pSC);
void adpativeMBQP(CWMImageStrCodec * pSC);
#endif