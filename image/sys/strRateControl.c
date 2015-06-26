//*******************************************************************
// File: image/sys/strRateControl.c
// Author: YD Liu
// Created: June 7 2015
// Comment: kit of QPCR linked list
//*******************************************************************

#include "strRateControl.h"

//*******************************************************************
// Test functions
//*******************************************************************

void printQPCRList(QPCRList* list){
	QPCRNode* curr=list->curr;
	QPCRNode* p = list->head;
	printf("qp\tcr\n");
	while(p != NULL){
		printf("%d\t%.2f\t%d", p->qp, p->cr,p->index);
		if(p == curr && curr != NULL) printf("<-\n");
		else printf("\n");
		p = p->next;
	}
	printf("EOL\n");
}

int countQPCRNode(QPCRList* list){
	if(list == NULL) return -1;
	return list->numOfNodes;
}

//*******************************************************************
// Private Functions
//*******************************************************************

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

int generateFirstQP(QPCRList* list){
	//future work
	return 5;
}

int generateSecondQP(QPCRList* list){
	if(list->head->cr > list->crt) return 1;
	return 100;
}

QPCRNode* addQPCRNodeinList(QPCRList* list, int qpc, float crc){
	QPCRNode* head=list->head;
	if(head == NULL) return createQPCRNode(list, qpc, crc, NULL, NULL);
	else if(head->cr > crc){
		list->head = createQPCRNode(list, qpc, crc, NULL, head);
		head->prev = list->head;
		return list->head;
	}
	else if(head->cr == crc) return head;
	else{
		QPCRNode* p = head;
		while(p->cr < crc){
			if(p->next == NULL){
				QPCRNode* nodec = createQPCRNode(list, qpc,crc,p,NULL);
				p->next = nodec;
				return nodec;
			}
			p = p->next;
		}
		if(p->cr == crc) return p;
		else{
			QPCRNode* nodec = createQPCRNode(list, qpc, crc, p->prev, p);
			p->prev->next = nodec;
			p->prev = nodec;
			return nodec;
		}
	}
}

QPCRNode* getLastQPCRNode(QPCRList* list){
	//return list->head;
	float crt=list->crt;
	QPCRNode* curr=list->curr;
	QPCRNode* head=list->head;
	QPCRNode* p;
	if(!isQPCRNodeinList(list,curr->qp)){
		p=addQPCRNodeinList(list,curr->qp,curr->cr);
		free(curr);
		list->numOfNodes--;
		curr = p;
	}
	p = curr;
	if(curr->cr < crt){
		while(p->cr < crt){
			if(p->next == NULL){
				if(p == curr) p = p->prev;
				break;
			}else{
				p = p->next;
			}
		}
	}else{
		while(p->cr > crt){
			if(p->prev == NULL){
				if(p == curr) p = p->next;
				break;
			}else{
				p = p->prev;
			}
		}
	}
	return p;
}

void updateFit(QPCRList* list){
	QPCRNode* curr=list->curr;
	QPCRNode* last=list->last;
	float* fit=list->fit;
	switch(list->fitMode){
		case FITLINEAR:
		default:	
		fit[0]=((float)curr->qp-(float)last->qp)/(curr->cr-last->cr);
		fit[1]=(float)last->qp-last->cr*fit[0];
	}
}

//*******************************************************************
// Public Functions
//*******************************************************************

int isTargetReached(QPCRList* list){
	if(list->numOfNodes==0) return 0;
	QPCRNode* curr = list->curr;
	float crt=list->crt;
	if(curr->qp==1) return(curr->cr-crt >= 0);
	if(curr->qp==255) return(curr->cr-crt < 0);
	if(curr->cr-crt < list->tol && curr->cr-crt >= 0) return 1;
	return 0;
}

int isQPCRNodeinList(QPCRList* list, int qp){
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

void updateList(QPCRList* list, int qptmp, float crtmp){
	if(list->numOfNodes==0){
		list->head = addQPCRNodeinList(list,qptmp,crtmp);
		list->curr = list->head;
		list->last = NULL;
	}else{
		list->curr = addQPCRNodeinList(list,qptmp,crtmp);
		list->last = getLastQPCRNode(list);
	}
}


void freeQPCRList(QPCRList** plist){
	if(plist==NULL || *plist==NULL) return;
	QPCRList* list=*plist;
	QPCRNode* p = list->head;
	if(p != NULL){
		while(p->next != NULL){
			p = p->next;
			free(p->prev);
		}
		free(p);
	}
	list->head=NULL;
	list->curr=NULL;
	list->last=NULL;
	if(list->fit!=NULL) free(list->fit);
	free(list);
}

int generateNextQP(QPCRList* list){
	QPCRNode* curr=list->curr;
	QPCRNode* last=list->last;
	float crt=list->crt;

	if (curr==NULL) return generateFirstQP(list);
	if (last==NULL) return generateSecondQP(list);
	updateFit(list);
	
	float* fit=list->fit;
	int nextQP;
	switch(list->fitMode){
		case FITLINEAR:
		default:
			nextQP=(int)(fit[0]*(float)list->crt+fit[1]+0.5);
	}
	return (nextQP>255)?255:((nextQP<1)?1:nextQP);
}

int generateFinalQP(QPCRList* list){
	QPCRNode* curr=list->curr;
	float crt=list->crt;
	int qpf = (crt<=curr->cr)?curr->qp:curr->qp + 1;
	//qpf=(qpf>255)?255:((qpf<1)?1:qpf);
	//printf("%.2f %d\n",crt,qpf);
	return (qpf>255)?255:((qpf<1)?1:qpf);
}

QPCRList* createQPCRList(int fitMode){
	QPCRList* list=(QPCRList*)malloc(sizeof(QPCRList));
	list->head=NULL;
	list->curr=NULL;
	list->last=NULL;
	list->crt=1;
	list->imageSize=INF;
	list->bits=32;
	list->tol=0;
	list->numOfNodes=0;
	list->fitMode=fitMode;
	int para;
	switch(fitMode){
		case FITLINEAR:
		default:
			para=2;
	}
	list->fit=(float*)malloc(para*sizeof(float));
	return list;
}