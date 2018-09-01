#!/usr/bin/env bash
#Path Where you want to create slurm files..
homepath=/exports/home/schatterjee
mkdir -p $homepath/slurms
mkdir -p $homepath/reports

chmod 777 $homepath/slurms
chmod 777 $homepath/reports

g++ -std=gnu++14 -O3  -pthread -Wall -mavx2 -mfma -o $homepath/cs553-pa1/cpu/MyCPUBench $homepath/cs553-pa1/cpu/MyCPUBench.cpp

echo "Folder /slurms and /reports created.."
echo "Running cpu tasks"

echo -e "\n\n #######-----   Time: $(date). New Execution ------####### \n \n " >> $homepath/reports/report-cpu.txt

cat <<EOF >$homepath/slurms/cpu.slurm
#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=$homepath/slurms/cpu.out
#SBATCH --wait-all-nodes=1
echo \$SLURM_JOB_NODELIST

srun $homepath/cs553-pa1/cpu/MyCPUBench
EOF

#Now Make it executable and schedule them
chmod u+x $homepath/slurms/cpu.slurm

sbatch $homepath/slurms/cpu.slurm



