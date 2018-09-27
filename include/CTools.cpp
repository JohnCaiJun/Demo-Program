/**********************************************************************   
 * * Copyright (c)2018, caijun
 * * Filename: CTools.cpp
 * * Description：工具类实现    
 * * Author：Cai Jun  <johncai.caijun.cn@gmail.com> 
 * * Time: 4:17 PM Aug 24, 2018 
 * ************************************************************************/ 

#ifndef FILE_TOOLES_CPP
#define FILE_TOOLES_CPP

#include <iostream>
#include <fstream>
#include <pwd.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <net/if.h> 
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <dirent.h>
#include <vector>
#include <getopt.h>
#include <algorithm>
#include "CTools.h"
using namespace std;


// 初始化参数值
Tool::Tool(){
	m_tCmdArgs.fileName.clear();
	m_tCmdArgs.user.clear();
	m_tCmdArgs.host.clear();
	m_tCmdArgs.appCommand.clear();
	m_tCmdArgs.port="22";
	m_tCmdArgs.passwd.clear();
	m_tCmdArgs.resolution="800x600";
	m_tCmdArgs.xinerama="no";
	m_tCmdArgs.monitor="null";
}

// 獲取家用戶目錄
string Tool::getHomeDir() {
	passwd *pw = getpwuid(getuid());
	string path(pw->pw_dir);
	return path;
} 

// 獲取當前目錄
string Tool::getPwd(){
	char* buffer;
	if((buffer = getcwd(NULL, 0)) == NULL){
		cout << "get current directory failed!" << endl;
		exit(-1);
	}else{
		return string(buffer);
	}
}

// 字符串分割函數
vector<string> Tool::split(string str, string pattern) {
	string::size_type pos;
	vector<string> result;
	str+=pattern;    // 在字符串最後追加一個分割符，方便統一處理
	int size = str.size();
	for (int i = 0; i < size; i++) {
		pos = str.find(pattern,i);
		if ( pos < size) {
			string s = str.substr(i,pos-i);
			result.push_back(s);
			i = pos+pattern.size()-1;
			
		}
	}
	return result;
}

// 杀死占用端口的进程
void Tool::kprocess(const string port){
	string cmd = string("lsof -i:")+port+
					string("| sed -n \"2,1p\" | awk \'{print $2}\'");
	string result;
	result.clear();
	execute_cmd(cmd.c_str(),result);

	string kcmd = string("kill ")+result;
	if (result.size() > 0) {
		execute_cmd(kcmd.c_str());
	}
}


// 將resumingsession信息寫入文件
void Tool::writeResumingsessionFile(x2goSession &resume_session){
	string homeDir = getHomeDir();
	string nxroot = homeDir + "/.x2go";
	
	if (access(nxroot.c_str(),0) == -1){
		#ifndef NDEBUG
		cout << nxroot <<" not exits!" << endl;
		cout << "now make it" << endl;
		#endif

		int flag = mkdir(nxroot.c_str(),0777);
	
		if (flag == 0){
			#ifndef NDEBUG
			cout << "make"<<nxroot<<" successfully" << endl;
			#endif
		} else {
			#ifndef NDEBUG
			cout << "make errorly" << endl;
			#endif
			exit(-1);
		}
	}
	
	string dirpath = nxroot + "/S-"+resume_session.sessionId;
	
	if (access(dirpath.c_str(),0) == -1){
		#ifndef NDEBUG
		cout << dirpath <<"is not exits!" << endl;
		cout << "now make it" << endl;
		#endif

		int flag = mkdir(dirpath.c_str(),0777);
	
		if (flag == 0){
		#ifndef NDEBUG
			cout << "make successfully" << endl;
		#endif
		} else {
			#ifndef NDEBUG
			cout << "make errorly" << endl;
			#endif
			exit(-1);
		}
	
	}
	
	int local_grport = atoi(resume_session.grPort.c_str()) + 1000;	
	string localGrPort =to_string(local_grport);	

	#ifndef NDEBUG
	cout << "监听本地图形界面的端口：" << localGrPort << endl;
	#endif

	kprocess(localGrPort);
	
	ofstream outfile;
	string filename = dirpath + "/options";
	outfile.open(filename.c_str());
	
	outfile << "nx/nx, root=" <<nxroot
				<<",connect=localhost,cookie="
				<<resume_session.cookie
				<<",port="<<localGrPort<<",errors="
				<<dirpath<<"/session:"
				<<resume_session.display;
	outfile.close();

}

// 保存resumieSession信息
void Tool::setResumingSession(char* buffer){
	string str = buffer;
	string pattern = "|";
	vector<string> result = split(str,pattern);
	#ifndef NDEBUG
	for ( int i = 0; i < result.size(); i++ ) {
		cout << "result["<< i << "]"<< result[i] << endl;
	}
	#endif

	// 因爲格式確定，所有直接用下標可以獲取相關信息（只需取消上面for循環註釋即可查看相關下標對應信息）
	resume_session.sessionId = result[1];
	resume_session.grPort = result[8];
	resume_session.cookie = result[6];
	resume_session.display = result[2];

	#ifndef NDEBUG
	cout << "set resume_session --> display: " 
		 << resume_session.display<< endl;
	#endif

	// 寫入.x2go目錄下的文件
	writeResumingsessionFile(resume_session);
}

