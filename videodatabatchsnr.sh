#!/bin/bash
rm out.jxr
rm out.raw



list=`ls /home/petros/Desktop/database/Snapshots/32f_*_589_236.raw`

for fname in $list
do

tmp=${fname%%.*}
tmp=${tmp##*/}
bit=${tmp%%_*}
tmp=${tmp#*_}
index=${tmp%%_*}
tmp=${tmp#*_}
frame=${tmp%%_*}
tmp=${tmp#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10 -A -E > /dev/null
./JxrDecApp -i out.jxr -o out.raw -F -S
snr1=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit)
size1=$(stat -c%s "out.jxr")
raw=$(echo "$width*$height*32/8"| bc)
target=$(echo "$raw/10"| bc)
ratio=$(echo "scale=4;$raw/$size1"| bc)


qp=$(./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr)
./JxrDecApp -i out.jxr -o out.raw -F -S
snr2=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit)
size2=$(stat -c%s "out.jxr")
#echo $snr1 $snr2
d1=$(echo "$snr2 - $snr1"| bc)
d2=$(echo "$target - $size1"| bc)
echo -e "32.5\t$index\t$frame\t$snr1\t$snr2\t$qp\t$d1\t$size1\t$size2\t$target\t$d2\t$ratio"

done


list=`ls /home/petros/Desktop/database/Snapshots/32_*_589_236.raw`

for fname in $list
do

tmp=${fname%%.*}
tmp=${tmp##*/}
bit=${tmp%%_*}
tmp=${tmp#*_}
index=${tmp%%_*}
tmp=${tmp#*_}
frame=${tmp%%_*}
tmp=${tmp#*_}
width=${tmp%%_*}
tmp=${tmp#*_}
height=${tmp%%_*}

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10 -A -E > /dev/null
./JxrDecApp -i out.jxr -o out.raw -S
snr1=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit)
size1=$(stat -c%s "out.jxr")
raw=$(echo "$width*$height*$bit/8"| bc)
target=$(echo "$raw/10"| bc)
ratio=$(echo "scale=4;$raw/$size1"| bc)


qp=$(./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -R 10)
./JxrDecApp -i out.jxr -o out.raw -S
snr2=$(./SNR -c out.raw -o $fname -w $width -h $height -b $bit)
size2=$(stat -c%s "out.jxr")
#echo $snr1 $snr2
d1=$(echo "$snr2 - $snr1"| bc)
d2=$(echo "$target - $size1"| bc)
echo -e "$bit\t$index\t$frame\t$snr1\t$snr2\t$qp\t$d1\t$size1\t$size2\t$target\t$d2\t$ratio"

done

