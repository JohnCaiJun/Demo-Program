#!/bin/bash
# description：cp and rm file
# version：0.0.1
# author：Cai Jun  <johncai.caijun.cn@gmail.com>
# date：12:24:27 AM  May 19, 2018

disdir="/backups"

echo $@
[ $# -lt 1 ] && echo -e "Information:\n	rm a file and cp to $disdir , at leaset one file" && exit 2

if ! [ -e $disdir ];then
	sudo mkdir -p $disdir
fi

for file in "$@";do
	if [ -e $file ];then
		sudo cp $file $disdir && rm -rf $file
	fi
done


