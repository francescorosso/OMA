#!/bin/bash

curdir=`pwd`
workdir=$1
report=$workdir/report.txt
tmp="tmp"
bests=$workdir/bests
bestresult=1000

if [ ! -d $tmp ]
then
	mkdir $tmp
fi

if [ ! -d $bests ]
then
	mkdir $bests
fi

for (( ; ; ))
do
	output=$tmp/tmp.$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM
	$workdir/a.out input.txt $output
	result=$?
	echo $result >> $report
	if [ $result -lt $bestresult ]
	then
		bestresult=$result
		mv $output $workdir/bests/$bestresult.$RANDOM$RANDOM$RANDOM$RANDOM$RANDOM
	else
		rm $output
	fi
done
