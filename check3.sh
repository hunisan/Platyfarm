#!/bin/bash
#finds image files that are loaded but not used in any other xml files
#advanced version: also looks in the code

images=`ls data/img/`
count=0

for i in $images ; do
	
	if grep -q $i data/xml/img.xml ; then
		pattern=`echo $i | awk -F '.' '{print $1;}'`
		
		if ! grep -q -i --exclude='img.xml' $pattern data/xml/*.xml ; then
			if ! grep -q -i $pattern *.h ; then
				if ! grep -q -i $pattern *.cpp ; then
					if [ ! "$1" = "q" ]; then
						echo $pattern
					fi
					let count=count+1
				fi
			fi
		fi
	fi


done

echo $count

