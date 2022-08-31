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

##list_time_regions=( 1 )
##list_time_regions=( 2 )
##list_time_regions=( 3 )
##list_time_regions=( 4 )
##list_time_regions=( 5 )
##list_time_regions=( 6 )
##list_time_regions=( 7 )
##list_time_regions=( 8 9 )
##list_time_regions=( 10 )
##list_time_regions=( 11 )
##list_time_regions=( 12 )
##list_time_regions=( 13 )
##list_time_regions=( 14 )
##list_time_regions=( 15 )
list_time_regions=( 16 )
##list_time_regions=( 17 )
##list_time_regions=( 18 )

verbose=1
cnt_type=$i

for time_region in ${list_time_regions[@]}
do
   $HOME/development/cosmicMuons/GetLightCoefForSim/light_coefficient_cnt_cor $verbose $time_region $cnt_type
done

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

