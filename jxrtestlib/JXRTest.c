//*@@@+++@@@@******************************************************************
//
// Copyright � Microsoft Corp.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// � Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// � Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//*@@@---@@@@******************************************************************

#include <JXRTest.h>

#define OFFSETFUNC(s) (0.001174*pow(s,2)-0.2731*s+28.07)

//================================================================

FILE * convert2int(ARGInputs* pMyArgs, struct WMPStream* pStream) {
	FILE * fp;
	double temp;
	int32_t *imageOut;
	double ma;
	double mi;
	double max;
	double quantStep;
	double snr;
	int shift=3;
	int i,j,k,l;
	int width,height;
	int size;
	width = pMyArgs->wid;
	height = pMyArgs->hei;
	fp = pStream->state.file.pFile;
	snr=pMyArgs->snr;
	
	size = width*height;
	imageOut = (int32_t *)malloc(size*sizeof(int32_t));

		float *image;
		ma = 0;
		mi = 0;
		image = (float *)calloc(size, sizeof(float));
		fread(image,sizeof(float),size,fp);
		for(i=0;i<size;i++)
		{
			ma = (((float)image[i])>ma)?((float)image[i]):ma; //max
			mi = (((float)image[i])<mi)?((float)image[i]):mi; //min
		}
		
		max = (fabs(ma)>fabs(mi))?fabs(ma):fabs(mi);
		quantStep = max/(pow(2,31)-1);
		
     /* 120  100  80   60   50   40   30
        12.4 12.5 14   16.2 17.4 19   21.2 */
        
		double offset;
		offset=(pMyArgs->snr>=100)?OFFSETFUNC(100):OFFSETFUNC(pMyArgs->snr);
		max=max/pow(2,31-offset);

            int start,index,pixelCounter;
            int blockCounter,zPixelNo;
            double sigma2=0.0;
            zPixelNo=0;
            for(i=0;i<pMyArgs->hei;i+=16){
                for(j=0;j<pMyArgs->wid;j+=16){
                    start=i*pMyArgs->wid+j;
                    blockCounter=0;
                    pixelCounter=0;
                    for(k=0;k<16;k++){
                        for(l=0;l<16;l++){
                            if(i+k<pMyArgs->hei&&j+l<pMyArgs->wid){
                                index=start+k*pMyArgs->wid+l;
                                if(fabs(image[index])>max)blockCounter++;
                                pixelCounter++;
                                sigma2+=pow(image[index],2);
                            }
                        }
                    }
                    if(blockCounter==0)zPixelNo+=pixelCounter;
                }
            }
		    pMyArgs->sigma2=sigma2;
		double qpf=pow(sigma2/(double)(size-zPixelNo),0.5)*pow(10,-snr/20)*sqrt(12)*pow(2,-shift-1);
		if(pMyArgs->snr>=100){
			quantStep=(quantStep>qpf)?quantStep:qpf;
		}else{
			pMyArgs->quant=lookupQP((int)(qpf/quantStep));
			qpf/=(double)lookupSF(pMyArgs->quant);
            qpf*=1.727;
            quantStep=(quantStep>qpf)?quantStep:qpf;
		}
		for(i = 0; i<size; i++)
		{
			imageOut[i]	= (int32_t)(((float)image[i])/quantStep);
		}
		pMyArgs->stepSize = quantStep;
	
	return fmemopen(imageOut, size*sizeof(int32_t), "rb");

}

//================================================================



//================================================================
const PKIID IID_PKImageIyuvEncode = 106;
const PKIID IID_PKImageYuv422Encode = 107;
const PKIID IID_PKImageYuv444Encode = 108;
const PKIID IID_PKImageRawEncode = 0; /* Rabih added: worthless */

const PKIID IID_PKImageIyuvDecode = 206;
const PKIID IID_PKImageYuv422Decode = 207;
const PKIID IID_PKImageYuv444Decode = 208;
const PKIID IID_PKImageRawDecode = 1; /* Rabih added */

