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
#$ -o $JOB_NAME.o$JOB_ID
#$ -j y
# -------------------------------------------
# --             Enviroment                --
#$ -v PATH=$PATH:$HOME/release/KdRunFastMon,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/alexbarn/release/lib,KDBHOST=bison-2
# -------------------------------------------
# --             Queue list                --
#$ -soft
#$ -l time=24:00:00
#$ -q remote
##$ -q extralong
#$ -m beas
#$ -M ovtin.ivan@gmail.com

##$ -t 1-4

i=${SGE_TASK_ID}
myrand=$[1000+$i]

inruns="/spool/users/ovtin/cosmruns/march16cosmruns_4"
outfile="cosm_runs_march16_4.root"

##inruns="/spool/users/ovtin/cosmruns/jun14cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/sep14cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/oct14cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/nov14cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jan15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/feb15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/apr15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/may15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jun15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jul15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/oct15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/nov15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/dec15cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jan16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/feb16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/march16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/apr16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/may16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jun16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/oct16cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/dec14cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jun17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/nov17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/feb17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/march17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/apr17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/may17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/oct17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/dec17cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jan18cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/feb18cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/march18cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/apr18cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/may18cosmruns_"$i
##inruns="/spool/users/ovtin/cosmruns/jun18cosmruns_"$i

##outfile="cosm_runs_jun18_"$i".root"
##outfile="cosm_runs_may18_"$i".root"
##outfile="cosm_runs_apr18_"$i".root"
##outfile="cosm_runs_march18_"$i".root"
##outfile="cosm_runs_feb18_"$i".root"
##outfile="cosm_runs_jan18_"$i".root"
##outfile="cosm_runs_dec17_"$i".root"
##outfile="cosm_runs_oct17_"$i".root"
##outfile="cosm_runs_may17_"$i".root"
##outfile="cosm_runs_apr17_"$i".root"
##outfile="cosm_runs_march17_"$i".root"
##outfile="cosm_runs_feb17_"$i".root"
##outfile="cosm_runs_nov17_"$i".root"
##outfile="cosm_runs_jun17_"$i".root"
##outfile="cosm_runs_dec14_"$i".root"
##outfile="cosm_runs_oct16_"$i".root"
##outfile="cosm_runs_jun16_"$i".root"
##outfile="cosm_runs_may16_"$i".root"
##outfile="cosm_runs_apr16_"$i".root"
##outfile="cosm_runs_march16_"$i".root"
##outfile="cosm_runs_feb16_"$i".root"
##outfile="cosm_runs_jan16_"$i".root"
##outfile="cosm_runs_dec15_"$i".root"
##outfile="cosm_runs_nov15_"$i".root"
##outfile="cosm_runs_oct15_"$i".root"
##outfile="cosm_runs_jul15_"$i".root"
##outfile="cosm_runs_jun15_"$i".root"
##outfile="cosm_runs_may15_"$i".root"
##outfile="cosm_runs_apr15_"$i".root"
##outfile="cosm_runs_feb15_"$i".root"
##outfile="cosm_runs_jan15_"$i".root"
##outfile="cosm_runs_nov14_"$i".root"
##outfile="cosm_runs_oct14_"$i".root"
##outfile="cosm_runs_sep14_"$i".root"
##outfile="cosm_runs_jun14_"$i".root"

echo "inruns=" "$inruns"
echo "outfile=" "$outfile"

#start the job
##$HOME/development/cosmicMuons/analysis_cosmic -n 10000-o $outfile $inruns
$HOME/development/cosmicMuons/analysis_cosmic -o $outfile $inruns

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

