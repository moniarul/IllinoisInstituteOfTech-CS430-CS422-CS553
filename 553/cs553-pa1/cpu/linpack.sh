#!/usr/bin/env bash
srun -n 1 -p interactive --pty /bin/bash
cd /exports/home/schatterjee

wget  "http://registrationcenter-download.intel.com/akdlm/irc_nas/9752/l_mklb_p_2018.1.009.tgz"

tar xvf l_mklb_p_2018.1.009.tgz

/exports/home/schatterjee/l_mklb_p_2018.1.009/benchmarks_2018/linux/mkl/benchmarks/linpack/xlinpack_xeon64 /exports/home/schatterjee/l_mklb_p_2018.1.009/benchmarks_2018/linux/mkl/benchmarks/linpack/lininput_xeon64

/exports/home/schatterjee/l_mklb_p_2018.1.009/benchmarks_2018/linux/mkl/benchmarks/linpack/xlinpack_xeon64 /exports/home/schatterjee/l_mklb_p_2018.1.009/benchmarks_2018/linux/mkl/benchmarks/linpack/mod
