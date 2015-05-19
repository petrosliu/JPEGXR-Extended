// code mark: this file is added

#include <stdlib.h>
#include <string.h>

#include <strcodec.h>
#include <JXRTest.h> /*gonna need to update this...*/

//===================================================
typedef float FLOAT;
typedef double DOUBLE;

//==================================================
// helpers come from the data entry at the beginning that are passed by nothing... need to customize
// so that there is a data structure that accommodates this functionality...

//we just care about decode at this point...
//
//==================================================
// Decode Raw (semantically, this is redundant, but whatever...)

//may need to update pID which is a PKTestDecode data structure... unless we hijack the EXT.TIF
//, ARGInputs* pMyArgs passed in through pID somehow...
ERR ParseRawArgInput ( PKTestDecode* pID, struct WMPStream* pWS )
{
    ERR err = WMP_errSuccess;
    PKPixelInfo PI;
    size_t offPosBase = 0;
    //hijacking EXT.TIF for the sake of time
    //default
    pID->EXT.TIF.uRowsPerStrip = ( U32 ) - 1; //this is useless...
    pID->EXT.TIF.uInterpretation = ( U32 ) - 1;
    pID->EXT.TIF.uSamplePerPixel = ( U32 ) - 1;
    //pID->EXT.TIF.uBitsPerSample = (U32) -1;
    //pID->EXT.TIF.uSampleFormat = 1;
    pID->EXT.TIF.uResolutionUnit = 2;
    pID->EXT.TIF.fResX = 96; //these will likely change...
    pID->EXT.TIF.fResY = 96; //these will likely change... must compare to reference code...
    //================================
    Call ( pWS->GetPos ( pWS, &offPosBase ) );
    FailIf ( 0 != offPosBase, WMP_errUnsupportedFormat );
    //================================
    //now we initialize pID from the ARGInputs
    pID->EXT.TIF.uExtraSamples = 0;//either 0,1,2 available. probably 0
    pID->EXT.TIF.uInterpretation = PK_PI_B0;//wild guess for now... ish. probably stands for min_is_black
    pID->EXT.TIF.uStripOffsets = 0;
    pID->EXT.TIF.uSamplePerPixel = 1; //ncomp = 1 from reference code
    pID->EXT.TIF.uRowsPerStrip = pID->uHeight;
    //try and keep default resolutionUnit and fResX and fResY
    FailIf (
        ( -1 == pID->EXT.TIF.uInterpretation || -1 == pID->EXT.TIF.uSamplePerPixel || -1 == pID->EXT.TIF.uBitsPerSample ),
        WMP_errUnsupportedFormat );
    PI.uInterpretation = pID->EXT.TIF.uInterpretation;
    PI.uSamplePerPixel = pID->EXT.TIF.uSamplePerPixel;
    PI.uBitsPerSample = pID->EXT.TIF.uBitsPerSample;
    PI.uSampleFormat = pID->EXT.TIF.uSampleFormat;
    PI.grBit =
        pID->EXT.TIF.uExtraSamples == 1 || pID->EXT.TIF.uExtraSamples == 2
        ||
        /* Workaround for some images without correct info about alpha channel */
        ( pID->EXT.TIF.uExtraSamples == 0
          && pID->EXT.TIF.uSamplePerPixel > 3 ) ?
        PK_pixfmtHasAlpha : 0x0;
    PI.grBit |= pID->EXT.TIF.uExtraSamples == 1 ? PK_pixfmtPreMul : 0x0;
    pID->fResX = (
                     3 == pID->EXT.TIF.uResolutionUnit ?
                     ( Float ) ( pID->EXT.TIF.fResX * 2.54 ) : pID->EXT.TIF.fResX ); //cm -> inch
    pID->fResY = (
                     3 == pID->EXT.TIF.uResolutionUnit ?
                     ( Float ) ( pID->EXT.TIF.fResY * 2.54 ) : pID->EXT.TIF.fResY ); //cm -> inch
    Call ( PixelFormatLookup ( &PI, LOOKUP_BACKWARD_TIF ) );
    pID->guidPixFormat = * ( PI.pGUIDPixFmt );
Cleanup:
    return err;
}

ERR PKImageDecode_Initialize_RAW ( PKTestDecode* pID, struct WMPStream* pWS )
{
    ERR err = WMP_errSuccess;
    Call ( PKTestDecode_Initialize ( pID, pWS ) ); /* should be no problem here... line 220 in JXRTest.c */
    /*use special arg pass*/
    Call ( ParseRawArgInput ( pID, pWS ) ); //custom
Cleanup:
    return err;
}

