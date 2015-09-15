//*******************************************************************
// File: image/sys/strRateControl.c
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************

#include "strRateControl.h"

const int LOOKUP[255] = {
	0x2, 0x4, 0x6, 0x8, 0xA, 0xC, 0xE, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20,
	0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40,
    0x44, 0x48, 0x4C, 0x50, 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C, 0x70, 0x74, 0x78, 0x7C, 0x80,
	0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0x100,
	0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170, 0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0, 0x200,
	0x220, 0x240, 0x260, 0x280, 0x2A0, 0x2C0, 0x2E0, 0x300, 0x320, 0x340, 0x360, 0x380, 0x3A0, 0x3C0, 0x3E0, 0x400,
	0x440, 0x480, 0x4C0, 0x500, 0x540, 0x580, 0x5C0, 0x600, 0x640, 0x680, 0x6C0, 0x700, 0x740, 0x780, 0x7C0, 0x800,
	0x880, 0x900, 0x980, 0xA00, 0xA80, 0xB00, 0xB80, 0xC00, 0xC80, 0xD00, 0xD80, 0xE00, 0xE80, 0xF00, 0xF80, 0x1000,
	0x1100, 0x1200, 0x1300, 0x1400, 0x1500, 0x1600, 0x1700, 0x1800, 0x1900, 0x1A00, 0x1B00, 0x1C00, 0x1D00, 0x1E00, 0x1F00, 0x2000,
	0x2200, 0x2400, 0x2600, 0x2800, 0x2A00, 0x2C00, 0x2E00, 0x3000, 0x3200, 0x3400, 0x3600, 0x3800, 0x3A00, 0x3C00, 0x3E00, 0x4000,
	0x4400, 0x4800, 0x4C00, 0x5000, 0x5400, 0x5800, 0x5C00, 0x6000, 0x6400, 0x6800, 0x6C00, 0x7000, 0x7400, 0x7800, 0x7C00, 0x8000,
	0x8800, 0x9000, 0x9800, 0xA000, 0xA800, 0xB000, 0xB800, 0xC000, 0xC800, 0xD000, 0xD800, 0xE000, 0xE800, 0xF000, 0xF800, 0x10000,
	0x11000, 0x12000, 0x13000, 0x14000, 0x15000, 0x16000, 0x17000, 0x18000, 0x19000, 0x1A000, 0x1B000, 0x1C000, 0x1D000, 0x1E000, 0x1F000, 0x20000,
	0x22000, 0x24000, 0x26000, 0x28000, 0x2A000, 0x2C000, 0x2E000, 0x30000, 0x32000, 0x34000, 0x36000, 0x38000, 0x3A000, 0x3C000, 0x3E000, 0x40000,
	0x44000, 0x48000, 0x4C000, 0x50000, 0x54000, 0x58000, 0x5C000, 0x60000, 0x64000, 0x68000, 0x6C000, 0x70000, 0x74000, 0x78000, 0x7C000, 0x80000,
	0x88000, 0x90000, 0x98000, 0xA0000, 0xA8000, 0xB0000, 0xB8000, 0xC0000, 0xC8000, 0xD0000, 0xD8000, 0xE0000, 0xE8000, 0xF0000, 0xF8000
};

//*******************************************************************
// Private Functions
//*******************************************************************
float fitLinearModel(int bits, float crt, int index){
	crt=(crt>0)?crt:1;
	float a,b,c;
	switch(bits){
		case 8:
			a=2.6401;
			b=0;
			c=-2.7445;
			break;
		case 16:
			a=330.9;
			b=0;
			c=-1088.5;
			break;
		case 32:
			a=8516;
			b=0;
			c=-42382;
			break;
		default:
			a=2950;
			b=0;
			c=-14491;
			break;
	}
	// switch(bits){
	// 	case 8:
	// 		if(crt>24.7301) crt=24.7301;
	// 		a=-4.4413;
	// 		b=44.1726;
	// 		c=-84.4936;
	// 		break;
	// 	case 16:
	// 		if(crt>16.1765) crt=16.1765;
	// 		a=-15.8981;
	// 		b=127.8816;
	// 		c=-132.9908;
	// 		break;
	// 	case 32:
	// 		if(crt>13.5083) crt=13.5083;
	// 		a=-34.9914;
	// 		b=257.2141;
	// 		c=-275.7579;
	// 		break;
	// 	default:
	// 		if(crt>15.0475) crt=15.0475;
	// 		a=-18.4436;
	// 		b=143.0894;
	// 		c=-262.4141;
	// 		break;
	// }
	if(index=='a') return a;
	if(index=='b') return b;
	if(index=='c') return c;
	return crt;
}

