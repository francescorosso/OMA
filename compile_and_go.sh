#|/bin/bash

curdir=`pwd`
report=$curdir/report.txt

rm -rf $curdir/bin
mkdir $curdir/bin

function compile() {
	workdir=$curdir/bin/$1_$2_$3
	mkdir $workdir
	echo $workdir
	`gcc -D$1 -D$2 -D$3 $curdir/src/main.c $curdir/src/greedy_rand.c $curdir/src/adaptive_search.c -o $workdir/a.out`
}

function go() {
	workdir=$curdir/bin/$1_$2_$3
	$curdir/go.sh $workdir &
}

dynbkt="DYNBKT VOID"
wrongs="WRONGS VOID"
multistart="MSANGLE MSANGPLO MSANGPLP VOID"

for dynbktword in $dynbkt
do
	for wrongsword in $wrongs
	do
		for multistartword in $multistart
		do
			compile $dynbktword $wrongsword $multistartword
#			go $dynbktword $wrongsword $multistartword
		done
	done
done

echo "Done..."

exit 0
