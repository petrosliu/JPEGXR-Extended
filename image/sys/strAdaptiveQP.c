//*******************************************************************
// File: image/sys/strAdaptiveQP.c
// Author: YD Liu
// Created: August 1 2015
// Comment: kit of Adaptive QP
//*******************************************************************

#include "strAdaptiveQP.h"

//*******************************************************************
// Test functions
//*******************************************************************
void printQPMatrix(QPMatrix * qpMatrix){
	int i,j;
	printf("wid:%d\tHei:%d\n",qpMatrix->iMBWid,qpMatrix->iMBHei);
	printf("DC: %d\n",qpMatrix->iDCQP);
	printf("LP:");
	if(qpMatrix->iLPNum==255) printf(" 1..255");
	else{
		for(i=0;i<qpMatrix->iLPNum;i++){
			printf(" %d",qpMatrix->iLPQP[i]);
		}
	}
	printf("\nHP:");
	if(qpMatrix->iLPNum==255) printf(" 1..255");
	else{
		for(i=0;i<qpMatrix->iHPNum;i++){
			printf(" %d",qpMatrix->iHPQP[i]);
		}
	}
	if(qpMatrix->iLPNum==255){
		for(i=0;i<qpMatrix->iMBHei;i++){
			printf("\n");
			for(j=0;j<qpMatrix->iMBWid;j++){
			}
			
				printf(" %d",qpMatrix->pLPQPMatrix[i*qpMatrix->iMBWid+j]);
		}	
	}
	else{
		printf("\nhead:");
		for(i=0;i<5;i++){
			printf(" %d %d|",qpMatrix->pLPQPMatrix[i],qpMatrix->pHPQPMatrix[i]);
		}
		printf("\ntail:");
		for(i=qpMatrix->iMBWid*qpMatrix->iMBHei-5;i<qpMatrix->iMBWid*qpMatrix->iMBHei;i++){
			printf(" %d %d|",qpMatrix->pLPQPMatrix[i],qpMatrix->pHPQPMatrix[i]);
		}
		printf("\n");
	}
}