// baocun startagentsessionxinxi
void Tool::setStartAgentSession(vector<string> result) {
	resume_session.pid = result[3];
	resume_session.sessionId = result[4];
	resume_session.grPort = result[5];
	resume_session.sndPort = result[6];
	resume_session.cookie = result[2];
	resume_session.display = result[1];
	#ifndef NDEBUG
	cout << "-display: " << resume_session.display<< endl;
	#endif
	// 寫入.x2go目錄下的文件
	writeResumingsessionFile(resume_session);
}

void Tool::setResumingSession(string buffer){
	string str = buffer;
	string pattern = "|";
	vector<string> result = split(str,pattern);
//	for ( int i = 0; i < result.size(); i++ ) {
//		cout << "result["<< i << "]"<< result[i] << endl;
//	}

	// 因爲格式確定，所有直接用下標可以獲取相關信息（只需取消上面for循環註釋即可查看相關下標對應信息）
	resume_session.sessionId = result[1];
	resume_session.grPort = result[8];
	resume_session.cookie = result[6];
	resume_session.display = result[2];
	cout << "-display: " << resume_session.display<< endl;

	// 寫入.x2go目錄下的文件
	writeResumingsessionFile(resume_session);
}

// 获取当前用户的用户名 
string Tool::getUserName()
{
    uid_t userid;
	struct passwd* pwd;
	userid=getuid();
	pwd=getpwuid(userid);
	return pwd->pw_name;
}

// 显示帮助菜单
void Tool::displayHelp(){
	cout << "Usage: tlrd-client [options] -v<server> -u<username> -p<passwd> --desktop=<cinnamon-session>"<<endl;
	cout << "常用的有：											\n\
-u<username>\t\tUsername						\n\
-p<password>\t\tPassword						\n\
-v<server>  \t\tremote server address			\n\
-f		     \t\tFull screen mode				\n\
--app=<program> 												\n\
--desktop=<desktop-session> Desktop name					\n\
--auto-reconnect            Enable Automatic reconnection   \n\
--fonts                     Enable smooth fonts				\n\
--monitors=<id>             Select monitors to use			\n\
--multimon                  Use multiple monitors			\n\
--port=<number>             Server port						\n\
--help						help information					\n\
--sound                     Audio output					\n\
-v<server> 	          Server hostname					\n\
-h							help information"<<endl;
}
	