float fitLinear(QPCRList* list){
	float qp;
	const int thsh=30;
	const float weight=0.2;
	
	if(list->numOfNodes>=2){
		qp=list->fit[0]*(float)list->crt+list->fit[1];
		qp=(qp>255)?255:((qp<1)?1:qp);
		if(qp>list->range[1]||qp<list->range[0]){
			qp=((float)list->range[1]-(float)list->range[0])*weight+(float)list->range[0];
		}
		else if(qp-list->curr->qp>thsh&&qp-list->last->qp>thsh){
			if(list->range[1]-list->range[0]>thsh){
				float max=((float)list->range[1]-(float)list->range[0])*weight+(float)list->range[0];
				float pct=(qp-(float)list->range[0])/(max-(float)list->range[0]);
				pct=((2-4*weight)*pct*pct+(4*weight-1)*pct);
				pct=(pct>1)?1:((pct<0)?0:pct);
				qp=pct*(max-(float)list->range[0])+(float)list->range[0];
			}
			else{
				float pct=(qp-(float)list->range[0])/((float)list->range[1]-(float)list->range[0]);
				pct=((2-4*weight)*pct*pct+(4*weight-1)*pct);
				pct=(pct>1)?1:((pct<0)?0:pct);
				qp=pct*((float)list->range[1]-(float)list->range[0])+(float)list->range[0];
			}
		}
		else if(qp<list->curr->qp&&qp>list->last->qp||qp<list->last->qp&&qp>list->curr->qp){
			if(list->range[1]-list->range[0]>thsh /*&& list->crt>=8.375*/){
				float pct=(qp-(float)list->range[0])/((float)list->range[1]-(float)list->range[0]);
				pct=(2*pct-(2-4*weight)*pct*pct-(4*weight-1)*pct);
				pct=(pct>1)?1:((pct<0)?0:pct);
				qp=pct*((float)list->range[1]-(float)list->range[0])+(float)list->range[0];	
			}
		}
	}else{	
		float a,b,c;
		float crt=list->crt;
		a = fitLinearModel(list->bits, crt, 'a');
		b = fitLinearModel(list->bits, crt, 'b');
		c = fitLinearModel(list->bits, crt, 'c');
		crt = fitLinearModel(list->bits, crt, 0);
		
		if(list->numOfNodes){
			QPCRNode* head=list->head;
			qp = a*crt + b*sqrt(crt) + (float)(head->qp)
			    -(a*(float)(head->cr) + b*sqrt((float)(head->cr)));
			qp=lookupQP(qp);
		}else{
			qp = a*crt + b*sqrt(crt) + c;
			qp=lookupQP(qp);
		}
	}
	return qp;
}

int generateFirstQP(QPCRList* list){
	float qp;
	int bits=list->bits;
	float crt=list->crt;
	switch(list->fitMode){
		case FITLINEAR:
			qp = fitLinear(list);
			break;
		case BINSEARCH:
		default:
			qp = 1;
	}
	return (int)(qp+0.5);
}

int generateSecondQP(QPCRList* list){
	float qp;
	int bits=list->bits;
	float crt=list->crt;
	switch(list->fitMode){
		case FITLINEAR:
			qp = fitLinear(list);
			break;
		case BINSEARCH:
		default:
			qp = 255;
	}
	return (int)(qp+0.5);
}

QPCRNode* createQPCRNode(QPCRList* list, int qp, float cr, QPCRNode* prev, QPCRNode* next){
	QPCRNode* p = (QPCRNode*) malloc(sizeof(QPCRNode));
	p->qp = qp;
	p->cr = cr;
	p->prev = prev;
	p->next = next;
	list->numOfNodes ++;
	p->index = list->numOfNodes;
	return p;
}

