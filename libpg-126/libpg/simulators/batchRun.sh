#!/bin/sh

if [ $# -lt 2 ] ; then
	echo "Usage: batchRun.sh <executable> <experimentName>"
	exit
fi

RUNS=100
run=0
DIR="results.$1-$2"
mkdir $DIR

while [ $run -lt $RUNS ] ; do

	echo "Run $run"

	OPTIONS=$run
	$1 $OPTIONS > $DIR/$run.log
	run=`expr $run + 1`

done
