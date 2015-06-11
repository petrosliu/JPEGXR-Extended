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

void printQPCRList(QPCRNode* curr, QPCRNode* head){
	QPCRNode* p = head;
	printf("qp\tcr\n");
	while(p != NULL){
		printf("%d\t%.2f", p->qp, p->cr);
		if(p == curr && curr != NULL) printf("<-\n");
		else printf("\n");
		p = p->next;
	}
	printf("EOL\n");
}

int countQPCRNode(QPCRNode* head){
	if(head == NULL) return 0;
	else{
		int n = 1;
		QPCRNode* p = head;
		while(p->next != NULL){
			p = p->next;
			n++;
		}
		return n;
	}
}

//*******************************************************************
// Private Functions
//*******************************************************************

QPCRNode* createQPCRNode(int qp, float cr, QPCRNode* prev, QPCRNode* next){
	QPCRNode* p = (QPCRNode*) malloc(sizeof(QPCRNode));
	p->qp = qp;
	p->cr = cr;
	p->prev = prev;
	p->next = next;
	return p;
}

int generateSecondQP(QPCRNode* head, float crt){
	//future work
	return 100;
}

//*******************************************************************
// Public Functions
//*******************************************************************

QPCRNode* addQPCRNodeinList(int qpc, float crc, QPCRNode* head){
	if(head == NULL) return createQPCRNode(qpc, crc, NULL, NULL);
	else if(head->cr == crc) return head;
	else{
		QPCRNode* p = head;
		while(p->cr < crc){
			if(p->next == NULL){
				QPCRNode* nodec = createQPCRNode(qpc,crc,p,NULL);
				p->next = nodec;
				return nodec;
			}
			p = p->next;
		}
		if(p->cr == crc) return p;
		else{
			QPCRNode* nodec = createQPCRNode(qpc, crc, p->prev, p);
			p->prev->next = nodec;
			p->prev = nodec;
			return nodec;
		}
	}
}

int isQPCRNodeinList(int qp, QPCRNode* head){
	QPCRNode* p = head;
	while(p != NULL){
		if(qp == p->qp) return 1;
		p = p->next;
	}
	return 0;
}

QPCRNode* findQPCRNodeinList(int qp, QPCRNode* head){
	QPCRNode* p = head;
	while(p != NULL){
		if(qp == p->qp) return p;
		p = p->next;
	}
	return NULL;
}

QPCRNode* getLastQPCRNode(float crt, QPCRNode* curr, QPCRNode* head){
	QPCRNode* p;
	if(!isQPCRNodeinList(curr->qp,head)){
		p=addQPCRNodeinList(curr->qp,curr->cr,head);
		free(curr);
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

void freeQPCRList(QPCRNode** phead){
	if(*phead == NULL) return;
	QPCRNode* p = *phead;
	while(p->next != NULL){
		p = p->next;
		free(p->prev);
	}
	free(p);
	*phead = NULL;
}

int generateNextQP(QPCRNode* curr, QPCRNode* last, float crt){
	if (last==NULL) return generateSecondQP(curr,crt);
	int nextQP=(int)((curr->qp-last->qp)/(curr->cr-last->cr)*(crt-last->cr)+last->qp+0.5);
	return (nextQP>255)?255:((nextQP<1)?1:nextQP);
}

int generateFinalQP(QPCRNode* curr, float crt){
	int qpf = (crt<=curr->cr)?curr->qp:curr->qp + 1;
	//qpf=(qpf>255)?255:((qpf<1)?1:qpf);
	//printf("%.2f %d\n",crt,qpf);
	return (qpf>255)?255:((qpf<1)?1:qpf);
}