ERR PKTestDecode_AppendARGS(PKTestDecode** ppID, ARGInputs* pMyArgs)
{
	ERR err = WMP_errSuccess;
	PKTestDecode* pID = NULL;
	pID = *ppID; /*rabih note: since called create, no need to PKAlloc again...*/
	pID->EXT.TIF.stepSize = pMyArgs->stepSize;
	pID->uWidth = pMyArgs->wid;
	pID->uHeight = pMyArgs->hei;
	pID->EXT.TIF.fLittleEndian = pMyArgs->end;//endianness
	//printf("end is set to %d\n",pMyArgs->end);
	pID->EXT.TIF.uBitsPerSample = pMyArgs->bpi; //one channel image, so treat it that way
	//printf("entering switch...\n");
	switch(pID->EXT.TIF.uBitsPerSample)
	{
		case 8:
	//		printf("got 8 bit...\n");
			pID->EXT.TIF.uSampleFormat = 1;//1 uint, 2 sint, 3 float
			break;
		case 16:
	//		printf("got 16 bit...\n");
			pID->EXT.TIF.uSampleFormat = 1;//1 uint, 2 sint, 3 float
			break;
		case 32:
	//		printf("got 32 bit!\n");
			//if(pMyArgs->isFloat)
			//	pID->EXT.TIF.uSampleFormat = 3;
			//else
				pID->EXT.TIF.uSampleFormat = 2;//1 uint, 2 sint, 3 float
	//			printf("have the correct uSampleFormat here %d\n",pID->EXT.TIF.uSampleFormat);
			break;
		default:
			Call(WMP_errInvalidArgument);
			break;
	}

	Cleanup: return err;
}

//================================================================
// Misc supporting functions
//================================================================
extern int PKStrnicmp(const char* s1, const char* s2, size_t c);

//----------------------------------------------------------------
typedef struct tagPKIIDInfo {
	const char* szExt;
	const PKIID* pIIDEnc;
	const PKIID* pIIDDec;
} PKIIDInfo;

static ERR GetTestInfo(const char* szExt, const PKIIDInfo** ppInfo) {
	ERR err = WMP_errSuccess;
/* Rabih edit: add .raw and NULL to this group and we will be solid. */

	static PKIIDInfo iidInfo[] = { 
/*
                       { ".iyuv", &IID_PKImageIyuvEncode, &IID_PKImageIyuvDecode }, 
                       { ".yuv422", &IID_PKImageYuv422Encode, &IID_PKImageYuv422Decode }, 
                       { ".yuv444", &IID_PKImageYuv444Encode, &IID_PKImageYuv444Decode }, 
*/
			{".raw", &IID_PKImageRawEncode, &IID_PKImageRawDecode}, /* Rabih added */
			{NULL, &IID_PKImageRawEncode, &IID_PKImageRawDecode}, /* Rabih added */
			};
	size_t i = 0;

	*ppInfo = NULL;
	if(szExt != NULL){
		for (i = 0; i < sizeof2(iidInfo); ++i) {
			if (0
					== PKStrnicmp(szExt, iidInfo[i].szExt,
							strlen(iidInfo[i].szExt))) {
				*ppInfo = &iidInfo[i];
				goto Cleanup;
			}
		}
	}
	else
	{
		*ppInfo = &iidInfo[1]; /*rabih added if else blocks for this, zx refer to ".raw" above */
		goto Cleanup;
	}
	Call(WMP_errUnsupportedFormat);

	Cleanup: return err;
}

ERR GetTestEncodeIID(const char* szExt, const PKIID** ppIID) {
	ERR err = WMP_errSuccess;

	const PKIIDInfo* pInfo = NULL;

	Call(GetTestInfo(szExt, &pInfo));
	*ppIID = pInfo->pIIDEnc;

	Cleanup: return err;
}

