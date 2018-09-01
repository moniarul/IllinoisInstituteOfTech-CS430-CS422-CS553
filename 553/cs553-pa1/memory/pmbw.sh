#!/usr/bin/env bash
srun -n 1 -p interactive --pty /bin/bash
cd /exports/home/schatterjee
wget https://panthema.net/2013/pmbw/pmbw-0.6.2-linux_x86-64.tar.bz2
tar xvf pmbw-0.6.2-linux_x86-64.tar.bz2
cd ./pmbw-0.6.2
nice -n -19 ./pmbw -S 0