#!/bin/bash

monitor=$(xfreerdp /monitor-list | tr "*" " " | awk '{if($2=="1920x1080") {print $1} else {print "0"}}' | head -1)

intranet_ip=$(ps -ef | grep xfreerdp | grep -v grep | awk '{if($9=="/monitors:"'$monitor'"") print $6}' | cut -d: -f2)

intranet_window=$(wmctrl -l | grep FreeRDP | awk '{if($5=="'$intranet_ip'") print $1}')

xdotool windowunmap $intranet_window
xdotool windowmap $intranet_window

wmctrl -r $intranet_ip -b add,fullscreen

/bin/bash /bin/panel_do.sh

