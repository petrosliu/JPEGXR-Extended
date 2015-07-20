//*******************************************************************
// File: image/sys/strRateControl.h
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************
#include "windowsmediaphoto.h"
#include "strcodec.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef INF
#define INF 0x3fffffff
#endif

#define ITEMAX 0

#ifndef RATECONTOL_YD
#define RATECONTOL_YD

//#define RATECONTROL_TEST_YD

#ifdef RATECONTROL_TEST_YD 
#define RATECONTROL_TEST_VERBOSE 0
#endif

#define BINSEARCH 0
#define FITLINEAR 1

typedef struct tagQPCRNode {
	int qp;
	float cr;
	struct tagQPCRNode* prev;
	struct tagQPCRNode* next;
	int index;
} QPCRNode;

typedef struct tagQPCRList {
	struct tagQPCRNode* head;
	struct tagQPCRNode* tail;
	struct tagQPCRNode* curr;
	struct tagQPCRNode* last;
	int numOfNodes;
	float crt;
	int imageSize;
	int bits;
	int fitMode;
	float* fit;
	float tol;
	int finalQP;
	int* pNumOfBits;
	int* range;
} QPCRList;

QPCRList* createQPCRList(int fitMode);

int isTargetReached(QPCRList* list);

int generateNextQP(QPCRList* list);

void updateList(QPCRList* list, int qptmp, float crtmp);

void freeQPCRList(QPCRList** plist);

#ifdef RATECONTROL_TEST_YD
void printQPCRList(QPCRList* list);
#endif

#endif