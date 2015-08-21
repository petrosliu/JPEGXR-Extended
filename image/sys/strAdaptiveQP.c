//*******************************************************************
// File: image/sys/strAdaptiveQP.c
// Author: YD Liu
// Created: Augest 1 2015
// Comment: kit of Adaptive QP
//*******************************************************************

#include "strAdaptiveQP.h"

//*******************************************************************
// Test functions
//*******************************************************************
void printQPMatrix(QPMatrix * qpMatrix){
	printf("wid:%d\tHei:%d\n",qpMatrix->iMBWid,qpMatrix->iMBHei);
	printf("DC: %d\n",qpMatrix->iDCQP);
	printf("LP:");
	int i;
	for(i=0;i<qpMatrix->iLPNum;i++){
		printf(" %d",qpMatrix->iLPQP[i]);
	}
	printf("\nHP:");
	for(i=0;i<qpMatrix->iHPNum;i++){
		printf(" %d",qpMatrix->iHPQP[i]);
	}
	printf("\nhead:");
	for(i=0;i<5;i++){
		printf(" %d %d",qpMatrix->pLPQPMatrix[i],qpMatrix->pHPQPMatrix[i]);
	}
	printf("\ntail:");
	for(i=qpMatrix->iMBWid*qpMatrix->iMBHei-5;i<qpMatrix->iMBWid*qpMatrix->iMBHei;i++){
		printf(" %d %d",qpMatrix->pLPQPMatrix[i],qpMatrix->pHPQPMatrix[i]);
	}
	printf("\n");
}

void defaultQPMatrix(QPMatrix * qpMatrix){
	qpMatrix->iLPNum=16;
	qpMatrix->iHPNum=16;
	qpMatrix->iDCQP=1;
	int i;
	for(i=0;i<qpMatrix->iLPNum;i++){
		qpMatrix->iLPQP[i]=i*16+1;
	}
	for(i=0;i<qpMatrix->iHPNum;i++){
		qpMatrix->iHPQP[i]=i*16+1;
	}
	for(i=0;i<qpMatrix->iMBWid*qpMatrix->iMBHei;i++){
		qpMatrix->pLPQPMatrix[i]=0;//rand()%qpMatrix->iLPNum;
		qpMatrix->pHPQPMatrix[i]=0;//rand()%qpMatrix->iHPNum;
	}
}

//*******************************************************************
// Private Functions
//*******************************************************************
int seachQP(int* qps, int num, int qp){
	if(qps[0]>=qp) return 0; 
	if(qps[num-1]<=qp) return num-1;
	int i;
	for(i=1;i<num-1;i++){
		if(qps[i]==qp) return i;
		if(qps[i]>qp) break;
	}
	return qps[i]-qp<qp-qps[i-1]?i:i-1;
}

float fitLinearWithTempCRT(CWMImageStrCodec * pSC, float crt){
	QPMatrix* qpMatrix = (QPMatrix*)pSC->qpMatrix;
	float qp;
	float a,b,c,d;
	int bits=pSC->WMII.cBitsPerUnit;
	a = fitLinearModel(bits, crt, 'a');
	b = fitLinearModel(bits, crt, 'b');
	c = fitLinearModel(bits, crt, 'c');
	d = fitLinearModel(bits, crt, 'd');
	crt = fitLinearModel(bits, crt, 0);

	qp = a*crt + b*sqrt(crt) + c + (float)qpMatrix->iDCQP
	    -(a*qpMatrix->fltCRatio + b*sqrt(qpMatrix->fltCRatio) + c);
				
	qp=(qp>255)?255:((qp<1)?1:qp);
	//printf("%d %.4f %.4f %.4f %.4f %.4f %.4f\n",bits,a,b,c,d,crt,qp);
	return qp;
}

void updateQPsWithInterval(int* qps, int num, int qp, int inv){
	inv = inv>15 ? 15 : (inv<1 ? 1 : inv);
	int half1,half2,cqp,i;
	const float pct=0.5;
	half1=(int)((float)((num-1)*inv+1)*pct+0.5);
	half2=(num-1)*inv-half1;
	cqp = qp+half2>255 ? 255 - half2 : (qp-half1<1 ? 1 + half1 : qp);
	qps[0]=1;
	for(i=1;i<num;i++){
		qps[i]=cqp-half1+i*inv;
	}
}

