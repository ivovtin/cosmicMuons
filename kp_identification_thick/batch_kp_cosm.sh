#!/bin/bash
# -------------------------------------------
# --          use bash                     --
#$ -S /bin/bash
# -------------------------------------------
# --             batch name                --
#$ -N atc
# -------------------------------------------
# --     What to redirect to where         --
#$ -cwd
#$ -o /store/users/ovtin/$JOB_NAME.o$JOB_ID
#$ -j y
##$ -shell n
#$ -V                 ##will pass all environment variables to the job
##$ -e /dev/null
# -------------------------------------------
# --             Enviroment                --
#$ -v PATH=$PATH:$HOME/development/lib:/home/ovtin/development/KrKRec,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/development/lib,KDBHOST=bison-2
##$ -v PATH=$PATH:$HOME/release/KdRunFastMon,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/release/lib,KDBHOST=localhost
# -------------------------------------------
# --             Queue list                --
##$ -soft
##$ -l time=24:00:00
##$ -q remote
##$ -q extralong
#$ -q day
#$ -m beas
#$ -M ovtin.ivan@gmail.com

##$ -t 1-12
#$ -t 5-5

i=${SGE_TASK_ID}
myrand=$[1000+$i]

if [ $i == 1 ]; then
Npethr=0.1
echo "Npethr=" "$Npethr"
fi

if [ $i == 2 ]; then
Npethr=0.3
echo "Npethr=" "$Npethr"
fi

if [ $i == 3 ]; then
Npethr=0.5
echo "Npethr=" "$Npethr"
fi

if [ $i == 4 ]; then
Npethr=0.7
echo "Npethr=" "$Npethr"
fi

if [ $i == 5 ]; then
Npethr=1.0
echo "Npethr=" "$Npethr"
fi

if [ $i == 6 ]; then
Npethr=1.5
echo "Npethr=" "$Npethr"
fi

if [ $i == 7 ]; then
Npethr=2.0
echo "Npethr=" "$Npethr"
fi

if [ $i == 8 ]; then
Npethr=2.5
echo "Npethr=" "$Npethr"
fi

if [ $i == 9 ]; then
Npethr=3.0
echo "Npethr=" "$Npethr"
fi

if [ $i == 10 ]; then
Npethr=3.5
echo "Npethr=" "$Npethr"
fi

if [ $i == 11 ]; then
Npethr=4.0
echo "Npethr=" "$Npethr"
fi

if [ $i == 12 ]; then
Npethr=4.5
echo "Npethr=" "$Npethr"
fi

#start the job
$HOME/development/cosmicMuons/kp_identification_thick/kp_cosmic 2023 $Npethr
##$HOME/development/cosmicMuons/kp_identification_thick/kp_cosmic 2014 $Npethr

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

