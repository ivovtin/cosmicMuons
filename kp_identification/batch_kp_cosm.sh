#!/bin/bash
# -------------------------------------------
# --          use bash                     --
#$ -S /bin/bash
# -------------------------------------------
# --             batch name                --
#$ -N atccosm
# -------------------------------------------
# --     What to redirect to where         --
#$ -cwd
#$ -o /spool/users/ovtin/$JOB_NAME.o$JOB_ID
#$ -j y
# -------------------------------------------
# --             Enviroment                --
##$ -v PATH=$PATH:$HOME/development/lib:/home/ovtin/development/KrKRec,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/development/lib,KDBHOST=bison-2
# -------------------------------------------
# --             Queue list                --
#$ -soft
#$ -l time=24:00:00
##$ -q remote
#$ -q extralong
#$ -m beas
#$ -M ovtin.ivan@gmail.com

#$ -t 1-7

i=${SGE_TASK_ID}
myrand=$[1000+$i]

if [ $i == 1 ]; then
Year=2014
echo "Year=" "$Year"
fi

if [ $i == 2 ]; then
Year=2015
echo "Year=" "$Year"
fi

if [ $i == 3 ]; then
Year=2016
echo "Year=" "$Year"
fi

if [ $i == 4 ]; then
Year=2017
echo "Year=" "$Year"
fi

if [ $i == 5 ]; then
Year=2018
echo "Year=" "$Year"
fi

if [ $i == 6 ]; then
Year=2019
echo "Year=" "$Year"
fi

if [ $i == 7 ]; then
Year=2020
echo "Year=" "$Year"
fi

#start the job
$HOME/development/cosmicMuons/kp_identification/kp_cosmic $Year

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

