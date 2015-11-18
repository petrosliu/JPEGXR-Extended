#!/bin/bash
# This is my first script.

for i in {1..15}
do
./JxrEncApp -L 32 -q $i -i test_data1 -w 576 -h 560 -o out.jxr
./JxrDecApp -i out.jxr -o out.tif
./snr out.tif test_data1 576 560 32
ls -l out.jxr  | awk '{print  $5}'
done

echo =====================

for i in {1..15}
do
./JxrEncApp -L 32 -q $i -i test_data2 -w 576 -h 560 -o out.jxr 
./JxrDecApp -i out.jxr -o out.tif
./snr out.tif test_data2 576 560 32
ls -l out.jxr  | awk '{print  $5}'
done

echo =====================

for i in {1..15}
do
./JxrEncApp -L 16 -q $i -i wavefield1_16b -w 576 -h 560 -o out.jxr
./JxrDecApp -i out.jxr -o out.tif
./snr out.tif wavefield1_16b 576 560 16 
ls -l out.jxr  | awk '{print  $5}'
done

echo =====================
for i in {1..15}
do
./JxrEncApp -L 8 -q $i -i wavefield1_8b -w 576 -h 560 -o out.jxr
./JxrDecApp -i out.jxr -o out.tif 
./snr out.tif wavefield1_8b 576 560 8
ls -l out.jxr  | awk '{print  $5}'
done


echo =====================
for i in {1..15}
do
./JxrEncApp -L 32f -q $i -i x1_of -h 241 -w 1025 -o out.jxr
./JxrDecApp -i out.jxr -o out.tif -F
#./scaleback out.tif out.float 1025 241 2.922378784770679e-09
./mse_ori out.tif x1_of 1025 241
ls -l out.jxr  | awk '{print  $5}'
done
