#!/bin/bash
# description： shell script color test
# version：0.0.1
# author：Cai Jun  <johncai.caijun.cn@gmail.com>
# date：8:16:30 PM  May 30, 2018

descInfo() {
cat << EOF
description information:
    
	Usage:

		\033[#;3#;4#m STRING \033[0m
		\e[#;3#;4#m STRING \e[0m
    
		#: 加粗、闪烁
		3#: 字体颜色
		4#: 背景颜色
		
        #代表数字:0,1,2,3,4,5,6,7

    注意：多种控制符可组合使用，彼此间用分号隔开，结尾必须是m

---------------------------------------------------------------

EOF
}

charBgColor() {
	local str
	local i
	local first

	first=$1

	case $first in 
	3)
		str="字体"
	;;
	4)
		str="背景"
	;;
	esac

	echo -e "\e[${first}1m${str}颜色：\e[0m \\\e[${first}#m aaaaaaaaaa \\\e[0m\n"
	
	for i in {0..7}; do
		echo -e "$i\t\e[${first}${i}m aaaaaaaaaa \e[0m"
	done
	echo -e "-------------------------\n"

}

statShow() {
	local i

	echo -e "\e[31m字体状态：\e[0m \\\e\[#m aaaaaaaaaa \\\e[0m\n"

	for i in {0..7}; do
		echo -e "${i}\t\e[${i};31m aaaaaaaaaa \e[0m"	
	done
	echo
	echo -e "-------------------------\n"

}

mixCbcSs() {
	local i
	local j
	local k

	echo -e "\e[31m字体颜色、背景颜色、字体状态混合：\e[0m \\\e[#;3#;4#m aaaaaaaaaa \\\e[0m \n[000]分别是字体颜色、背景颜色、状态\n"

	for ((i=0; i<=7;i++)); do
		for ((j=0; j<=7; j++)) do
			for ((k=0; k<=7; k++)); do
				echo -e -n "[${i}${j}${k}]\e[${k};3${i};4${j}m aaaaaaaaaa \e[0m"
			done
			echo -e "---\n"
		done
		echo -e "----------\n"
	done
}


main() {

	clear
	
	descInfo

	charBgColor 3
	charBgColor 4

	statShow

	mixCbcSs
}


main





