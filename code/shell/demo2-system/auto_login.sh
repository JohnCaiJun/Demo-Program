#!/bin/bash
# Author: Cai Jun
# Date: 2018-12-08 星期六

function strTrArr()
{
	local url=$1
	for index in `seq 0 $((${#url}-1))`
	do
		arr[$index]=${url:$index:1}	
	done
	echo "${arr[@]}"
}

function inputDelay()
{
	array=$@
	for i in ${array[@]}
	do
		sleep 0.16
		xdotool type "$i"
	done
}

url_git="www.github.com"
url_git=$(strTrArr $url_git)
echo "url_git=${url_git}"
sleep 1

/opt/google/chrome/chrome --user-data-dir &
window=$(wmctrl -l -G -p | grep -i chrome | awk '{print $1}')
xdotool windowfocus $window 
wmctrl -r chrome -b add,fullscreen
xdotool key ctrl+l 
sleep 1.4

inputDelay ${url_git}
xdotool key "Return"

xdotool mousemove 2697 120

sleep 1.8

xdotool click 1


user_name="JohnCaiJun"
user_name=$(strTrArr $user_name)
echo "user_name=${user_name}"
sleep 1

inputDelay $user_name

xdotool key "Tab"

passwd="cj-cn-lcc-8"
passwd=$(strTrArr $passwd)
echo "passwd=${passwd}"

sleep 1

inputDelay $passwd


xdotool type "
"


xdotool mousemove 2308 435 

sleep 1

xdotool click 1
