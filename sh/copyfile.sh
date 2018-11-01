#!/bin/bash
fromDir=/home/gy/share/backups/F/movie/Arrow/arrow5
toDir=/home/gy/share/backups/F/movie/Arrow/test
# Suffix=rmvb
for i in `ls "$fromDir" | grep '.*rmvb$'`
do      
	# echo "$fromDir/$i $toDir/$i"
	cp "$fromDir/$i" "$toDir/$i"
done
