#!/usr/bin/env bash
#Path Where you want to create slurm files..
homepath=/exports/home/schatterjee
mkdir -p $homepath/slurms
mkdir -p $homepath/reports

chmod 777 $homepath/slurms
chmod 777 $homepath/reports

g++ -std=gnu++14 -O3  -pthread -Wall -o $homepath/cs553-pa1/memory/MyRAMBench $homepath/cs553-pa1/memory/MyRAMBench.cpp

echo -e "\n\n #######-----   Time: $(date). New Execution ------####### \n \n " >> $homepath/reports/report-memory.txt

echo "Folder /slurms and /reports created.."
echo "Running Memory tasks"

cat <<EOF >$homepath/slurms/memory.slurm
#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=$homepath/slurms/memory.out
#SBATCH --wait-all-nodes=1
echo \$SLURM_JOB_NODELIST

srun $homepath/cs553-pa1/memory/MyRAMBench
EOF

#Now Make it executable and schedule them
chmod u+x $homepath/slurms/memory.slurm

sbatch $homepath/slurms/memory.slurm