// 处理命令行选项和参数
void Tool::getOptLong(int argc, char* argv[], string opts){
	int OPT;
	int digit_optind = 0;
	int option_index = 0;
	char *string =(char*) opts.c_str();
	static struct option long_options[] =
	{  
		{"help", no_argument,NULL, HELP},
		{"app", required_argument,NULL, APP},
		{"auto-reconnect", no_argument,NULL, AUTOCONNECT},
		{"desktop", required_argument,NULL, DESKTOP},
		{"fonts", required_argument,NULL, FONTS},
		{"monitors", required_argument,NULL, MONITORS},
		{"multimon", no_argument,NULL, MULTIMON},
		{"port", optional_argument,NULL, PORT},
		{"sound", required_argument,NULL, SOUND},
		{NULL,     0,                      NULL, 0},
	}; 

	while((OPT =getopt_long_only(argc,argv,string,long_options,&option_index))!= -1)
	{  
		#ifndef NDEBUG
		printf("opt = %c\t\t", OPT);
		printf("optarg = %s\t\t",optarg);
		printf("optind = %d\t\t",optind);
		printf("argv[optind] =%s\t\t", argv[optind]);
		printf("option_index = %d\n",option_index);
		#endif
		
		switch(OPT){
			//处理短选项
			case 'h':
				displayHelp();
				#ifndef NDEBUG
				printf("option=m, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case 'p':
				m_tCmdArgs.passwd = optarg;		
				#ifndef NDEBUG
				cout<< "\noption=p,m_tCmdArgs.passwd="<<m_tCmdArgs.passwd<<endl;
				printf("option=p, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case 'u':
				m_tCmdArgs.user = optarg;
				#ifndef NDEBUG
				cout<<"\noption=u,m_tCmdArgs.user="<<m_tCmdArgs.user<<endl;
				printf("option=u, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case 'f':
				m_tCmdArgs.resolution = "fullscreen";
				#ifndef NDEBUG
				cout<<"\noption=u,m_tCmdArgs.user="<<m_tCmdArgs.user<<endl;
				printf("option=u, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case 'v':
				m_tCmdArgs.host = optarg;
				#ifndef NDEBUG
				cout << "\noption=h,m_tCmdArgs.host="<<m_tCmdArgs.host<<endl;
				printf("option=s, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;

			// 处理长选项
			case HELP:
				displayHelp();
				break;
			case APP:
				m_tCmdArgs.appCommand = optarg;
				#ifndef NDEBUG
				printf("option=c, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case DESKTOP:
				m_tCmdArgs.appCommand = optarg;
				#ifndef NDEBUG
				printf("option=c, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case MULTIMON:
				m_tCmdArgs.resolution = "fullscreen";
				m_tCmdArgs.xinerama = "yes";
				#ifndef NDEBUG
				printf("option=c, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case PORT:
				m_tCmdArgs.port = optarg;		
				#ifndef NDEBUG
				printf("option=c, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case MONITORS:
				m_tCmdArgs.monitor.clear();
				m_tCmdArgs.monitor = optarg;		
				#ifndef NDEBUG
				printf("option=c, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			case '?':
				#ifndef NDEBUG
				printf("result=?, optopt=%c, optarg=%s\n", optopt, optarg);
				#endif
				break;
			default:
				#ifndef NDEBUG
				printf("default, result=%c\n",OPT);
				cout << "\n没有此选项：" <<endl;
				#endif
				break;
		}

	} 
	
	#ifndef NDEBUG
	//看看最后的命令行参数，看顺序是否改变了哈。
	for(OPT = 1; OPT < argc; OPT++)
		printf("\nat the end-----argv[%d]=%s\n", OPT, argv[OPT]);
	#endif

}

// 只执行linux命令，不返回信息结果
int Tool::execute_cmd(const char* pCommand) {
    FILE *pFstream=NULL;         
    if(NULL==(pFstream = popen(pCommand,"r")))       
    {
	//	OutLogFile("execute command failed: %s",strerror(errno));
		printf("execute command failed\n");
		return -1;
    }

    pclose(pFstream);
	return 0;
}

// 执行linux命令并获取返回信息
int Tool::execute_cmd(const char* pCommand, string& strResult)
{
    FILE *pFstream=NULL;         
    if(NULL==(pFstream = popen(pCommand,"r")))       
    {
	//	OutLogFile("execute command failed: %s",strerror(errno));
		printf("execute command failed\n");
        return -1;       
    }

	strResult.clear();
    
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    while(NULL != fgets(buff , sizeof(buff) , pFstream))
    {
		strResult += buff;
    }
     
    pclose(pFstream);
    return 0;
}

// 获取扩展屏幕分辨率
string Tool::getScrennResolution(){
	string str = "xrandr | awk \'{if($2 == \"connected\" && $3 != \"primary\") print $3}\'";
	string result = "";
	int i = execute_cmd(str.c_str(),result);
	if ( i != 0 ){
		cout << "linux命令执行错误！没有获取到主机屏幕分辨率"<<endl;	
		exit(-1);
	}
	result = result.substr(0,result.find_first_of("+"));
	#ifndef NDEBUG
	cout << "get substr = "<< result<<endl;
	#endif
	return result; 
}

// 获取当前主机屏幕分辨率
string Tool::getMainResolution(){
	string str = "xrandr | awk \'{if($2 == \"connected\" && $3 == \"primary\") print $4}\'";
	string result = "";
	int i = execute_cmd(str.c_str(),result);
	if ( i != 0 ){
		cout << "linux命令执行错误！没有获取到主机屏幕分辨率"<<endl;	
		exit(-1);
	}
	result = result.substr(0,result.find_first_of("+"));
	#ifndef NDEBUG
	cout << "get substr = "<< result<<endl;
	#endif
	return result; 
}

// 指定屏幕显示
void Tool::choiceMonitor(const string& monitor){
	string result;
	result.clear();
	string str1 ="xdotool search \"x2go\"";
	execute_cmd(str1.c_str(),result);        						// 获取窗口句柄
	int j = result.find_first_of("\n");								// 获取横坐标
	result = result.substr(0,j);
	string str2 = "xdotool windowactivate --sync "+result;
	#ifndef NDEBUG
	cout << "\n\n获取窗口句柄："<<result<<endl;
	#endif
	execute_cmd(str2.c_str()); 										// 使该窗口在最上层   
	if (monitor == "0"){
		string str3 = string("xdotool windowmove --sync ")+result+string(" 0 0");
		#ifndef NDEBUG
		cout << "\n\n主屏0:" << str3 <<endl;
		#endif
		execute_cmd(str3.c_str());  								// 移到当前主屏	
	} else if (monitor == "1"){
		string resolution = getMainResolution();        			// 获取当前主屏幕的分辨率
		int i = resolution.find_first_of("x");						// 获取横坐标
		resolution = resolution.substr(0,i);
		int num = atoi(resolution.c_str());
		++num;
		#ifndef NDEBUG
		cout<<"\n\nresolution="<<num<<endl;
		#endif
		resolution = to_string(num);
		string str4 = "xdotool windowmove --sync "+result+" "+resolution+" 0"; 
		#ifndef NDEBUG	
		cout<< "\n\n扩展屏1："<< str4 << endl;
		#endif
		execute_cmd(str4.c_str());   								// 移到扩展屏
	} else {
		cerr<<"no this monitor"<<endl;
		exit(-1);
	}
}

// 封装锁 
CMutex::CMutex(){
	mutex = PTHREAD_MUTEX_INITIALIZER;
}

void CMutex::Lock(){
	pthread_mutex_lock(&mutex);
}

void CMutex::Unlock(){
	pthread_mutex_unlock(&mutex);
}

#endif