ERR GetTestDecodeIID(const char* szExt, const PKIID** ppIID) {
	ERR err = WMP_errSuccess;

	const PKIIDInfo* pInfo = NULL;

	Call(GetTestInfo(szExt, &pInfo));
	*ppIID = pInfo->pIIDDec;

	Cleanup: return err;
}

//================================================================
// PKTestFactory
//================================================================
ERR PKTestFactory_CreateCodec(const PKIID* iid, void** ppv, ARGInputs* pMyArgs) {
	ERR err = WMP_errSuccess;
	PKTestDecode** ppMyppV = (PKTestDecode**)ppv;

        if (IID_PKImageRawEncode == *iid) {
		Call(PKImageEncode_Create_RAW((PKImageEncode**)ppv));
	} 
/*
        else if (IID_PKImageIyuvEncode == *iid) {
		Call(PKImageEncode_Create_IYUV((PKImageEncode**)ppv));
	} else if (IID_PKImageYuv422Encode == *iid) {
		Call(PKImageEncode_Create_YUV422((PKImageEncode**)ppv));
	} else if (IID_PKImageYuv444Encode == *iid) {
		Call(PKImageEncode_Create_YUV444((PKImageEncode**)ppv));
	}

	else if (IID_PKImageIyuvDecode == *iid) {
		Call(PKImageDecode_Create_IYUV((PKTestDecode**)ppv));
	} else if (IID_PKImageYuv422Decode == *iid) {
		Call(PKImageDecode_Create_YUV422((PKTestDecode**)ppv));
	} else if (IID_PKImageYuv444Decode == *iid) {
		Call(PKImageDecode_Create_YUV444((PKTestDecode**)ppv));
	}
*/
	else if (IID_PKImageRawDecode == *iid) {
	/*rabih: this whole area is new*/
		Call(PKTestDecode_Create(ppMyppV));
		PKTestDecode_AppendARGS(ppMyppV,pMyArgs);
		Call(PKImageDecode_Create_RAW((ppMyppV))); /* Rabih edit: custom raw decode from input...*/

	} 

	else {
		Call(WMP_errUnsupportedFormat);
	}

	Cleanup: return err;
}
ERR PKTestFactory_CreateDecoderFromFileRaw(const char* szFilename,	PKImageDecode** ppDecoder, ARGInputs* pMyArgs) {
	ERR err = WMP_errSuccess;
	char *pExt = NULL;
	PKIID* pIID = NULL;

	struct WMPStream* pStream = NULL;
	PKImageDecode* pDecoder = NULL;
/* rabih edit: gonna need this section to take in raw files.. */
	// get file extension
	pExt = strrchr(szFilename, '.');
//	FailIf(NULL == pExt, WMP_errUnsupportedFormat);

	// get decode PKIID

	Call(GetTestDecodeIID(pExt, &pIID)); /* Rabih customized &pIID value to have raw included...*/

	// create stream
	Call(CreateWS_File(&pStream, szFilename, "rb")); /*shouldn't have problems here*/
	
	//
	//call the file memory open (fmemopen) and even can call convert2int here...
	//
	
	pStream->state.file.pFile = convert2int(pMyArgs, pStream);
	

	// Create decoder
	Call(PKTestFactory_CreateCodec(pIID, ppDecoder, pMyArgs)); /*Here is the big step. */

	pDecoder = *ppDecoder;

	// attach stream to decoder
	Call(pDecoder->Initialize(pDecoder, pStream));
	pDecoder->fStreamOwner = !0;
	
	Cleanup: return err;
}