QPCRNode* addQPCRNodeinList(QPCRList* list, int qpc, float crc){
	QPCRNode* head=list->head;
	if(head == NULL) return createQPCRNode(list, qpc, crc, NULL, NULL);
	else if(head->qp > qpc){
		list->head = createQPCRNode(list, qpc, crc, NULL, head);
		head->prev = list->head;
		return list->head;
	}
	else if(head->qp == qpc) return head;
	else{
		QPCRNode* p = head;
		while(p->qp < qpc){
			if(p == list->tail){
				QPCRNode* nodec = createQPCRNode(list, qpc,crc,p,NULL);
				p->next = nodec;
				list->tail = nodec;
				return nodec;
			}
			p = p->next;
		}
		if(p->qp == qpc) return p;
		else{
			QPCRNode* nodec = createQPCRNode(list, qpc, crc, p->prev, p);
			p->prev->next = nodec;
			p->prev = nodec;
			return nodec;
		}
	}
}

void deleteQPCRNodeinList(QPCRList* list, QPCRNode* p){
	if(p==NULL) return;
	if(p->prev==NULL && p->next==NULL){
		list->head=NULL;
		list->tail=NULL;
	}
	else if(p->prev==NULL){
		list->head=p->next;
		p->next->prev=NULL;
	}
	else if(p->next==NULL){
		list->tail=p->prev;
		p->prev->next=NULL;
	}
	else{
		p->next->prev=p->prev;
		p->prev->next=p->next;
	}
	if(list->curr==p)list->curr=NULL;
	if(list->last==p)list->last=NULL;
	p->next=NULL;
	p->prev=NULL;
	list->numOfNodes--;
	free(p);
}

int isQPCRNodeinList(QPCRList* list, int qp){
	if(qp<1||qp>255||list->numOfNodes==0) return 0;
	QPCRNode* p = list->head;
	while(p != NULL){
		if(qp == p->qp) {
			list->curr=p;
			return 1;
		}
		p = p->next;
	}
	return 0;
}

QPCRNode* getQPCRNodeByIndex(QPCRList* list, int index){
//call this with index from 1 to numOfNodes!
	QPCRNode* p = list->head;
	while(p->index!=index){
		p=p->next;
		if(p==list->tail) break;
	}
	return p;
}

QPCRNode* getQPCRNodeByQP(QPCRList* list, int qp){
//call this after isQPCRNodeinList() true!
	QPCRNode* p = list->head;
	while(p->qp!=qp){
		p=p->next;
		if(p==list->tail) break;
	}
	return p;
}

QPCRNode* getLastQPCRNode(QPCRList* list){
	float crt=list->crt;
	QPCRNode* curr=list->curr;
	QPCRNode* p;
	
	switch(list->fitMode){
		case FITLINEAR:
			return getQPCRNodeByIndex(list,list->numOfNodes-1);
		case BINSEARCH:
		default:
			if(curr->cr>crt) return curr->prev;
			else return curr->next;
	}
}

void updateFit(QPCRList* list){
	QPCRNode* curr=list->curr;
	QPCRNode* last=list->last;
	float* fit=list->fit;
	switch(list->fitMode){
		case FITLINEAR:
			fit[0]=((float)curr->qp-(float)last->qp)/(curr->cr-last->cr);
			fit[1]=(float)last->qp-last->cr*fit[0];
			break;
		case BINSEARCH:
		default:
			break;
	}
}

QPCRNode* locateCRT(QPCRList* list){
//return the least node larger than or equal crt
	QPCRNode* p = list->head;
	if(p->cr>=list->crt) return p;
	while(p!=list->tail&&p->cr<list->crt){
		p=p->next;
	}
	if(p->cr>=list->crt) return p;
	else return NULL;
}

void updateRange(QPCRList* list){
	int* range=list->range;
	if(list->numOfNodes==0){
		range[0]=1;
		range[1]=255;
	}else{
		QPCRNode* p = locateCRT(list);
		if(p==NULL){
			range[0]=list->tail->qp;
			range[1]=255;
		}else if(p==list->head){
			range[0]=1;
			range[1]=p->qp;
		}else{
			range[0]=p->prev->qp;
			range[1]=p->qp;
		}
	}
}