void finalizeQPs(int* qps, int* num, int* matrix, int mbNum){
	int currQPs[16]={0};
	int tranQPs[16]={0};
	int bQPs[16]={0};
	int currNum=*num,newNum=0;
	int i;
	for(i=0;i<currNum;i++){
		currQPs[i]=qps[i];
	}
	for(i=0;i<mbNum;i++){
		bQPs[matrix[i]]=1;
	}
	for(i=0;i<currNum;i++){
		if(bQPs[i]){
			qps[newNum]=currQPs[i];
			tranQPs[i]=newNum;
			newNum++;
		}
	}
	for(i=0;i<mbNum;i++){
		matrix[i]=tranQPs[matrix[i]];
	}
	*num=newNum;
}
//*******************************************************************
// Public Functions
//*******************************************************************
QPMatrix* createQPMatrix(CWMImageStrCodec * pSC){
	QPMatrix* qpMatrix=(QPMatrix*)malloc(sizeof(QPMatrix));
	qpMatrix->iMBWid = (int)pSC->cmbWidth;
	qpMatrix->iMBHei = (int)pSC->cmbHeight;
	qpMatrix->iLPNum=16;
	qpMatrix->iHPNum=16;
	qpMatrix->iDCQP=1;
	qpMatrix->iLPQP=(int*)malloc(qpMatrix->iLPNum*sizeof(int));
	qpMatrix->iHPQP=(int*)malloc(qpMatrix->iHPNum*sizeof(int));
	int i;
	for(i=0;i<qpMatrix->iLPNum;i++){
		qpMatrix->iLPQP[i]=i*16+1;
	}
	for(i=0;i<qpMatrix->iHPNum;i++){
		qpMatrix->iHPQP[i]=i*16+1;
	}
	qpMatrix->pLPQPMatrix=(int*)malloc(qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(int));
	memset(qpMatrix->pLPQPMatrix,0,qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(int));
	qpMatrix->pHPQPMatrix=(int*)malloc(qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(int));
	memset(qpMatrix->pHPQPMatrix,0,qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(int));
	return qpMatrix;
}

void adpativeMBQP(CWMImageStrCodec * pSC){
	QPMatrix* qpMatrix = (QPMatrix*)pSC->qpMatrix;
	const int MBindex = (pSC->cRow-1)*pSC->cmbWidth+pSC->cColumn-1;
	int* pLPQP = qpMatrix->pLPQPMatrix+MBindex;
	int* pHPQP = qpMatrix->pHPQPMatrix+MBindex;
	
	if(MBindex){
		const float MBpercent = ((float) MBindex + 1.0)
								/(float) qpMatrix->iMBHei
								/(float) qpMatrix->iMBWid;
		const int cRawBits = pSC->WMII.cWidth * pSC->WMII.cHeight*pSC->WMII.cBitsPerUnit;
		int cCurrBits = 0;
		int k;
		for (k = 0; k < pSC->cNumBitIO; k++) {
			cCurrBits += pSC->m_ppBitIO[k]->cBitsCounter;
		}
		const float fltCRatio = qpMatrix->fltCRatio;
		const float fltRestCRatio = (float) cRawBits * (1.0 - MBpercent)
									/((float) cRawBits / fltCRatio - (float) cCurrBits);
									
		int tempQP=(int) (fitLinearWithTempCRT (pSC, fltRestCRatio));
		*pLPQP = seachQP(qpMatrix->iLPQP, qpMatrix->iLPNum, tempQP);
		*pHPQP = seachQP(qpMatrix->iHPQP, qpMatrix->iHPNum, tempQP);
	
		//printf("%d %d %d %.2f %.2f %d\n",MBindex,*pLPQP,*pHPQP,fltCRatio,fltRestCRatio,tempQP);
	}else{
		*pLPQP = seachQP(qpMatrix->iLPQP, qpMatrix->iLPNum, qpMatrix->iDCQP);
		*pHPQP = seachQP(qpMatrix->iHPQP, qpMatrix->iHPNum, qpMatrix->iDCQP);
		//printf("%d %d %d\n",MBindex,*pLPQP,*pHPQP);
	}
}

void updateQPs(QPMatrix * qpMatrix, int qp){
	qp=(qp>255)?255:(qp<1)?1:qp;
	qpMatrix->iDCQP = qp;
	int inv=(int)(0.0003*(float)qp*(float)qp-0.0192*(float)qp+1);
	updateQPsWithInterval(qpMatrix->iLPQP, qpMatrix->iLPNum, qp, inv);
	updateQPsWithInterval(qpMatrix->iHPQP, qpMatrix->iHPNum, qp, inv+1);
}

void finalizeQPMatrix(QPMatrix * qpMatrix){
	const int mbNum=qpMatrix->iMBWid*qpMatrix->iMBHei;
	finalizeQPs(qpMatrix->iLPQP, &qpMatrix->iLPNum,qpMatrix->pLPQPMatrix,mbNum);
	finalizeQPs(qpMatrix->iHPQP, &qpMatrix->iHPNum,qpMatrix->pHPQPMatrix,mbNum);
}