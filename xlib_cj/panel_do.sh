#!/bin/bash
# Copyright (c) 2018, Caijun
# Filename: panel_do.sh
# Description: panel handle
# Author: Cai Jun <johncai.caijun.cn@gnail.com>
# Time: 12:37 sunday 208/10/14

while [ "1" = "1" ]
do
	sleep 2
	ps -ef | grep freerdp | grep -v grep > /dev/null
	freerdp_flag=$?
	ps -ef | grep xfce4-panel | grep -v grep > /dev/null
	panel_flag=$?
	if [ "$freerdp_flag" -eq "0" -a "$panel_flag" -eq "0" ]; then
		xfce4-panel -q
	elif [ "$freerdp_flag" -eq "0" -a "$panel_flag" -ne "0" ]; then
		continue
	elif [ "$freerdp_flag" -ne "0" -a "$panel_flag" -eq "0" ]; then
		continue
	elif [ "$freerdp_flag" -ne "0" -a "$panel_flag" -ne "0" ]; then
		xfce4-panel &
	fi
done	
