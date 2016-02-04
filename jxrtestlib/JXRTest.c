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
const int LOOKUP[255] = {
	1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256, 272, 288, 304, 320, 336, 352, 368, 384, 400, 416, 432, 448, 464, 480, 496, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048, 2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 4096, 4352, 4608, 4864, 5120, 5376, 5632, 5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936, 8192, 8704, 9216, 9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848, 15360, 15872, 16384, 17408, 18432, 19456, 20480, 21504, 22528, 23552, 24576, 25600, 26624, 27648, 28672, 29696, 30720, 31744, 32768, 34816, 36864, 38912, 40960, 43008, 45056, 47104, 49152, 51200, 53248, 55295, 57343, 59391, 61439, 63487, 65535, 69631, 73727, 77823, 81919, 86015, 90111, 94206, 98302, 102398, 106494, 110590, 114686, 118782, 122877, 126973
};
//================================================================
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

FILE * convert2int(ARGInputs* pMyArgs, struct WMPStream* pStream) {
	FILE * fp;
	double temp;
	int32_t *imageOut;
	double ma;
	double mi;
	double max;
	double quantStep;
	double snr;
	const int shift=3;
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
            if(quantStep>qpf){
                if(pMyArgs->snr!=INF){
                    printf("SNR target should be lower than %ddB.\n",
                        (int)(-log10(quantStep*pow(sigma2/(double)(size-zPixelNo),-0.5)*pow(2,shift+1)/sqrt(12))*20+0.5)
                    );
                }
            }else{
                quantStep=qpf;
            }
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
const PKIID IID_PKImagePnmEncode = 102;
const PKIID IID_PKImageBmpEncode = 103;
const PKIID IID_PKImageTifEncode = 104;
const PKIID IID_PKImageHdrEncode = 105;
const PKIID IID_PKImageIyuvEncode = 106;
const PKIID IID_PKImageYuv422Encode = 107;
const PKIID IID_PKImageYuv444Encode = 108;
const PKIID IID_PKImageRawEncode = 0; /* Rabih added: worthless */

const PKIID IID_PKImageBmpDecode = 202;
const PKIID IID_PKImagePnmDecode = 203;
const PKIID IID_PKImageTifDecode = 204;
const PKIID IID_PKImageHdrDecode = 205;
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

	static PKIIDInfo iidInfo[] = { { ".bmp", &IID_PKImageBmpEncode,
			&IID_PKImageBmpDecode }, { ".ppm", &IID_PKImagePnmEncode,
			&IID_PKImagePnmDecode }, { ".pgm", &IID_PKImagePnmEncode,
			&IID_PKImagePnmDecode }, { ".pnm", &IID_PKImagePnmEncode,
			&IID_PKImagePnmDecode }, { ".pfm", &IID_PKImagePnmEncode,
			&IID_PKImagePnmDecode }, { ".tif", &IID_PKImageTifEncode,
			&IID_PKImageTifDecode }, { ".hdr", &IID_PKImageHdrEncode,
			&IID_PKImageHdrDecode }, { ".iyuv", &IID_PKImageIyuvEncode,
			&IID_PKImageIyuvDecode }, { ".yuv422", &IID_PKImageYuv422Encode,
			&IID_PKImageYuv422Decode }, { ".yuv444", &IID_PKImageYuv444Encode,
			&IID_PKImageYuv444Decode }, 
			{".raw", &IID_PKImageTifEncode, &IID_PKImageRawDecode}, /* Rabih added */
			{NULL, &IID_PKImageTifEncode, &IID_PKImageRawDecode}, /* Rabih added */
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
		*ppInfo = &iidInfo[11]; /*rabih added if else blocks for this*/
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

	if (IID_PKImageBmpEncode == *iid) {
		Call(PKImageEncode_Create_BMP((PKImageEncode**)ppv));
	} else if (IID_PKImagePnmEncode == *iid) {
		Call(PKImageEncode_Create_PNM((PKImageEncode**)ppv));
	} else if (IID_PKImageTifEncode == *iid) {
		Call(PKImageEncode_Create_TIF((PKImageEncode**)ppv));
	} else if (IID_PKImageHdrEncode == *iid) {
		Call(PKImageEncode_Create_HDR((PKImageEncode**)ppv));
	} else if (IID_PKImageIyuvEncode == *iid) {
		Call(PKImageEncode_Create_IYUV((PKImageEncode**)ppv));
	} else if (IID_PKImageYuv422Encode == *iid) {
		Call(PKImageEncode_Create_YUV422((PKImageEncode**)ppv));
	} else if (IID_PKImageYuv444Encode == *iid) {
		Call(PKImageEncode_Create_YUV444((PKImageEncode**)ppv));
	}

	else if (IID_PKImageBmpDecode == *iid) {
		Call(PKImageDecode_Create_BMP((PKTestDecode**)ppv));
	} else if (IID_PKImagePnmDecode == *iid) {
		Call(PKImageDecode_Create_PNM((PKTestDecode**)ppv));
	} else if (IID_PKImageTifDecode == *iid) {
		Call(PKImageDecode_Create_TIF((PKTestDecode**)ppv));
	} else if (IID_PKImageHdrDecode == *iid) {
		Call(PKImageDecode_Create_HDR((PKTestDecode**)ppv));
	} else if (IID_PKImageIyuvDecode == *iid) {
		Call(PKImageDecode_Create_IYUV((PKTestDecode**)ppv));
	} else if (IID_PKImageYuv422Decode == *iid) {
		Call(PKImageDecode_Create_YUV422((PKTestDecode**)ppv));
	} else if (IID_PKImageYuv444Decode == *iid) {
		Call(PKImageDecode_Create_YUV444((PKTestDecode**)ppv));
	}

	else if (IID_PKImageRawDecode == *iid) {
	/*rabih: this whole area is new*/
		Call(PKTestDecode_Create(ppMyppV));
		PKTestDecode_AppendARGS(ppMyppV,pMyArgs);
		Call(PKImageDecode_Create_RAW((ppMyppV))); /* Rabih edit: custom raw decode from input...*/

	} else if (IID_PKImageRawEncode == *iid){
	/*This one is a dummy and should never be accessed so long as we stick with the hacked decode...*/
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
	
	if(pMyArgs->isFloat)
	{
		pStream->state.file.pFile = convert2int(pMyArgs, pStream);
	}
	

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
