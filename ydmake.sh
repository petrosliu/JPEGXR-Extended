echo =====================
echo Making files
make all
make clean

echo =====================
echo time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -q 160
time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -q 160 > /dev/null
./JxrDecApp -i out.jxr -o out.raw
./PSNR out.raw test_data1 576 560 32
echo $(stat -c%s "out.jxr") 129024
echo =====================
echo time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -R 10
time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -R 10 > /dev/null
./JxrDecApp -i out.jxr -o out.raw
./PSNR out.raw test_data1 576 560 32
echo $(stat -c%s "out.jxr") 129024
echo =====================
echo time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -R 10 -A -E
time ./JxrEncApp -L 32 -i test_data1 -h 560 -w 576 -o out.jxr -R 10 -A -E > /dev/null
./JxrDecApp -i out.jxr -o out.raw
./PSNR out.raw test_data1 576 560 32
echo $(stat -c%s "out.jxr") 129024
echo =====================