//*******************************************************************
// Public Functions
//*******************************************************************
int lookupSF(int qp){
	qp=(qp>255)?255:(qp<1)?1:qp;
	return LOOKUP[qp-1];
}

int lookupQP(int sf){
	int qp;
	if(sf<=LOOKUP[0]) return 1;
	else if(sf>=LOOKUP[254]) return 255;
	else{
		int a=0,b=254;
		while(b-a>1){
			if(sf==LOOKUP[(a+b)/2]) return (a+b)/2+1;
			if(sf<LOOKUP[(a+b)/2]) b=(a+b)/2;
			else a=(a+b)/2;
		}
		if(abs(sf-LOOKUP[a])<=abs(sf-LOOKUP[b])) return a+1;
		else return b+1;
	}
}

int isTargetReached(QPCRList* list){
	if(list->numOfNodes==0) return 0;
	QPCRNode* p = locateCRT(list);
	
	if(p==NULL){
		if(list->tail->qp==255){
			list->finalQP=255;
			list->curr=list->tail;
			return 1;
		}else return 0;
	}
	if(p->qp==1){
		list->finalQP=1;
		list->curr=p;
		return 1;
	}
	if(p->cr < list->crt + list->tol && p->cr >= list->crt){
		list->finalQP=p->qp;
		list->curr=p;
		return 1;
	}
	if(p->prev!=NULL&&p->prev->qp+1==p->qp){
		list->finalQP=p->qp;
		list->curr=p;
		return 1;
	}
	if(p->prev!=NULL && p->prev->cr > list->crt - 0.25 && p->prev->cr <= list->crt){
		list->finalQP=p->prev->qp+1;
		list->curr=p->prev;
		return 1;
	}
	return 0;
}

int generateNextQP(QPCRList* list){
	QPCRNode* curr=list->curr;
	QPCRNode* last=list->last;
	float crt=list->crt;
	int nextQP;
	if (list->numOfNodes==0) nextQP = generateFirstQP(list);
	else if (list->numOfNodes==1) nextQP = generateSecondQP(list);
	else{	
		switch(list->fitMode){
			case FITLINEAR:
			{
				if(curr->cr==last->cr){
					if(curr->cr<crt){
						nextQP=(curr->qp>last->qp)?curr->qp+1:last->qp+1;
					}else{
						nextQP=(curr->qp<last->qp)?curr->qp-1:last->qp-1;
					}
				}
				else{
					nextQP=(int)(fitLinear(list)+0.5);
				}
				break;
			}
			case BINSEARCH:
			default:
				nextQP=(int)((curr->qp+last->qp+1)/2);
				break;
		}	
	}
	if(isQPCRNodeinList(list,nextQP)){
		QPCRNode* p = getQPCRNodeByQP(list,nextQP);
		int a;
		if(p->cr<crt) a=1;
		else a=-1;
		while(isQPCRNodeinList(list,nextQP)){
			nextQP=nextQP+a;
		}
	}
	return (nextQP>255)?255:((nextQP<1)?1:nextQP);
}

void updateList(QPCRList* list, int qptmp, float crtmp){
	if(list->numOfNodes==0){
		list->head = addQPCRNodeinList(list,qptmp,crtmp);
		list->tail = list->head;
		list->curr = list->head;
		list->last = NULL;
	}
	else{
		list->curr = addQPCRNodeinList(list,qptmp,crtmp);
		list->last = getLastQPCRNode(list);
		updateFit(list);
	}
	updateRange(list);
	if(  ITEMAX!=0
	  && list->numOfNodes==ITEMAX 
	  && list->fitMode!=BINSEARCH){
		list->fitMode=BINSEARCH;
		list->curr=locateCRT(list);
		list->last=list->curr->prev;
	}
}

QPCRList* createQPCRList(int fitMode){
	QPCRList* list=(QPCRList*)malloc(sizeof(QPCRList));
	list->head=NULL;
	list->tail=NULL;
	list->curr=NULL;
	list->last=NULL;
	list->crt=1;
	list->finalQP=1;
	list->imageSize=INF;
	list->bits=32;
	list->tol=0;
	list->numOfNodes=0;
	list->fitMode=fitMode;
	list->pNumOfBits=NULL;
	list->range=(int*)malloc(2*sizeof(int));
	list->range[0]=1;
	list->range[1]=255;
	switch(fitMode){
		case FITLINEAR:
			list->fit=(float*)malloc(2*sizeof(float));
			memset(list->fit,0,2);
			break;
		case BINSEARCH:
		default:
			list->fit=NULL;
			break;
	}
	return list;
}

