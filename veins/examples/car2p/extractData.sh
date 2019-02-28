#!bin/bash

if [ "$#" -ne 4 ]
then
  echo "Usage: ./genPerlRunFile.sh <configname> <freq> <wl> <numUEs>"
  exit 1
else
    configname=$1
    freq=$2
    wl=$3
    numUEs=$4
fi

BASEDIR=$(dirname $0)
scripts=$BASEDIR"/../../scripts"
input=$BASEDIR"/results/"$configname"/out"
csv=$BASEDIR"/results/"$configname"/csv"
opp_sca=$scripts"/opp_sca2csv.pl"
opp_vec=$scripts"/opp_vec2csv_v2.pl"

if [ ! -d ${csv} ]; then
    mkdir ${csv}
fi

if ls ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.vec > /dev/null 2>&1; then
    perl $opp_vec -A repetition -F localTime:vector -F endToEndDelay:vector -F offloadDelay:vector ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.vec > ${csv}/freq${freq}-wl${wl}-numUEs${numUEs}-vec.csv
fi

if ls ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > /dev/null 2>&1; then
    perl $opp_sca -m '^scenario\.car\[?[0-9]+\]\.application' numReceived:last -f ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > ${csv}/car-freq${freq}-wl${wl}-numUEs${numUEs}-sca.csv
    
    perl $opp_sca -m '^scenario\.ped\[?[0-9]+\]\.udpApp\[?[0-9]+\]' numSent:last numReceived:last -f ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > ${csv}/ped-freq${freq}-wl${wl}-numUEs${numUEs}-sca.csv
fi
