#!/usr/bin/env bash
#Path Where you want to create slurm files..
homepath=/exports/home/schatterjee

rm -rf $homepath/slurms
mkdir -p $homepath/slurms
mkdir -p $homepath/reports
rm -rf $homepath/ips
mkdir -p $homepath/ips

g++ -std=gnu++14  -O3  -pthread -Wall -o $homepath/cs553-pa1/network/MyNETBench-UDP $homepath/cs553-pa1/network/MyNETBench-UDP.cpp

echo "Folder /slurms created.."
touch $homepath/reports/report-udp.txt
echo -e "\n\n #######-----   Time: $(date). New Execution ------####### \n \n " >> $homepath/reports/report-udp.txt

declare -a chunks=($1)
declare -a threads=("8" "4" "2" "1")
declare -a modes=("server" "client")

## now loop through the above array
for mode in "${modes[@]}"
  {

   for chunk in "${chunks[@]}"
        {
        for thrd in "${threads[@]}"
        {

    #following are the content of each slurm file
cat <<EOF >$homepath/slurms/udp$thrd$mode$chunk.slurm
#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=$homepath/slurms/udp$thrd$mode$chunk.out
#SBATCH --wait-all-nodes=1
echo \$SLURM_JOB_NODELIST

srun $homepath/cs553-pa1/network/MyNETBench-UDP $mode $thrd $chunk
EOF
               #Now Make it executable and schedule them
                chmod u+x $homepath/slurms/udp$thrd$mode$chunk.slurm
                for expNo in {1..1}
                do
                sbatch $homepath/slurms/udp$thrd$mode$chunk.slurm
                done
           }
        }
        sleep 1m #So that server scripts can be queued and running
     }