void defaultQPMatrix(QPMatrix * qpMatrix){
	if(qpMatrix->bExtendedJXR){
		qpMatrix->iLPNum=255;
		qpMatrix->iHPNum=255;
	}else{
		qpMatrix->iLPNum=16;
		qpMatrix->iHPNum=16;
	}
	qpMatrix->iDCQP=1;
	qpMatrix->tempQP = 1;
	int i;
	for(i=0;i<qpMatrix->iLPNum;i++){
		qpMatrix->iLPQP[i]=i+1;
	}
	for(i=0;i<qpMatrix->iHPNum;i++){
		qpMatrix->iHPQP[i]=i+1;
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

int fitLinearWithTempCRT(CWMImageStrCodec * pSC, float crt){
	QPMatrix* qpMatrix = (QPMatrix*)pSC->qpMatrix;
	int bits=pSC->WMII.cBitsPerUnit;
	float cCurrRatio = 16.0*16.0*(float)bits/(float)(getBitCounter(pSC)-138);
	float sf;
	int qp;
	float a,b,crttmp;
	a = fitLinearModel(bits, 'a');
	b = fitLinearModel(bits, 'b');
	
	sf=lookupSF(qpMatrix->tempQP)/(cCurrRatio+b/a)*(crt+b/a);
	qp=lookupQP((int)sf);
	qp=(qp>255)?255:(qp<1)?1:qp;
	return qp;
}

void distributeQPs(int* qps, int num, int qp, int method, float par){
	if(num==255){
		int i;
		for(i=0;i<num;i++){
			qps[i]=i+1;
		}
	}
	return;
	
	switch(method){ //0:uniform 1:interval 2:curve
		case 0:{
			int i;
			for(i=0;i<num;i++){
				qps[i]=254/(num-1)*i+1;
			}
		}
		break;
		case 1:{
			int inv=(int)(0.0003*(float)qp*(float)qp-0.0192*(float)qp+1) + (int)par;
			inv = inv>15 ? 15 : (inv<1 ? 1 : inv);
			int half1,half2,cqp,i;
			const float pct=0.5;
			half1=(int)((float)((num-1)*inv+1)*pct+0.5);
			half2=(num-1)*inv-half1;
			cqp = qp+half2>255 ? 255 - half2 : (qp-half1<1 ? 1 + half1 : qp);
			qps[0]=1;
			for(i=1;i<num-1;i++){
				qps[i]=cqp-half1+i*inv;
			}
			qps[num-1]=255;
		}
		break;
		case 2:{
			float t=(par>254.0/((float)num-1.0))?254.0/((float)num-1.0):(par<1)?1:par;
			int m=(int)(((float)qp-1.0)*((float)num-1.0)/254.0);

			float x1,y1,x2,y2,x3,y3,x4,y4;
			float a1,b1,c1,a2,b2,c2;
			int i;
			
			x1=m-3<1?1.0:m-3.0; y1=(float)qp+(x1-(float)m)*t;
			x2=0; y2=1.0;
			a1=(y1-y2+(x2-x1)*t)/(x1*x1-x2*x2+2*x1*(x2-x1));
			b1=t-2*a1*x1;
			c1=y1-a1*x1*x1-b1*x1;
			
			x3=m+3>num-1?(float)num-1:(float)m+3; y3=(float)qp+(x3-(float)m)*t;
			x4=(float)num-1.0; y4=255.0;
			a2=(y3-y4+(x4-x3)*t)/(x3*x3-x4*x4+2*x3*(x4-x3));
			b2=t-2*a2*x3;
			c2=y3-a2*x3*x3-b2*x3;
				
			for(i=0;i<num;i++){
				if(i<x1) qps[i]=(int)(a1*i*i+b1*i+c1);
				else if(i>x3) qps[i]=(int)(a2*i*i+b2*i+c2);
				else qps[i]=(int)(qp+(i-m)*t);
				qps[i]=(qps[i]<1)?1:((qps[i]>255)?255:qps[i]);
			}
		}
		break;
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
	qpMatrix->bExtendedJXR = pSC->WMISCP.bExtendedJXR;
	if(qpMatrix->bExtendedJXR){
		qpMatrix->iLPNum=255;
		qpMatrix->iHPNum=255;
	}else{
		qpMatrix->iLPNum=16;
		qpMatrix->iHPNum=16;
	}
	
	qpMatrix->iDCQP=1;
	qpMatrix->tempQP = 1;
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
	
	qpMatrix->psnr=(double*)malloc(qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(double));
	memset(qpMatrix->psnr,0,qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(double));
	qpMatrix->rn=(double*)malloc(qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(double));
	memset(qpMatrix->psnr,0,qpMatrix->iMBWid*qpMatrix->iMBHei*sizeof(double));
	
	return qpMatrix;
}

void adpativeMBQP(CWMImageStrCodec * pSC){
	QPMatrix* qpMatrix = (QPMatrix*)pSC->qpMatrix;
	const int MBindex = (pSC->cRow-1)*pSC->cmbWidth+pSC->cColumn-1;
	int* pLPQP = qpMatrix->pLPQPMatrix+MBindex;
	int* pHPQP = qpMatrix->pHPQPMatrix+MBindex;
	int* pLPQPs = qpMatrix->iLPQP;
	int* pHPQPs = qpMatrix->iHPQP;
	
	printf("%f\n",qpMatrix->rn[MBindex]);
	if(!(pSC->cColumn==1&&pSC->cRow==1)&&(pSC->cColumn==1)){
		const float MBpercent = ((float) MBindex + 1.0)
								/(float) qpMatrix->iMBHei
								/(float) qpMatrix->iMBWid;
		const int cRawBits = pSC->WMII.cWidth * pSC->WMII.cHeight * pSC->WMII.cBitsPerUnit;
		int cCurrBits = getBitCounter(pSC);
		const float fltCRatio = qpMatrix->fltCRatio;
		float restBits=0;
		{
		int i;
		for(i=MBindex;i<qpMatrix->iMBWid*qpMatrix->iMBHei;i++)
			restBits+=(16.0*16.0*pSC->WMII.cBitsPerUnit)/(fltCRatio+qpMatrix->rn[MBindex]);
		}
		float fltRestCRatio = restBits/((float) cRawBits / fltCRatio - (float) cCurrBits)
							*(fltCRatio+qpMatrix->rn[MBindex]);
		//if(fltRestCRatio<=0) fltRestCRatio=INF;
			double mpsnr=0;
			int i=(MBindex<=2*qpMatrix->iMBWid)?1:MBindex-2*qpMatrix->iMBWid;
			double m=(double)MBindex-(double)i-1.0;
			for(;i<MBindex;i++){
				mpsnr+=qpMatrix->psnr[i];
			}
			mpsnr/=m;
			if(fltRestCRatio < fltCRatio && fltRestCRatio <= qpMatrix->fltRestCRatio){
				int qptmp=quickQP(mpsnr,pSC->WMII.cBitsPerUnit);
				if(qpMatrix->tempQP<=qptmp) qpMatrix->tempQP -= 1;
				else qpMatrix->tempQP -= (qpMatrix->tempQP-qptmp)/20 + 1;
			}
			else if (fltRestCRatio > fltCRatio && fltRestCRatio >= qpMatrix->fltRestCRatio){
				int qptmp=quickQP(mpsnr,pSC->WMII.cBitsPerUnit);
				if(qpMatrix->tempQP>=qptmp) qpMatrix->tempQP += 1;
				else qpMatrix->tempQP += (qptmp-qpMatrix->tempQP)/20 + 1;
			}

		qpMatrix->tempQP=(qpMatrix->tempQP>255)?255:(qpMatrix->tempQP<1)?1:qpMatrix->tempQP;

		if(qpMatrix->bExtendedJXR){
			*pLPQP = qpMatrix->tempQP;
			*pHPQP = qpMatrix->tempQP;
		}
		else{
			*pLPQP = seachQP(qpMatrix->iLPQP, qpMatrix->iLPNum, qpMatrix->tempQP)+(fltRestCRatio > fltCRatio);
			*pHPQP = seachQP(qpMatrix->iHPQP, qpMatrix->iHPNum, qpMatrix->tempQP)+(fltRestCRatio > fltCRatio);
			*pLPQP = (*pLPQP >= qpMatrix->iLPNum)?qpMatrix->iLPNum-1:(*pLPQP < 0)? 0:*pLPQP;
			*pHPQP = (*pHPQP >= qpMatrix->iHPNum)?qpMatrix->iHPNum-1:(*pHPQP < 0)? 0:*pHPQP;
		}
		qpMatrix->fltRestCRatio = fltRestCRatio;
		printf("%d %.2f %d %d %.2f %.2f %d %d %.0f\n",MBindex,MBpercent,*pLPQP,*pHPQP,fltCRatio,fltRestCRatio,cRawBits,cCurrBits,((float) cRawBits / fltCRatio - (float) cCurrBits));
	}else{
		if(qpMatrix->bExtendedJXR){
			*pLPQP = qpMatrix->tempQP;
			*pHPQP = qpMatrix->tempQP;
		}
		else{
			*pLPQP = seachQP(qpMatrix->iLPQP, qpMatrix->iLPNum, qpMatrix->tempQP);
			*pHPQP = seachQP(qpMatrix->iHPQP, qpMatrix->iHPNum, qpMatrix->tempQP);
		}
		//printf("%d %d %d %d %d\n",MBindex,*pLPQP,*pHPQP,pLPQPs[*pLPQP],pHPQPs[*pHPQP]);
	}
	qpMatrix->psnr[MBindex]=quickPSNR(qpMatrix->tempQP,pSC->WMII.cBitsPerUnit);
	//printf("%d\n",qpMatrix->tempQP);
}

void updateQPs(QPMatrix * qpMatrix, int qp){
	qp=(qp>255)?255:(qp<1)?1:qp;
	qpMatrix->tempQP = qp;
	qpMatrix->iDCQP = 1;
	distributeQPs(qpMatrix->iLPQP, qpMatrix->iLPNum, qp, 2, 1);
	distributeQPs(qpMatrix->iHPQP, qpMatrix->iHPNum, qp, 2, 2);
}

void finalizeQPMatrix(QPMatrix * qpMatrix){
	if(qpMatrix->bExtendedJXR) return;
	const int mbNum=qpMatrix->iMBWid*qpMatrix->iMBHei;
	finalizeQPs(qpMatrix->iLPQP, &qpMatrix->iLPNum,qpMatrix->pLPQPMatrix,mbNum);
	finalizeQPs(qpMatrix->iHPQP, &qpMatrix->iHPNum,qpMatrix->pHPQPMatrix,mbNum);
}