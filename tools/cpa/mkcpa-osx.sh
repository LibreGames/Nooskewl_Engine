#!/bin/sh

FILES=`find * -type f | grep -v LICENSE.txt | grep -v README.txt | grep -v "^flp" | grep -v .DS_Store | sort`

echo "Writing header..."
ls -l $FILES | awk '{sum += $5} END {print sum}' > $1

echo "Writing data..."
cat $FILES >> $1

echo "Writing info..."
ls -l $FILES | awk '{size = $5; name = $9} {printf "%d\t%s\n", size, name}' >> $1

echo "Saving uncompressed archive..."
cp $1 $1.uncompressed

echo "Compressing..."
gzip $1
mv $1.gz $1
