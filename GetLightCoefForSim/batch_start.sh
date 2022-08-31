#!/bin/bash
# -------------------------------------------
# --          use bash                     --
#$ -S /bin/bash                            ##specifies the interpreting shell for this job to be the Bash shell.
# -------------------------------------------
# --             batch name                --
#$ -N calibATC
# -------------------------------------------
# --     What to redirect to where         --
# -- working directory --
#$ -cwd             # run the job in the directory specified.
#$ -o /store/users/ovtin/$JOB_NAME.o$JOB_ID
# -- Merge the standard out and standard error to one file --
#$ -j y
##$ -shell n
#$ -V                 ##will pass all environment variables to the job
##$ -e /dev/null
# -------------------------------------------
# --             Enviroment                --
##$ -v PATH=$PATH:$HOME/release/KdRunFastMon,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/release/lib,KDBHOST=bison-2
#$ -v PATH=$PATH:$HOME/release/KdRunFastMon,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/release/lib,KDBHOST=localhost
# -------------------------------------------
# --             Queue list                --
##$ -soft
##$ -hard
##$ -l time=24:00:00
#$ -q remote
##$ -q extralong
##$ -q day
#
# -- Send mail at submission and completion of script --
#$ -m beas
#$ -M ovtin.ivan@gmail.com

#$ -t 1-5

i=${SGE_TASK_ID}
myrand=$[1000+$i]

verbose=1
longStableATC=0
lightCoeffATC=1
cnt_type=$i
##list_time_regions=( 1 2 )
##list_time_regions=( 3 4 )
##list_time_regions=( 5 6 )
##list_time_regions=( 7 8 )
##list_time_regions=( 9 10 )
##list_time_regions=( 11 12 )
##list_time_regions=( 13 14 )
##list_time_regions=( 15 16 )
list_time_regions=( 17 18 )

for time_region in ${list_time_regions[@]}
do
   $HOME/development/cosmicMuons/GetLightCoefForSim/light_coefficient $verbose $longStableATC $lightCoeffATC $time_region $cnt_type
done

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

