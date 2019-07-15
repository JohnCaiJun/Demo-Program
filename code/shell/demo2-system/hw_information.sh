#!/bin/bash

echo "核心数以及线程数"
cat /proc/cpuinfo | grep name | cut -d: -f2 | uniq -c

echo "cpu物理数"
cat /proc/cpuinfo | grep "physical id" | uniq -c

echo "物理内存"
free -m | awk '{if($1=="Mem:")print $2}'

echo "显卡信息"
dis_num=$(lspci | grep -i vga | cut -d" " -f1)
lspci -v -s $dis_num | grep -i vga

echo "显存"
lspci -v -s $dis_num | grep -i prefetchable | cut -d"=" -f2 | tr "=]" " "

