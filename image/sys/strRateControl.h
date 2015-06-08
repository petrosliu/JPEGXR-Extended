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
QPCRNode * addQPCRNode(int qpc, float crc, QPCRNode* head);
int isQPCRNodeinList(int qp, QPCRNode* head);
QPCRNode* findQPCRNodeinList(int qp, QPCRNode* head);
QPCRNode* searchQPCRNode(float crt, QPCRNode* curr, QPCRNode* head);
#endif