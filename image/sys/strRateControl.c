#include "strRateControl.h"

void printQPCRList(QPCRNode* curr,QPCRNode* head){
	QPCRNode* p = head;
	printf("qp\tcr\n");
	while(p!=NULL){
		printf("%d\t%.2f",p->qp,p->cr);
		if(p==curr)printf("<-\n");
		else printf("\n");
		p=p->next;
	}
	printf("EOL\n");
}

QPCRNode * addQPCRNode(int qpc, float crc, QPCRNode* head){
	if(head->cr==crc) return head;
	
	if(head==NULL){
		QPCRNode* nodec = (QPCRNode*)malloc(sizeof(QPCRNode));
		nodec->qp=qpc;
		nodec->cr=crc;
		nodec->prev=NULL;
		nodec->next=NULL;
		return nodec;
	}else{
		QPCRNode* p = head;
		while(p->cr<crc){
			if(p->next==NULL){
				QPCRNode* nodec = (QPCRNode*)malloc(sizeof(QPCRNode));
				nodec->qp=qpc;
				nodec->cr=crc;
				nodec->prev=p;
				nodec->next=NULL;
				p->next=nodec;
				return nodec;
			}
			p=p->next;
		}
		if(p->cr==crc) return p;
		else{
			QPCRNode* nodec = (QPCRNode*)malloc(sizeof(QPCRNode));
			nodec->qp=qpc;
			nodec->cr=crc;
			nodec->prev=p->prev;
			nodec->next=p;
			p->prev->next=nodec;
			p->prev=nodec;
			return nodec;
		}
	}
}

int isQPCRNodeinList(int qp, QPCRNode* head){
	QPCRNode* p=head;
	while(p!=NULL){
		if(qp==p->qp) return 1;
		p=p->next;
	}
	return 0;
}

QPCRNode* findQPCRNodeinList(int qp, QPCRNode* head){
	QPCRNode* p=head;
	while(p!=NULL){
		if(qp==p->qp) return p;
		p=p->next;
	}
	return NULL;
}

QPCRNode* searchQPCRNode(float crt, QPCRNode* curr, QPCRNode* head){
	QPCRNode* p;
	if(!isQPCRNodeinList(curr->qp,head)){
		p=addQPCRNode(curr->qp,curr->cr,head);
		free(curr);
		curr=p;
	}
	p=curr;
	if(curr->cr<crt){
		while(p->cr<crt){
			if(p->next==NULL){
				if(p==curr) p=p->prev;
				break;
			}else{
				p=p->next;
			}
		}
	}else{
		while(p->cr>crt){
			if(p->prev==NULL){
				if(p==curr) p=p->next;
				break;
			}else{
				p=p->prev;
			}
		}
	}
	return p;
}