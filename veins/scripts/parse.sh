#!/bin/bash

if [ "$#" -ne 2 ]
then
  echo "Usage: ./parse.sh <config_name> <freq>"
  exit 1
else
  configname=$1
  inFreq=$2
fi

BASEDIR=$(dirname $0)
scripts=$BASEDIR"/../../scripts"
input=$BASEDIR"/results/"$configname"/out"
csv=$BASEDIR"/results/"$configname"/csv"
opp_sca=$scripts"/opp_sca2csv.pl"
opp_vec=$scripts"/opp_vec2csv_v2.pl"

######################################################
#   ds    = data size generated by DataGeneratorApp
#   dgi   = data generation interval 
#   pr    = penetration rate
##############################################1########

# metrics
# 1. channel load
# 2. delay from driving to parked
# 3. delay from parked to server
# 4. total delay (endToEndDelay)
# 5. total data uploaded successfully
# 6. success ratio

echo "Generate *.csv files..."

# 10.0 16.0 32.0 50.0 100.0
# 0.2 0.5 1.0 1.5 2.0
# 50 100 150 200 250
if [ ! $inFreq == 0 ]; then
    freq=$inFreq
    for wl in 0.2 0.5 1.0 1.5 2.0
        do 
            for numUEs in 50 100 150 200 250
                do
                    if ls ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > /dev/null 2>&1; then
                    
                    fi
                done
        done
    exit 1
fi

for freq in 10.0 16.0 32.0 50.0 100.0
    do
        for wl in 0.2 0.5 1.0 1.5 2.0
            do 
                for numUEs in 50 100 150 200 250
                    do
                        if ls ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > /dev/null 2>&1; then

#                             echo "freq${freq}-wl${wl}-numUEs${numUEs}"
                        
#                         perl $opp_vec -A repetition -F localTime:vector -F phoneToServerDelay:vector -F serverToPhoneDelay:vector -F serverToCarDelay:vector -F endToEndDelay:vector -F offloadDelay:vector ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.vec > ${csv}/freq${freq}-wl${wl}-numUEs${numUEs}.csv

#                 perl $opp_sca -m '^scenario\.server\.udpApp\[0\]' phoneToServerDelay:mean -f ${input}/freq${freq}-wl${wl}-rep*.sca > ${csv}/server-freq${freq}-wl${wl}-scalar.csv
                            perl $opp_sca -m 'scenario\.car\[1\].application' serverToCarDelay:mean endToEndDelay:mean -f ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > ${csv}/car-freq${freq}-wl${wl}-numUEs${numUEs}-scalar.csv
#                         
                            perl $opp_sca -m 'scenario\.ped\[0\].application' localTime:mean serverToPhoneDelay:mean offloadDelay:mean -f ${input}/freq${freq}-wl${wl}-numUEs${numUEs}-rep*.sca > ${csv}/ped-freq${freq}-wl${wl}-numUEs${numUEs}-scalar.csv

#                 sca2csv_multi.pl -m '".'^scenario\.host\[(?<module>[0-9]+)\]'."' totalRcvd totalSent <input.sca >output.csv

#                 perl $opp_sca -m '^VCPScenario\.eNodeB1' macCellThroughputUl:lteRate avgServedBlocksUl:lteAvg cellBlocksUtilizationUl:lteAvg measuredItbs:mean -f ${results}/pr${pr}-dataSize${ds}-dgi${dgi}-lte${lte}-rep*.sca > ${csv}/pr${pr}-ds${ds}-dgi${dgi}-lte${lte}-scalars.csv
                        fi
                    done
            done
    done
# =head1 NAME
# 
# opp_vec2csv.pl -- converts OMNeT++ .vec files to csv format
# 
# =head1 SYNOPSIS
# 
# opp_vec2csv.pl [options] [file ...]
# 
# -F --filter [name=alias]
# 
# 	add a column for vector [name], calling it [alias] (if provided)
# 
# -A --attr [name=alias]
# 
# 	add a column for attribute [name], calling it [alias] (if provided)
# 
# -s --sample [rate]
# 
# 	output only a random sample of one in [rate] observations
# 
# -S --seed [seed]
# 
# 	use a specific random seed for pseudo-random sampling
# 
# -v --verbose
# 
# 	log debug information to stderr
# 
# e.g.: opp_vec2csv.pl -A configname -F senderName -F receivedBytes=bytes input.vec >output.csv
# 