#!/usr/bin/env bash
#Path Where you want to create slurm & reportfiles..
homepath=/exports/home/schatterjee
mkdir -p $homepath/slurms
mkdir -p $homepath/reports

chmod 777 $homepath/slurms
chmod 777 $homepath/reports

g++ -std=gnu++14 -O3 -pthread -Wall -o $homepath/cs553-pa1/disk/MyDiskBench $homepath/cs553-pa1/disk/MyDiskBench.cpp

echo -e "\n\n #######-----   Time: $(date). New Execution ------####### \n \n " >> $homepath/reports/report-disk.txt


echo "Folder /slurms and /reports created.."
echo "Running Disk tasks"

declare -a threads=("4" "2" "1")
declare -a blockSize=("1000000" "10000000" "100000000")

for thrd in "${threads[@]}"
{

for block in "${blockSize[@]}"
{

cat <<EOF >$homepath/slurms/disk$thrd$block.slurm
#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=$homepath/slurms/disk$thrd$block.out
#SBATCH --wait-all-nodes=1
echo \$SLURM_JOB_NODELIST

srun $homepath/cs553-pa1/disk/MyDiskBench $thrd $block
EOF

#Now Make it executable and schedule them
chmod u+x $homepath/slurms/disk$thrd$block.slurm

sbatch $homepath/slurms/disk$thrd$block.slurm
}
}
