#!/bin/bash
# This is my first script.

dropbox stop

for i in {1..19}
do
for k in {0..1}
do

let j=$k*5
r=$i.$j

list=`ls database/8/256`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 8 -i database/8/256/$name -h 256 -w 256 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/8/512`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 8 -i database/8/512/$name -h 512 -w 512 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/8/1024`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 8 -i database/8/1024/$name -h 1024 -w 1024 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/8/2250`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 8 -i database/8/2250/$name -h 2250 -w 2250 -o out.jxr -R $r
rm out.jxr
done




list=`ls database/16/256`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 16 -i database/16/256/$name -h 256 -w 256 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/16/512`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 16 -i database/16/512/$name -h 512 -w 512 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/16/1024`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 16 -i database/16/1024/$name -h 1024 -w 1024 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/16/2250`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 16 -i database/16/2250/$name -h 2250 -w 2250 -o out.jxr -R $r
rm out.jxr
done




list=`ls database/32/256`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 32 -i database/32/256/$name -h 256 -w 256 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/32/512`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 32 -i database/32/512/$name -h 512 -w 512 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/32/1024`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 32 -i database/32/1024/$name -h 1024 -w 1024 -o out.jxr -R $r
rm out.jxr
done

list=`ls database/32/2250`

for name in $list
do

echo -ne "$name\t"
./JxrEncApp -L 32 -i database/32/2250/$name -h 2250 -w 2250 -o out.jxr -R $r
rm out.jxr
done



done
done

dropbox start