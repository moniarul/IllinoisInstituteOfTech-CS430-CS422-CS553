#!/usr/bin/env bash
#Path Where you want to create slurm files..
homepath=/exports/home/schatterjee

rm -rf $homepath/slurms
mkdir -p $homepath/slurms
mkdir -p $homepath/reports
rm -rf $homepath/ips
mkdir -p $homepath/ips

g++ -std=gnu++14  -O3  -pthread -Wall -o $homepath/cs553-pa1/pingpong/Pingpong $homepath/cs553-pa1/pingpong/Pingpong.cpp

echo "Folder /slurms created.."
touch $homepath/reports/report-pingpong.txt
echo -e "\n\n #######-----   Time: $(date). New Execution ------####### \n \n " >> $homepath/reports/report-pingpong.txt

declare -a isTcp=($1)
declare -a threads=("8" "4" "2" "1")
declare -a modes=("server" "client")

## now loop through the above array
for mode in "${modes[@]}"
  {
   for chunk in "${isTcp[@]}"
        {
        for thrd in "${threads[@]}"
        {

    #following are the content of each slurm file
cat <<EOF >$homepath/slurms/pingpong$thrd$mode$isTcp.slurm
#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=$homepath/slurms/pingpong$thrd$mode$isTcp.out
#SBATCH --wait-all-nodes=1
echo \$SLURM_JOB_NODELIST

srun $homepath/cs553-pa1/pingpong/Pingpong $mode $thrd $isTcp
EOF
               #Now Make it executable and schedule them
                chmod u+x $homepath/slurms/pingpong$thrd$mode$isTcp.slurm
                sbatch $homepath/slurms/pingpong$thrd$mode$isTcp.slurm
           }
        }
        sleep 1m #So that server scripts can be queued and running
     }


