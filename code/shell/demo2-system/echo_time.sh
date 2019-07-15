
#!/bin/bash
# description：
# version：0.0.1
# author：Cai Jun  <johncai.caijun.cn@gmail.com>
# date：12:59:14 PM  Jun 10, 2018

. /etc/profile
elinks --dump https://blog.csdn.net/aarron_dl/article/details/80544719 &> /dev/null && echo `date "+%F %T"` >> ~/timeshblog


grepFun() {
	local i

	for ((i=0; i<3; i++)); do
	
		elinks --dump https://blog.csdn.net/aarron_dl/article/details/80617241 &> /dev/null && echo `date "+%F %T"` >> ~/timegrep
		sleep 20
		let i++
	done
}
grepFun
