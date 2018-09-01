## Instructions to execute Benchmarks

> If any of the following bash script gives permission error try `chmod u+x <scriptname>.sh to make it executable.

### CPU benchmarks

- 1) cd into cs553-pa1 folder.
- 2) ./cpu_engine.sh
- 3)  *Linpack* - It's run manually . executed commands are stored in `/cs553-pa1/cpu/linpack.sh`. Just stored the commands serially.
- 4) Out put willn be saved by the programme at `/exports/home/schatterjee/reports/report-cpu.txt`.

> Note - This script will create a slurm file at `/exports/home/schatterjee/slurms/cpu.slurm` and will batch it. At the top of the bash file we have the
code to compile the file first and it also tries to create the necessary folders if missing.If slurm script results in error 
/ Or program console prints anything it's stored on `/exports/home/schatterjee/slurms/cpu.out`

### Memory Benchmarks

- 1) cd into cs553-pa1 folder.
- 2) ./memory_engine.sh
- 3) *pmbw* : It's run manually . From it's output specific lines are copied and kept in `cs553-pa1/memory/pmbw.txt`. 
Also the commands to run pmbw is kept in cs553-pa1/memory/pmbw.sh
- 4) Output will be saved by the programme at `/exports/home/schatterjee/reports/report-memory.txt`.

> Note - This script will create a slurm file at `/exports/home/schatterjee/slurms/memory.slurm` and will batch it.At the top of the bash file we have the
code to compile the file first and it also tries to create the necessary folders if missing.If slurm script results in error 
/ Or program console prints anything it's stored on `/exports/home/schatterjee/slurms/memory.out`

### Disk Benchmarks

- 1) cd into cs553-pa1 folder.
- 2) ./disk_engine.sh
- 3) *iozone* : It's run manually . Commands to run pmbw is kept in cs553-pa1/disk/iozone.sh
- 4) Output will be saved by the programme at `/exports/home/schatterjee/reports/report-disk.txt`.

> Note - This script will create 9 slurm files at `/exports/home/schatterjee/slurms/<diskSpecificInputs>.slurm` and will batch them.At the top of the bash file we have the
code to compile the file first and it also tries to create the necessary folders if missing.If slurm script results in error 
/ Or program console prints anything it's stored on `/exports/home/schatterjee/slurms/<DiskSpecificFileName>.out`

### -Network TCP

- 1) cd into cs553-pa1 folder.
- 2) ./net_engine.sh 1000 

Note - (here 1000 signifies BlockSize as per assignment . 1KB ~ 1000. It should create 8 different slurm
files inside `/exports/home/schatterjee/slurms` folder with different params . It also deletes and re-creates the folder if already existing.
This script first submit 4 jobs as you can see in script output . sleeps for 2minutes as those 4 jobs are server programs
and we don't want to run client programs until servers are queued and running ) Then after 2 minutes it queues remaining
4 client jobs.

- 3) ./net_engine.sh 32000

Note - (here 32000 signifies BlockSize as per assignment . 32KB ~ 32000. It should create 8 different slurm files and
schedule them as above).

> why 16 different slurm jobs ?

We need to perform 16 different combination of tests for Network , i.e. 
`2 modes (server & client) * 4 different concurrency * 2 different block size`.
Each slurm file represent each combination of parameters and schedule them .

- 4) Reports are kept inside `/exports/home/schatterjee/slurms/<inputcombinationname>.out` files. 
Also `/exports/home/schatterjee/reports/net-report.txt` has the combined report.

- 5) Server and Client communicate ips using specifically named files kept inside /exports/home/schatterjee/ips


## -Network UDP

- 1) cd into cs553-pa1 folder.
- 2) ./udp_engine.sh 1000 

Note - (here 1000 signifies BlockSize as per assignment . 1KB ~ 1000. It should create 8 different slurm
files inside /exports/home/schatterjee/slurms folder with different params . It also deletes and re-creates the folder if already existing.
This script first submit 4 jobs as you can see in script output . sleeps for 2minutes as those 4 jobs are server programs
and we don't want to run client programs until servers are queued and running ) Then after 2 minutes it queues remaining
4 client jobs.

- 3) ./udp_engine.sh 32000

Note - (here 32000 signifies BlockSize as per assignemnt . 32KB ~ 32000. It should create 8 different slurm files and
schedule them as above).

> why 16 different slurm jobs ?

We need to perform 16 different combination of tests for Network , i.e. 2 modes (server & client) * 4 different concurrency * 2 different block size.
Each slurm file represent each combination of parameters and schedule them .

- 4) Reports are kept inside `/exports/home/schatterjee/slurms/<inputcombinationname>.out` files. 
Also /exports/home/schatterjee/reports/net-report has the combined report.

- 5) Server and Client communicate ips using a specifically named files kept inside /exports/home/schatterjee/ips

### -Network Latency Pingpong

I created a separate folder and file (`/Users/diesel/Documents/cs553-pa1/pingpong/Pingpong.cpp`)for latency measurement 
as the existing code was becoming too big and hard to follow.

- 1) ./pingpiong_engine.sh 1

Note - Here 1 suggest isTcp true. So it will create 1 slurm file and batch it. The program will auto-run for all 
different kind of inputs and use TCP protocol.

- 2) ./pingpiong_engine.sh 0

Note - Here 0 suggest isTcp false. So it will create 1 slurm file and batch it. The program will auto-run for all 
different kind of inputs and use UDP protocol.

- 3) Outputs will be kept at - `/exports/home/schatterjee/reports/report-pingpong.txt`

 




