//*******************************************************************
// File: image/sys/strRateControl.c
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************

#include "strRateControl.h"

//*******************************************************************
// Private Functions
//*******************************************************************
float fitLinear(QPCRList* list){
	float qp;
	float a,b,c,d;
	
	switch(list->bits){
		case 8:
			a=-4.4413;
			b=44.1726;
			c=-41.4936;
			d=-43;
			break;
		case 16:
			a=-15.8981;
			b=127.8816;
			c=-79.9908;
			d=-53;
			break;
		case 32:
			a=-34.9914;
			b=257.2141;
			c=-215.7579;
			d=-60;
			break;
		default:
			a=-18.4436;
			b=143.0894;
			c=-112.4141;
			d=-150;
			break;
	}
	if(list->numOfNodes==0){
		qp=a*list->crt+b*sqrt(list->crt)+c+d;
	}else if(list->numOfNodes==1){
		qp=a*list->crt+b*sqrt(list->crt)+c+(float)(list->head->qp)-(a*(float)(list->head->cr)+b*sqrt((float)(list->head->cr))+c);
	}else{
		qp=list->fit[0]*(float)list->crt+list->fit[1];
		qp=(qp>255)?255:((qp<1)?1:qp);
		const int thsh=30;
		const float weight=0.2;
		if(qp>list->range[1]||qp<list->range[0]){
			qp=((float)list->range[1]-(float)list->range[0])*weight+(float)list->range[0];
		}
		else if(qp-list->curr->qp>thsh&&qp-list->last->qp>thsh){
			if(list->range[1]-list->range[0]>thsh){
				float max=((float)list->range[1]-(float)list->range[0])*weight+(float)list->range[0];
				float pct=(qp-(float)list->range[0])/(max-(float)list->range[0]);
				pct=(pct>1)?1:pct;
				qp=((2-4*weight)*pct*pct+(4*weight-1)*pct)*(max-(float)list->range[0])+(float)list->range[0];
			}
			else{
				float pct=(qp-(float)list->range[0])/((float)list->range[1]-(float)list->range[0]);
				qp=((2-4*weight)*pct*pct+(4*weight-1)*pct)*((float)list->range[1]-(float)list->range[0])+(float)list->range[0];
			}
		}
		else if(qp<list->curr->qp&&qp>list->last->qp||qp<list->last->qp&&qp>list->curr->qp){
			if(list->range[1]-list->range[0]>thsh){
				float pct=(qp-(float)list->range[0])/((float)list->range[1]-(float)list->range[0]);
				qp=(1-(2-4*weight)*pct*pct-(4*weight-1)*pct)*((float)list->range[1]-(float)list->range[0])+(float)list->range[0];	
			}
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

int isTargetReached(QPCRList* list){
	if(list->numOfNodes==0) return 0;
	QPCRNode* p = locateCRT(list);
	
	if(p==NULL){
		if(list->tail->qp==255){
			list->finalQP=255;
			list->tail=p;
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
	if(p->prev!=NULL && p->prev->cr > list->crt - list->tol/2 && p->prev->cr <= list->crt){
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

//*******************************************************************
// Test functions
//*******************************************************************
#ifdef RATECONTROL_TEST_YD 
int evaluateQPCRList(QPCRList* list){
	QPCRNode* h=getQPCRNodeByIndex(list,list->numOfNodes);
	QPCRNode* p=h;
	while(p!=list->head){
		p=p->prev;
		if(p->index>=p->next->index) return 0;
	}
	p=h;
	while(p!=list->tail){
		p=p->next;
		if(p->index>=p->prev->index) return 0;
	}
	return 1;
}

void printQPCRList(QPCRList* list){
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
		if(!evaluateQPCRList(list)) printf("ALERT: APPROACH IS NOT EFFICIENT!\n");
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
			printf("ERROR %d",list->fitMode);
	}
	if(ITEMAX) printf("itemax\t%d\t",ITEMAX);
	else printf("itemax\tNA\t");
	printf("ite\t%d\n",list->numOfNodes);
	printf("qp\tcur\t%d\tfin\t%d\n",curr->qp, list->finalQP);
	printf("cr\tcur\t%.2f\ttar\t%.2f\ttol\t%.2f\n",curr->cr, list->crt, list->tol);
	printf("size\tcur\t%.0f\ttar\t%.0f\traw\t%.0f\n",
			(float) (*(list->pNumOfBits)) / 8,
			(float) list->imageSize * (float) list->bits / list->crt / 8,
			(float) list->imageSize * (float) list->bits / 8);
	printf("================================================================\n");
}
#endif