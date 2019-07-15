#!/bin/bash
# description：copy command and dependencies lib to new root
# version：0.0.1
# author：Cai Jun  <johncai.caijun.cn@gmail.com>
# date：1:41:43 AM  May 31, 2018


usageInfo() {

cat << EOF

Usage : 


 eg. Input a command : bash 
      
	note : if command is quit or q, then return.

------------------------------------------------------------

EOF
}



#命令存在则复制到新的根目录下
cpComLib() {
	local command
	local basedir
	local adddir
	local base_name
	local maindir
	local list_ldd
	local lddbasedir
	local lddmaindir

	command=$1

	#new root
	basedir=$2

	! [ -d $basedir ] && mkdir -p $basedir
	
	! type $command &> /dev/null && echo "$command is not existed!!" && continue

	command=$(whereis $command | cut -d" " -f2)

	#old root
	adddir=$(dirname $command)
	base_name=$(basename $command)

	#new dir
	maindir="${basedir}${adddir}"

	! [ -d "$maindir" ] && mkdir -p $maindir 

	! cp -r $command $maindir &> /dev/null && echo "copy $command error!!" && continue

	list_ldd=$(ldd `whereis $base_name` 2> /dev/null | grep -o "/.*[[:space:]]")

	for dir_and_name in $list_ldd; do
		lddbasedir=$(dirname $dir_and_name)
		lddmaindir=${basedir}${lddbasedir}
		! [ -d $lddmaindir ] && mkdir -p $lddmaindir
		! cp $dir_and_name $lddmaindir && echo "copy dependencies lib error!!" && break
	done 
}


basedir=/mnt/newroot

#复制
main(){
	local command

	clear

	usageInfo

	while true; do
		read -p "command:" command
	
		case $command in
		quit|q|Q)
			break ;;
		*)
			cpComLib $command $basedir ;;
		esac
	done
}

main



