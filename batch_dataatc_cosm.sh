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
#$ -o /store/users/ovtin/$JOB_NAME.o$JOB_ID
#$ -j y
# -------------------------------------------
# --             Enviroment                --
#$ -v PATH=$PATH:$HOME/release/KdRunFastMon,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/release/lib,KDBHOST=bison-2
##$ -v PATH=$PATH:$HOME/release/KdRunFastMon,LD_LIBRARY_PATH=/usr/local/root/lib/root:/home/ovtin/release/lib,KDBHOST=localhost
# -------------------------------------------
# --             Queue list                --
##$ -soft
##$ -l time=24:00:00
##$ -q remote
#$ -q extralong
##$ -q 6h
##$ -q day
#$ -m beas
#$ -M ovtin.ivan@gmail.com

#$ -t 4-6

i=${SGE_TASK_ID}
myrand=$[1000+$i]

##inruns="/store/users/ovtin/cosmruns/march16cosmruns_4"
##outfile="cosm_runs_march16_4.root"
##inruns=23484
##outfile="cosm_runs_test.root"

##19 +
##inruns="/store/users/ovtin/cosmruns/jun14cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun14_"$i".root"

##4 +
##inruns="/store/users/ovtin/cosmruns/sep14cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_sep14_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/oct14cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_oct14_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/nov14cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_nov14_"$i".root"

##5 +
##inruns="/store/users/ovtin/cosmruns/dec14cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_dec14_"$i".root"

##2 +
##inruns="/store/users/ovtin/cosmruns/jan15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jan15_"$i".root"

##4 +
##inruns="/store/users/ovtin/cosmruns/feb15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb15_"$i".root"

##2 +
##inruns="/store/users/ovtin/cosmruns/apr15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_apr15_"$i".root"

##2 +
##inruns="/store/users/ovtin/cosmruns/may15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may15_"$i".root"

##5 +
##inruns="/store/users/ovtin/cosmruns/jun15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun15_"$i".root"

##2 +
##inruns="/store/users/ovtin/cosmruns/jul15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jul15_"$i".root"

##5 +
##inruns="/store/users/ovtin/cosmruns/oct15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_oct15_"$i".root"

##4 +
##inruns="/store/users/ovtin/cosmruns/nov15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_nov15_"$i".root"

##3 +
##inruns="/store/users/ovtin/cosmruns/dec15cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_dec15_"$i".root"

##3 +
##inruns="/store/users/ovtin/cosmruns/jan16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jan16_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/feb16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb16_"$i".root"

##5 +
##inruns="/store/users/ovtin/cosmruns/march16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march16_"$i".root"

##4 +
##inruns="/store/users/ovtin/cosmruns/apr16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_apr16_"$i".root"

##6 +
##inruns="/store/users/ovtin/cosmruns/may16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may16_"$i".root"

##4 +
##inruns="/store/users/ovtin/cosmruns/jun16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun16_"$i".root"

##10 +
##inruns="/store/users/ovtin/cosmruns/oct16cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_oct16_"$i".root"

##2 +
##inruns="/store/users/ovtin/cosmruns/feb17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb17_"$i".root"

##3 +
##inruns="/store/users/ovtin/cosmruns/march17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march17_"$i".root"

##3 +
##inruns="/store/users/ovtin/cosmruns/apr17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_apr17_"$i".root"

##3 +
##inruns="/store/users/ovtin/cosmruns/may17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may17_"$i".root"

##1 +
##inruns="/store/users/ovtin/cosmruns/jun17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun17_"$i".root"

##9 +
##inruns="/store/users/ovtin/cosmruns/oct17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_oct17_"$i".root"

##1 +
##inruns="/store/users/ovtin/cosmruns/nov17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_nov17_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/dec17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_dec17_"$i".root"

##6
##inruns="/store/users/ovtin/cosmruns/SC1_17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_SC1_17_"$i".root"

