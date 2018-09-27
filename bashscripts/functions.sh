#!/bin/bash
# Copyright (c)2018,caijun
# Filename: functions.sh
# Description: 一些脚本工具
# Author: Cai Jun <johncai.caijun.cn@gmail.com>
# Time: 13:38 星期三 2018/9/26 

# 添加环境变量路径
# 用法：prepend PATH /usr/lib
prepend() 
{
	[ -d "$2" ] && eval $1=\"$2\$\{$1:+':'\$$1\}\" && export $1; 
}

# 删除原始文件，同时在backup目录中保留副本
setrmbakup()
{
	if [ ! -d ~/backup ];then
		mkdir ~/backup
	fi
	
	alias rm='cp -ra $@ ~/backup && rm -rf $@'
}









