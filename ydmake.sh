echo =====================
echo Making files
make all
make clean

echo =====================
echo ./JxrEncApp -L 8 -i wavefield1_8b -w 576 -h 560 -o out.jxr
./JxrEncApp -L 8 -i wavefield1_8b -w 576 -h 560 -o out.jxr > /dev/null
./JxrDecApp -i out.jxr -o out.raw
./SNR -c out.raw -o wavefield1_8b -w 576 -h 560 -b 8
echo $(stat -c%s "out.jxr") 322560

echo ./JxrEncApp -L 16 -i wavefield1_16b -w 576 -h 560 -o out.jxr
./JxrEncApp -L 16 -i wavefield1_16b -w 576 -h 560 -o out.jxr > /dev/null
./JxrDecApp -i out.jxr -o out.raw
./SNR -c out.raw -o wavefield1_16b -w 576 -h 560 -b 16
echo $(stat -c%s "out.jxr") 645120

echo ./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr
./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr > /dev/null
./JxrDecApp -i out.jxr -o out.raw -S
./SNR -c out.raw -o test_data1 -w 576 -h 560 -b 32
echo $(stat -c%s "out.jxr") 1290240

echo ./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr -q 157
./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr -q 157 > /dev/null
./JxrDecApp -i out.jxr -o out.raw -S
./SNR -c out.raw -o test_data1 -w 576 -h 560 -b 32
echo $(stat -c%s "out.jxr") 129024

echo ./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr -R 10
./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr -R 10 > /dev/null
./JxrDecApp -i out.jxr -o out.raw -S
./SNR -c out.raw -o test_data1 -w 576 -h 560 -b 32
echo $(stat -c%s "out.jxr") 129024

echo ./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr -R 10 -A -E
./JxrEncApp -L 32 -i test_data1 -w 576 -h 560 -o out.jxr -R 10 -A -E > /dev/null
./JxrDecApp -i out.jxr -o out.raw -S
./SNR -c out.raw -o test_data1 -w 576 -h 560 -b 32
echo $(stat -c%s "out.jxr") 129024

echo ./JxrEncApp -L 32f -i wavefield1_32f -w 576 -h 560 -o out.jxr
./JxrEncApp -L 32f -i wavefield1_32f -w 576 -h 560 -o out.jxr> /dev/null
./JxrDecApp -i out.jxr -o out.raw -F -S
./SNR -c out.raw -o wavefield1_32f -w 576 -h 560 -b 32f
echo $(stat -c%s "out.jxr") 1290240

echo ./JxrEncApp -L 32f -i wavefield1_32f -w 576 -h 560 -o out.jxr -R 10
./JxrEncApp -L 32f -i wavefield1_32f -w 576 -h 560 -o out.jxr -R 10 > /dev/null
./JxrDecApp -i out.jxr -o out.raw -F -S
./SNR -c out.raw -o wavefield1_32f -w 576 -h 560 -b 32f
echo $(stat -c%s "out.jxr") 129024
echo =====================