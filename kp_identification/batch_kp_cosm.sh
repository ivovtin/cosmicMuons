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

#$ -t 2014-2020

Year=${SGE_TASK_ID}
myrand=$[1000+$Year]

#start the job
$HOME/development/cosmicMuons/kp_identification/kp_cosmic $Year

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

