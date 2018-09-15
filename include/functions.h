/**********************************************************************   
 * * Copyright (c)2018, caijun                                     
 * * Filename: functions.h
 * * Description：tools                                     
 * * Author：Cai Jun  <johncai.caijun.cn@gmail.com>                
 * * Time: 22:18  2018/9/13 
 * ************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

/*
 * 执行linux命令的函数：
 * 返回执行结果，并保存至strResult
 * */
int execute_cmd(const char* pCommand, std::string& strResult)
{
	FILE *pFstream=NULL;
	if (NULL == ( pFstream = popen(pCommand,"r")))
	{
		printf("Error:%s\n",strerror(errno));
		return -1;
	}
	
	strResult.clear();

	char buff[1024];

	// Memset用来对一段内存空间全部设置为某个字符,一般用在对定义的字符串进行初始化
	memset(buff,0,sizeof(buff));
	while(NULL != fgets(buff, sizeof(buff), pFstream))
	{
		strResult += buff;
	}

	pclose(pFstream);
	return 0;
}


#endif