ERR PKTestFactory_CreateDecoderFromFile(const char* szFilename,	PKImageDecode** ppDecoder, ARGInputs* pMyArgs) {
	ERR err = WMP_errSuccess;
	char *pExt = NULL;
	PKIID* pIID = NULL;

	struct WMPStream* pStream = NULL;
	PKImageDecode* pDecoder = NULL;
/* rabih edit: gonna need this section to take in raw files.. */
	// get file extension
	pExt = strrchr(szFilename, '.');
//	FailIf(NULL == pExt, WMP_errUnsupportedFormat);

	// get decode PKIID

	Call(GetTestDecodeIID(pExt, &pIID)); /* Rabih customized &pIID value to have raw included...*/

	// create stream
	Call(CreateWS_File(&pStream, szFilename, "rb")); /*shouldn't have problems here*/

	// Create decoder
	Call(PKTestFactory_CreateCodec(pIID, ppDecoder, pMyArgs)); /*Here is the big step. */

	pDecoder = *ppDecoder;

	// attach stream to decoder
	Call(pDecoder->Initialize(pDecoder, pStream));
	pDecoder->fStreamOwner = !0;

	Cleanup: return err;
}

ERR PKCreateTestFactory(PKCodecFactory** ppCFactory, U32 uVersion, ARGInputs* pMyArgs) {
	ERR err = WMP_errSuccess;
	PKCodecFactory* pCFactory = NULL;

	UNREFERENCED_PARAMETER( uVersion);

	Call(PKAlloc(ppCFactory, sizeof(**ppCFactory)));
	pCFactory = *ppCFactory;

	pCFactory->CreateCodec = PKTestFactory_CreateCodec;
	pCFactory->CreateDecoderFromFile = PKTestFactory_CreateDecoderFromFile;
	pCFactory->CreateDecoderFromFileRaw = PKTestFactory_CreateDecoderFromFileRaw; /*rabih added*/
	pCFactory->CreateFormatConverter = PKCodecFactory_CreateFormatConverter;
	pCFactory->Release = PKCreateCodecFactory_Release;

	Cleanup: return err;
}

//================================================================
// PKTestDecode
//================================================================
ERR PKTestDecode_Initialize(PKTestDecode* pID, struct WMPStream* pStream) {
	ERR err = WMP_errSuccess;

	pID->pStream = pStream;
	pID->guidPixFormat = GUID_PKPixelFormatDontCare;
	pID->fResX = 96;
	pID->fResY = 96;
	pID->cFrame = 1;

	Call(pID->pStream->GetPos(pID->pStream, &pID->offStart)); /*temp comment for segFault...*/

	Cleanup: return WMP_errSuccess;
}

ERR PKTestDecode_Copy(PKTestDecode* pID, const PKRect* pRect, U8* pb,
		U32 cbStride) {
	UNREFERENCED_PARAMETER( pID);
	UNREFERENCED_PARAMETER( pRect);
	UNREFERENCED_PARAMETER( pb);
	UNREFERENCED_PARAMETER( cbStride);

	return WMP_errAbstractMethod;
}

ERR PKTestDecode_Release(PKTestDecode** ppID) {
	PKTestDecode* pID = *ppID;

	pID->fStreamOwner && pID->pStream->Close(&pID->pStream);

	return PKFree(ppID);
}

ERR PKTestDecode_Create(PKTestDecode** ppID) { 
	ERR err = WMP_errSuccess;
	PKTestDecode* pID = NULL;

	Call(PKAlloc(ppID, sizeof(**ppID)));

	pID = *ppID; /*rabih note: the below are all abstract function */
	pID->Initialize = PKTestDecode_Initialize;
	pID->GetPixelFormat = PKImageDecode_GetPixelFormat;
	pID->GetSize = PKImageDecode_GetSize;
	pID->GetResolution = PKImageDecode_GetResolution;
	pID->GetColorContext = PKImageDecode_GetColorContext;
	pID->GetDescriptiveMetadata = PKImageDecode_GetDescriptiveMetadata;
	pID->Copy = PKTestDecode_Copy;
	pID->GetFrameCount = PKImageDecode_GetFrameCount;
	pID->SelectFrame = PKImageDecode_SelectFrame;
	pID->Release = PKTestDecode_Release;

	Cleanup: return err;
}
