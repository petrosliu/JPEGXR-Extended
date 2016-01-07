//*******************************************************************
// File: image/sys/strRateControl.c
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************

#include "strRateControl.h"

const int LOOKUP[255] = {
	1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256, 272, 288, 304, 320, 336, 352, 368, 384, 400, 416, 432, 448, 464, 480, 496, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048, 2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 4096, 4352, 4608, 4864, 5120, 5376, 5632, 5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936, 8192, 8704, 9216, 9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848, 15360, 15872, 16384, 17408, 18432, 19456, 20480, 21504, 22528, 23552, 24576, 25600, 26624, 27648, 28672, 29696, 30720, 31744, 32768, 34816, 36864, 38912, 40960, 43008, 45056, 47104, 49152, 51200, 53248, 55295, 57343, 59391, 61439, 63487, 65535, 69631, 73727, 77823, 81919, 86015, 90111, 94206, 98302, 102398, 106494, 110590, 114686, 118782, 122877, 126973
};

//*******************************************************************
// Private Functions
//*******************************************************************
float fitLinearModel(int bits, int index){
	float a,b;
	// switch(bits){
	// 	case 8:
	// 		a=2.6401;
	// 		b=0;
	// 		c=-2.7445;
	// 		break;
	// 	case 16:
	// 		a=330.9;
	// 		b=0;
	// 		c=-1088.5;
	// 		break;
	// 	case 32:
	// 		a=8516;
	// 		b=0;
	// 		c=-42382;
	// 		break;
	// 	default:
	// 		a=2950;
	// 		b=0;
	// 		c=-14491;
	// 		break;
	// }
	switch(bits){
		case 8:
			a=2.8978;
			b=-3.0286;
			break;
		case 16:
			a=364.8;
			b=-1182.4;
			break;
		case 32:
			a=9542;
			b=-47647;
			break;
		default:
			a=3303;
			b=-1627.7;
			break;
	}
	if(index=='a') return a;
	return b;
}

float fitLinear(QPCRList* list){
	float qp;

	if(list->numOfNodes>=2){
		qp=lookupQP(list->fit[0]*(float)list->crt+list->fit[1]);
		qp=(qp>255)?255:((qp<1)?1:qp);
		if(qp>list->range[1]||qp<list->range[0]){
			qp=((float)list->range[1]-(float)list->range[0])*0.5+(float)list->range[0];
		}
		if(list->curr->cr==list->last->cr && list->curr->cr>list->crt){
			qp=1;
		}
	}else{	
		float a,b;
		float crt=list->crt;
		a = fitLinearModel(list->bits,'a');
		b = fitLinearModel(list->bits,'b');
		
		if(list->numOfNodes){
			QPCRNode* head=list->head;
			if(head->cr>crt){
			qp=lookupQP(
				(float)lookupSF(head->qp)/(head->cr+ b / a)*crt + (float)lookupSF(head->qp)/(head->cr+b / a) * b / a
				);
			}
			else{
			qp=lookupQP(
				a*(crt-head->cr)+(float)lookupSF(head->qp)
				);
			}
		}else{
			qp = lookupQP(a*crt + b);
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
			fit[0]=((float)lookupSF(curr->qp)-(float)lookupSF(last->qp))/(curr->cr-last->cr);
			fit[1]=(float)lookupSF(last->qp)-last->cr*fit[0];
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
		while(b-a>1 && LOOKUP[a]!=LOOKUP[b]){
			if(sf==LOOKUP[(a+b)/2]) return (a+b)/2+1;
			if(sf<LOOKUP[(a+b)/2]) b=(a+b)/2;
			else a=(a+b)/2;
		}
		return b;
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
			list->fit[0] = fitLinearModel(list->bits,'a');
			list->fit[1] = fitLinearModel(list->bits,'b');
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

void resetBitCounter(CWMImageStrCodec * pSC){
	int k;
	for (k = 0; k < pSC->cNumBitIO; k++) {
		pSC->m_ppBitIO[k]->cBitsCounter=0;
	}
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
			printf("============================================================================\n");
			printf("index\tqp\tsf\tcr\tsize\tindex\tqp\tsf\tcr\tsize\n");
			int i=1;
			while(p != NULL){
				q=getQPCRNodeByIndex(list,i);
				i++;
				printf("%d\t%d\t%d\t%.2f\t%.0f\t", q->index, q->qp,lookupSF(q->qp), q->cr, (float) list->imageSize * (float) list->bits / q->cr / 8 + 0.5);
				printf("%d\t%d\t%d\t%.2f\t%.0f\n", p->index, p->qp, lookupSF(p->qp), p->cr, (float) list->imageSize * (float) list->bits / p->cr / 8 + 0.5);
				p = p->next;
			}
			if(!evaluateQPCRList(list)) printf("ALERT: APPROACH IS INEFFICIENT!\n");
		}
		printf("============================================================================\n");
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