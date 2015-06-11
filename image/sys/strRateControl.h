//*******************************************************************
// File: image/sys/strRateControl.h
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************

#ifndef NULL
#define NULL 0
#endif

#ifndef RATECONTOL_YD
#define RATECONTOL_YD

#include "windowsmediaphoto.h"
#include "strcodec.h"

typedef struct tagQPCRNode {
	int qp;
	float cr;
	struct tagQPCRNode* prev;
	struct tagQPCRNode* next;
} QPCRNode;

void printQPCRList(QPCRNode* curr,QPCRNode* head);

QPCRNode* createQPCRNode(int qp, float cr, QPCRNode* prev, QPCRNode* next);
QPCRNode* addQPCRNodeinList(int qpc, float crc, QPCRNode* head);

int isQPCRNodeinList(int qp, QPCRNode* head);
QPCRNode* findQPCRNodeinList(int qp, QPCRNode* head);
QPCRNode* searchQPCRNode(float crt, QPCRNode* curr, QPCRNode* head);

void freeQPCRList(QPCRNode** phead);

int countQPCRNode(QPCRNode* head);

int generateSecondQP(QPCRNode* head, float crt);
int generateNextQP(QPCRNode* curr, QPCRNode* last, float crt);
int generateFinalQP(QPCRNode* curr, float crt);
#endif