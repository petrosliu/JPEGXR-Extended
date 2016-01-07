#!/bin/bash
rm out.jxr
rm out.raw
rm gmon.out
rm profile


#list=`ls /home/petros/Desktop/database/Snapshots/32f_*_589_236.raw`
list=`ls /home/petros/Desktop/database/TraceData/32f_*.raw`

#list=( '/home/petros/Desktop/database/TraceData/32f_18_*.raw')
#list=( '/home/petros/Desktop/database/TraceData/32f_16_*.raw')
#list=( '/home/petros/Desktop/database/Snapshots/32f_1_10_589_236.raw')
#list=( '/home/petros/Desktop/database/Snapshots/32f_29_120_589_236.raw')

if [ "$#" -eq 0 ]; then
	#tarray=( 118 105 94 81 73 69 56 42 37)
	tarray=( 120 100 90 80 70 60 50 40 30)
else
	tarray=( $1)
fi

#for tsnr in "${tarray[@]}"
for tsnr in {30..120}
do
count=0

for fname in $list
do

count=`expr $count + 1`

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

for ucnru in {1..10}
do

./JxrEncApp -L $bit -i $fname -h $height -w $width -o out.jxr -S $tsnr > /dev/null;
gprof JxrEncApp gmon.out > profile
p=$(grep -i '^[0-9. ]*convert2int$' profile | awk '{print $1}')
rm gmon.out
rm profile
echo -e "$tsnr\t$p\t32.5\t$index\t$frame"

done

done

done
