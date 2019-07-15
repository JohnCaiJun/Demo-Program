#!/bin/bash
# description：
# version：0.0.1
# author：Cai Jun  <johncai.caijun.cn@gmail.com>
# date：11:33:38 PM  May 27, 2018


mainMenu(){

cat << EOF

--------------------------------
User tools:

aduser ) add user 

---------------------------------
Script information:

shco ) list color in bash script

---------------------------------
Useful tools:

cpco ) copy command and lib to new root

---------------------------------
system infomation:                
				  
cpu  ))  display cpu information  
				  
mem  )) display memory infomation 
			 	  
disk )) display disks information 

---------------------------------			  
q    )) quit			  
---------------------------------
EOF

}

# 按任意键继续执行脚本
getChar() 
{ 
	SAVEDSTTY=`stty -g` 
	stty -echo 
	stty cbreak 
	dd if=/dev/tty bs=1 count=1 2> /dev/null 
	stty -raw 
	stty echo 
	stty $SAVEDSTTY 
}

cpuInfo() {
	clear	
	echo "cpu information:"
	echo -e "--------------------------\n"
	lscpu
}

memInfo() {
	clear
	echo "mem information:"
	echo -e "--------------------------\n"
	free -m
}

diskInfo() {
	clear
	echo "disk information"
	echo -e "--------------------------\n"
	fdisk -l /dev/[sv]d[a-z]
}

exitInfo() {
	clear
	echo "succeed quit"
	echo -e "\n--------------------------\n"
	exit 0
}

errorInfo() {
	local i
	clear
	echo -e "\nError information:\n"
	echo -e "\t 不存在的选项，按任意键继续"
	for ((i=0; i<=6; i++)); do
		echo "."
		sleep 1
	done
}

main() {
	local option

	while true;do
		clear

		#主菜单
		mainMenu

		#输入表单选项
		read -p "Enter your option: " option

		option=$(echo $option | tr 'A-Z' 'a-z')

		case $option in 
			aduser)
				./add_user.sh ;;
			shco)
				./color_script.sh 
				char=`getChar` ;;
			cpco)
				./copy_command.sh ;;
			cpu)
				cpuInfo	
				char=`getChar` ;;
			mem)
				memInfo	
				char=`getChar` ;;
			disk)
				diskInfo 
				char=`getChar` ;;
			q|Q|quit)
				exitInfo ;;
			*)
				errorInfo 
				char=`getChar` ;;
		esac

	done
}

main
