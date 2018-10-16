#!/bin/bash
# Copyright (c) 2018,caijun
# Filename: checkScreen.sh
# Description: resolve double window
# Author: Cai Jun <johncai.caijun.cn@gmail.com>
# Time: 16:33 tuesday 2018/10/16

# move window
move_window()
{
	window_ip=$1
	move_location=$(xrandr | grep VGA1 | awk '{print $3}' | cut -dx -f1)
	wmctrl -r $window_ip -b remove,fullscreen
	wmctrl -r $window_ip -b remove,maximized_vert
	wmctrl -r $window_ip -e 0,$move_location,0,-1,-1
	wmctrl -r $window_ip -b add,fullscreen
}

geometry_current=$(xdotool getdisplaygeometry | tr " " "x")

window_1=$(ps -ef | grep xfreerdp | grep -v grep | awk '{if($9=="/monitors:0") print $6}' | cut -d: -f2)
window_2=$(ps -ef | grep xfreerdp | grep -v grep | awk '{if($9=="/monitors:1") print $6}' | cut -d: -f2)

geometry_window_1=$(wmctrl -G -l | grep $window_1 | grep -v grep | awk '{print $5"x"$6}')
geometry_window_2=$(wmctrl -G -l | grep $window_2 | grep -v grep | awk '{print $5"x"$6}')


if [ "$geometry_current" == "$geometry_window_1" ]; then
	move_window $window_1
else
	move_window $window_2

fi

/bin/bash /bin/panel_do.sh &
