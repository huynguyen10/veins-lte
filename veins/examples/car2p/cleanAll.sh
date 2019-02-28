#!/bin/bash
if [ "$#" -ne 1 ]
then
  echo "Usage: ./cleanAll.sh <config_name>"
  exit 1
else
  conf=$1
fi

if ls *.log > /dev/null 2>&1; then
    rm *.log
fi

if ls results/*.rt > /dev/null 2>&1; then
    rm results/*.rt
fi

echo "Remove *.sca *.vec *.vci?"
echo "Press 'y' to delete all related files"
read choice
case "$choice" in
  y)
    if ls results/$conf/out/* > /dev/null 2>&1; then
        rm results/$conf/out/*.sca
        rm results/$conf/out/*.vec
        rm results/$conf/out/*.vci
    fi
  ;;
  *) echo "No delete"
  ;;
esac

echo "Remove *.csv?"
echo "Press 'y' to delete all related files"
read choice
case "$choice" in
  y)
    if ls results/$conf/csv/*.csv > /dev/null 2>&1; then
        rm results/$conf/csv/*.csv
    fi
  ;;
  *) echo "No delete"
  ;;
esac
