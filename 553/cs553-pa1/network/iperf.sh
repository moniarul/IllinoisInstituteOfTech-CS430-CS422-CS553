#!/usr/bin/env bash


#get server ip
ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1'

#### SERVER  ------

#TCP
iperf -s --port 1030 --format m --len 1K
iperf -s --port 1030 --format m --len 32K
iperf -s --port 1030 --format m --len 1K --parallel 2
iperf -s --port 1030 --format m --len 32K --parallel 2

#UDP
iperf -s  --port 1030 --format m --len 1K  --parallel 4 --udp



#### CLIENT  ------

#TCP
srun -n 1 -p interactive --pty /bin/bash

iperf -c 172.16.1.4 --port 1030 --format m --len 1K --num 10240M
iperf -c 172.16.1.4 --port 1030 --format m --len 32K --num 10240M
iperf -c 172.16.1.4 --port 1030 --format m --len 1K --num 5120M --parallel 2
iperf -c 172.16.1.4 --port 1030 --format m --len 32K --num 5120M --parallel 2
iperf -c 172.16.1.4 --port 1030 --format m --len 1K --num 2560M --parallel 4
iperf -c 172.16.1.4 --port 1030 --format m --len 32K --num 2560M --parallel 4
iperf -c 172.16.1.4 --port 1030 --format m --len 32K --num 1280M --parallel 8
iperf -c 172.16.1.4 --port 1030 --format m --len 32K --num 1280M --parallel 8

#UDP

iperf -c 172.16.1.4 --port 1030 --format m --len 1K  --udp -i 5 -b 800M

iperf -c 172.16.1.4 --port 1030 --format m --len 1K  --udp -i 5 -b 500M --parallel 2
iperf -c 172.16.1.4 --port 1030 --format m --len 32K  --udp -i 5 -b 1700M --parallel 2


##Pings -

ping -s 1024 172.16.1.4