void freeQPCRList(QPCRList** plist){
	if(plist==NULL || *plist==NULL) return;
	QPCRList* list=*plist;
	QPCRNode* p = list->head;
	if(p != NULL){
		while(list->numOfNodes){
			deleteQPCRNodeinList(list,list->head);
		}
	}
	list->pNumOfBits=NULL;
	if(list->fit!=NULL) free(list->fit);
	free(list->range);
	free(list);
}

int getBitCounter(CWMImageStrCodec * pSC){
	int cCurrBits = 0;
	int k;
	for (k = 0; k < pSC->cNumBitIO; k++) {
		cCurrBits += pSC->m_ppBitIO[k]->cBitsCounter;
	}
	return cCurrBits;
}

//*******************************************************************
// Test functions
//*******************************************************************
#ifdef RATECONTROL_TEST_YD 
int evaluateQPCRList(QPCRList* list){
	QPCRNode* h=getQPCRNodeByIndex(list,list->numOfNodes);
	QPCRNode* p=h;
	while(p!=list->head){
		p=p->prev;
		if(p->index>=p->next->index && p->index>2) return 0;
	}
	p=h;
	while(p!=list->tail){
		p=p->next;
		if(p->index>=p->prev->index && p->index>2) return 0;
	}
	return 1;
}

void printQPCRList(QPCRList* list){	
	if(RATECONTROL_TEST_VERBOSE){
		QPCRNode* curr=list->curr;
		QPCRNode* p = list->head;
		QPCRNode* q;
		if(list->numOfNodes!=0){
			printf("================================================================\n");
			printf("index\tqp\tcr\tsize\t|index\tqp\tcr\tsize\n");
			int i=1;
			while(p != NULL){
				q=getQPCRNodeByIndex(list,i);
				i++;
				printf("%d\t%d\t%.2f\t%.0f\t", q->index, q->qp, q->cr, (float) list->imageSize * (float) list->bits / q->cr / 8 + 0.5);
			
				printf("|%d\t%d\t%.2f\t%.0f", p->index, p->qp, p->cr, (float) list->imageSize * (float) list->bits / p->cr / 8 + 0.5);
				if(p == curr && curr != NULL) printf("<-\n");
				else printf("\n");
				p = p->next;
			}
			if(!evaluateQPCRList(list)) printf("ALERT: APPROACH IS INEFFICIENT!\n");
		}
		printf("================================================================\n");
		printf("mode\t");
		switch(list->fitMode){
			case FITLINEAR:
				printf("FITLINEAR\t");
				break;
			case BINSEARCH:
				printf("BINSEARCH\t");
				break;
			default:
				printf("ERROR %d\t",list->fitMode);
		}
		printf("bits %d\n",list->bits);
		printf("ite\t%d\t",list->numOfNodes);
		if(ITEMAX) printf("itemax\t%d\n",ITEMAX);
		else printf("itemax\tNA\n");
		printf("qp\tcur\t%d\tfin\t%d\n",curr->qp, list->finalQP);
		printf("cr\tcur\t%.2f\ttar\t%.2f\ttol\t%.2f\n",curr->cr, list->crt, list->tol);
		printf("size\tcur\t%.0f\ttar\t%.0f\traw\t%.0f\n",
				(float) (*(list->pNumOfBits)) / 8,
				(float) list->imageSize * (float) list->bits / list->crt / 8,
				(float) list->imageSize * (float) list->bits / 8);
		printf("================================================================\n");
	}else{		
		printf("%.2f\t%d\t%d\t%d\t%d\t%d\t%.0f\t%.0f",list->crt,list->finalQP,lookupSF(list->finalQP),list->imageSize,list->bits,list->numOfNodes,
			(float) list->imageSize * (float) list->bits / list->crt / 8,
			(float) (*(list->pNumOfBits)) / 8);
		if(!evaluateQPCRList(list)) printf("\tINEFF");
		printf("\n");
	}
}
#endif