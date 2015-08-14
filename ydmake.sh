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

./ydclean.sh