#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=/exports/home/schatterjee/cs553-pa2a/linsort2gb.log
#SBATCH --wait-all-nodes=1
chmod u+x /exports/home/schatterjee/cs553-pa2a/linsort2gb.sh
srun /exports/home/schatterjee/cs553-pa2a/linsort2gb.sh

