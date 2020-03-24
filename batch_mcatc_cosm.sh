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

##$ -t 1-19
#$ -t 16-16

i=${SGE_TASK_ID}
myrand=$[1000+$i]

##inpatt='/spool/users/skononov/runs/psi2s/tt*.nat'
inruns="sim00000"$i".dat"
##outfile="cosm_runs_1range_"$i".root"
##outfile="cosm_runs_2range_"$i".root"
##outfile="cosm_runs_3range_"$i".root"
##outfile="cosm_runs_4range_"$i".root"
##outfile="cosm_runs_5range_"$i".root"
##outfile="cosm_runs_6range_"$i".root"
##outfile="cosm_runs_7range_"$i".root"
##outfile="cosm_runs_8range_"$i".root"
##outfile="cosm_runs_9range_"$i".root"
outfile="cosm_runs_10range_"$i".root"
##outfile="cosm_runs_11range_"$i".root"
##outfile="cosm_runs_12range_"$i".root"
##outfile="cosm_runs_13range_"$i".root"

echo "inruns=" "$inruns"
echo "outfile=" "$outfile"

#start the job
##$HOME/development/KEDR/bin/mktree2t -b -s "$inpatt" -o $outfile $inruns
#
##$HOME/development/cosmicMuons/analysis_cosmic  -v 19862 -n 100000 -o cosm_runs_15012018_12range_1.root $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 23136 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22996 -n 100000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 19635 -n 5000 -o $outfile $inruns
## 1 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 19635 -n 100000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 19635 -o $outfile $inruns
## 2 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 20035 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 20035 -o $outfile $inruns
## 3 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 20611 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 20611 -o $outfile $inruns
## 4 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 21596 -n 500000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 21596 -o $outfile $inruns
## 5 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22330 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22330 -o $outfile $inruns
## 6 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22390 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22390 -o $outfile $inruns
## 7 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22803 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 22803 -o $outfile $inruns
## 8 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 23310 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 23310 -o $outfile $inruns
## 9 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 23598 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 23598 -o $outfile $inruns
## 10 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 23863  -n 800000 -o $outfile $inruns
$HOME/development/cosmicMuons/analysis_cosmic  -v 23863 -o $outfile $inruns
## 11 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 24925 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 24925 -o $outfile $inruns
## 12 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 26776 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 26776 -o $outfile $inruns
## 13 range
##$HOME/development/cosmicMuons/analysis_cosmic  -v 27390 -n 800000 -o $outfile $inruns
##$HOME/development/cosmicMuons/analysis_cosmic  -v 27390 -o $outfile $inruns

status=$?
if [ $status != 0 ]; then
  echo "Program exited with status $status"
  exit
fi

echo "Job finished\n"