//may need copy and release
ERR PKImageDecode_Copy_RAW ( PKTestDecode* pID, const PKRect* pRect, U8* pb,
                             U32 cbStride )
{
    ERR err = WMP_errSuccess;
    struct WMPStream* pS = pID->pStream;
    PKPixelInfo PI;
    U32 cbLine = 0;
    I32 i = 0;
    PI.pGUIDPixFmt = &pID->guidPixFormat;
    PixelFormatLookup ( &PI, LOOKUP_FORWARD );
    cbLine = cbStride;//this might be the fix!!!!!!!!
//	cbLine = (
//			BD_1 == PI.bdBitDepth ?
//					((PI.cbitUnit * pRect->Width + 7) >> 3) :
//					(((PI.cbitUnit + 7) >> 3) * pRect->Width));
    assert ( 0 == pRect->X && pID->uWidth == ( U32 ) pRect->Width );
    assert ( cbLine <= cbStride );
    for ( i = 0; i < pRect->Height; ++i )
    {
        U32 offPixels = 0;
        //Call(GetScanLineOffset(pID, pRect->Y + i, cbLine, &offPixels));
        offPixels = i * cbLine; /*perfect hack.*/
        Call ( pS->SetPos ( pS, offPixels ) );
        Call ( pS->Read ( pS, pb + cbStride * i, cbLine ) );
        if ( PK_PI_W0 == pID->EXT.TIF.uInterpretation )
        {
            U32 j, begin = cbStride * ( U32 ) i, end = begin + cbLine;
            for ( j = begin; j < end; ++j )
                pb[j] = ~pb[j];
        }
    }
Cleanup:
    return err;
}

ERR PKImageDecode_Release_RAW ( PKTestDecode** ppID ) //copy of Release_TIF
{
    ERR err = WMP_errSuccess;
    PKTestDecode* pID = *ppID;
    Call ( WMPFree ((void**) &pID->EXT.TIF.uStripOffsets ) ); //should be zero
    Call ( WMPFree ((void**) &pID->EXT.TIF.uStripByteCounts ) ); //
    Call ( PKTestDecode_Release ( ppID ) );
Cleanup:
    return err;
}

ERR PKImageDecode_Create_RAW ( PKTestDecode** ppID )
{
    //asm(""); /*supposed work around to prevent optimization of ppID*/
    ERR err = WMP_errSuccess;
    PKTestDecode* pID  = NULL;
    //Call(PKTestDecode_Create(ppID)); /*moved this to CreateCodec*/
    pID = *ppID;
    pID->Initialize = PKImageDecode_Initialize_RAW;/*maybe initialize to pass pMyArgs correctly...*/
    pID->Copy = PKImageDecode_Copy_RAW; /* try it without changing from tif copy */
    pID->Release = PKImageDecode_Release_RAW;
Cleanup:
    return err;
}

ERR PKImageEncode_WritePixels_RAW(PKImageEncode* pIE, U32 cLine, U8* pbPixel,
                U32 cbStride) {
        ERR err = WMP_errSuccess;

        struct WMPStream* pS = pIE->pStream;
        PKPixelInfo PI;
        size_t cbLine = 0;
        size_t offPos = 0;
        size_t i = 0;

        PI.pGUIDPixFmt = &pIE->guidPixFormat;
        PixelFormatLookup(&PI, LOOKUP_FORWARD);

        cbLine = (
                        BD_1 == PI.bdBitDepth ?
                                        ((PI.cbitUnit * pIE->uWidth + 7) >> 3) :
                                        (((PI.cbitUnit + 7) >> 3) * pIE->uWidth));

        FailIf(cbStride < cbLine, WMP_errInvalidParameter);

        offPos = pIE->offPixel + cbLine * pIE->idxCurrentLine;
        Call(pS->SetPos(pS, offPos));

        for (i = 0; i < cLine; ++i) {
                Call(pS->Write(pS, pbPixel + cbStride * i, cbLine));
        }
        pIE->idxCurrentLine += cLine;

        Cleanup: return err;
}

ERR PKImageEncode_Create_RAW(PKImageEncode** ppIE) {
        ERR err = WMP_errSuccess;

        PKImageEncode* pIE = NULL;

        Call(PKImageEncode_Create(ppIE));

        pIE = *ppIE;
        pIE->WritePixels = PKImageEncode_WritePixels_RAW;
        Cleanup: return err;
}