##2
##inruns="/store/users/ovtin/cosmruns/SC2_17cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_SC2_17_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/jan18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jan18_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/feb18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb18_"$i".root"

##5 +
##inruns="/store/users/ovtin/cosmruns/march18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march18_"$i".root"

##5 +
##inruns="/store/users/ovtin/cosmruns/apr18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_apr18_"$i".root"

##6 +
##inruns="/store/users/ovtin/cosmruns/may18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may18_"$i".root"

##3 +
##inruns="/store/users/ovtin/cosmruns/jun18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun18_"$i".root"

##1 +
##inruns="/store/users/ovtin/cosmruns/nov18cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_nov18_"$i".root"

##7 +
##inruns="/store/users/ovtin/cosmruns/apr19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_apr19_"$i".root"

##13 +
##inruns="/store/users/ovtin/cosmruns/may19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may19_"$i".root"

##10 +
##inruns="/store/users/ovtin/cosmruns/jun19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun19_"$i".root"

##15 +
##inruns="/store/users/ovtin/cosmruns/oct19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_oct19_"$i".root"

##13 +
##inruns="/store/users/ovtin/cosmruns/nov19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_nov19_"$i".root"

##1 +
##inruns="/store/users/ovtin/cosmruns/dec19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_dec19_"$i".root"

##21
##inruns="/store/users/ovtin/cosmruns/SC1_19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_SC1_19_"$i".root"

##3
##inruns="/store/users/ovtin/cosmruns/SC2_19cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_SC2_19_"$i".root"

##10 +
##inruns="/store/users/ovtin/cosmruns/feb20cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb20_"$i".root"

##9 +
##inruns="/store/users/ovtin/cosmruns/march20cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march20_"$i".root"

##15 +
##inruns="/store/users/ovtin/cosmruns/sep20cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_sep20_"$i".root"

##45 +
##inruns="/store/users/ovtin/cosmruns/oct20cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_oct20_"$i".root"

###################################################################################

##11
##inruns="/store/users/ovtin/cosmruns/march21cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march21_"$i".root"

##10
##inruns="/store/users/ovtin/cosmruns/april21cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_april21_"$i".root"

##6
##inruns="/store/users/ovtin/cosmruns/may21cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may21_"$i".root"

##3
##inruns="/store/users/ovtin/cosmruns/jun21cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jun21_"$i".root"

##6
##inruns="/store/users/ovtin/cosmruns/dec21cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_dec21_"$i".root"

##5
##inruns="/store/users/ovtin/cosmruns/jan22cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jan22_"$i".root"

##6
##inruns="/store/users/ovtin/cosmruns/feb22cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb22_"$i".root"

##7
##inruns="/store/users/ovtin/cosmruns/march22cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march22_"$i".root"

##11
##inruns="/store/users/ovtin/cosmruns/april22cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_april22_"$i".root"

##5
##inruns="/store/users/ovtin/cosmruns/may22cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_may22_"$i".root"

##9
##inruns="/store/users/ovtin/cosmruns/dec22cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_dec22_"$i".root"

##1
##bad ?
##inruns="/store/users/ovtin/cosmruns/jan23cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_jan23_"$i".root"

##4
##inruns="/store/users/ovtin/cosmruns/feb23cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_feb23_"$i".root"

##9
inruns="/store/users/ovtin/cosmruns/march23cosmruns_"$i
outfile="/store/users/ovtin/cosmruns/results/cosm_runs_march23_"$i".root"

##2
##inruns="/store/users/ovtin/cosmruns/april23cosmruns_"$i
##outfile="/store/users/ovtin/cosmruns/results/cosm_runs_april23_"$i".root"

echo "inruns=" "$inruns"
echo "outfile=" "$outfile"

#start the job
##$HOME/development/cosmicMuons/analysis_cosmic -n 10000 -o $outfile $inruns
$HOME/development/cosmicMuons/analysis_cosmic -o $outfile $inruns

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

