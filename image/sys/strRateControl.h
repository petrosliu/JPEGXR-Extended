//*******************************************************************
// File: image/sys/strRateControl.h
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************

#ifndef NULL
#define NULL 0
#endif

#ifndef INF
#define INF 0x3fffffff
#endif

#ifndef RATECONTOL_YD
#define RATECONTOL_YD

#include "windowsmediaphoto.h"
#include "strcodec.h"

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
	struct tagQPCRNode* curr;
	struct tagQPCRNode* last;
	int numOfNodes;
	float crt;
	int imageSize;
	int bits;
	int fitMode;
	float* fit;
	float tol;
} QPCRList;

#define RATECONTROL_TEST_YD

#ifdef RATECONTROL_TEST_YD 
void printQPCRList(QPCRList* list);
int countQPCRNode(QPCRList* list);
#endif

int isTargetReached(QPCRList* list);

int isQPCRNodeinList(QPCRList* list, int qp);

void updateList(QPCRList* list, int qptmp, float crtmp);

void freeQPCRList(QPCRList** plist);

int generateNextQP(QPCRList* list);

int generateFinalQP(QPCRList* list);

QPCRList* createQPCRList(int fitMode);
#endif