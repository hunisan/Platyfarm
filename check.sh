#!/bin/bash
#finds image files that are not loaded ergo not used

images=`ls data/img/`
count=0
for i in $images ; do
	#echo $i
	if ! grep -q $i data/xml/img.xml ; then
		if [ ! "$1" = "q" ]; then
			echo $i
		fi
		let count=count+1
		#echo asd
	fi

done

echo $count

