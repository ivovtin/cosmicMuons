##list=( 3 4 5 6 7 8 9 10 11 12 13 15 16 17 18 )
list=( 1 2 14 )

for range in ${list[@]}
do
  if [ $range -lt 10 ]; then
    rm data_Kcnt_all_0${range}range.dat
    cat data_Kcnt_0-20_${range}time_range.dat data_Kcnt_20-40_${range}time_range.dat data_Kcnt_40-60_${range}time_range.dat data_Kcnt_60-100_${range}time_range.dat data_Kcnt_100-120_${range}time_range.dat data_Kcnt_120-140_${range}time_range.dat data_Kcnt_140-160_${range}time_range.dat >> data_Kcnt_all_0${range}range.dat
  else
    rm data_Kcnt_all_${range}range.dat
    cat data_Kcnt_0-20_${range}time_range.dat data_Kcnt_20-40_${range}time_range.dat data_Kcnt_40-60_${range}time_range.dat data_Kcnt_60-100_${range}time_range.dat data_Kcnt_100-120_${range}time_range.dat data_Kcnt_120-140_${range}time_range.dat data_Kcnt_140-160_${range}time_range.dat >> data_Kcnt_all_${range}range.dat
  fi
done
