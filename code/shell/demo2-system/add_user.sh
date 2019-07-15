#!/bin/bash
# description：add a user (can set default shell)
# version：0.0.2
# author：Cai Jun  <johncai.caijun.cn@gmail.com>
# date：7:06:31 PM  Jun 3, 2018
addUser() {
	local num
	local user_name
	local default_shell

	num=$#
	user_name=$1
	default_shell=$2


	case $num in 
	0)
		echo "please input an userName"
		break ;;
	1)
		id $user_name &> /dev/null && userdel -r $user_name
		useradd $user_name ;;
	2)
		id $user_name &> /dev/null && userdel -r $user_name
		if ! grep "$default_shell" /etc/shells &> /dev/null; then
			echo -e "\n\terror info : \n\t\t$default_shell not exited in /etc/shells , you can't use it. so , use default shell /bin/bash\n"
			default_shell=/bin/bash
		fi
		useradd -s $default_shell $user_name  ;;
	esac
}

menu() {

cat << EOF

Usage:
	Add a user：user1 [shell]

	the seconed is user's shell, this default is /bin/bash

	exit ：use q or Q 
-----------------------------------------------------------

EOF
}

main () {
	local add_info
	local user_name
	local default_shell
	
	menu

	while true; do
		
		read -p "Add a user：" add_info

		user_name=`echo $add_info | awk '{print $1}'`
		default_shell=`echo $add_info | awk '{print $2}'`

		case $user_name in 
		null|q|Q)
			echo "exit"
			exit 0 ;;
		*)
			addUser $user_name $default_shell;;
		esac
	done
}

main
