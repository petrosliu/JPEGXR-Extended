
echo =====================
echo Removing files

rm scrn.txt
rm adapthuff.o
rm libjxrglue.a
rm strInvTransform.o
rm JXRGlue.o
rm strPredQuantDec.o
rm JXRGluePFC.o
rm strPredQuantEnc.o
rm JXRMeta.o
rm out.jxr
rm strPredQuant.o
rm JXRTestBmp.o
rm out.tif
rm strTransform.o
rm decode.o
rm JXRTestHdr.o
rm perfTimerANSI.o
rm postprocess.o
rm encode.o
rm JXRTest.o
rm segdec.o
rm JXRTestPnm.o
rm segenc.o
rm image.o
rm JXRTestRaw.o
rm JXRTestTif.o
rm strcodec.o
rm JXRTestYUV.o
rm strdec.o
rm JXRTranscode.o
rm strenc.o
rm JXRGlueJxr.o
rm libjpegxr.a
rm strFwdTransform.o
rm strRateControl.o

echo =====================
echo Making files
make

echo =====================
echo time ./JxrEncApp -L 8 -i wavefield1_8b -h 560 -w 576 -o out.jxr -R 10
time ./JxrEncApp -L 8 -i wavefield1_8b -h 560 -w 576 -o out.jxr -R 10
stat out.jxr
echo =====================
echo time ./JxrEncApp -L 16 -i wavefield1_16b -h 560 -w 576 -o out.jxr -R 10
time ./JxrEncApp -L 16 -i wavefield1_16b -h 560 -w 576 -o out.jxr -R 10
stat out.jxr
echo =====================
echo time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -R 10
time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -R 10
stat out.jxr
echo =====================

