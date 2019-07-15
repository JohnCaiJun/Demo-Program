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

using namespace std;


// 獲取家用戶目錄
string getHomeDir() {
	passwd *pw = getpwuid(getuid());
	string path(pw->pw_dir);
	return path;
} 

// 獲取當前目錄
string getPwd(){
	char* buffer;
	if((buffer = getcwd(NULL, 0)) == NULL){
		cout << "get current directory failed!" << endl;
		exit(-1);
	}else{
		return string(buffer);
	}
}

// 字符串分割函數
vector<string> split(string str, string pattern) {
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
void kprocess(const string port){
	string cmd = string("lsof -i:")+port+string("| sed -n \"2,1p\" | awk \'{print $2}\'");
	string result;
	result.clear();
	execute_cmd(cmd.c_str(),result);

	string kcmd = string("kill ")+result;
	if (result.size() > 0) {
		execute_cmd(kcmd.c_str());
	}
}

// 只执行linux命令，不返回信息结果
int execute_cmd(const char* pCommand) {
	FILE *pFstream=NULL;    
	if(NULL==(pFstream = popen(pCommand,"r")))    
	{   
		printf("execute command failed\n");
		return -1; 
	}   

 	pclose(pFstream);
	return 0;
}

// 执行linux命令的函数：返回执行结果，并保存至strResult
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

// enum枚举搭配switch做参数选择器
enum OPT
{	
	HELP,
	APP,
	AUTOCONNECT,
	DESKTOP,
	FONTS,
	MONITORS,
	MULTIMON,
	PORT,
	SOUND,
};
	
/*
string optstr = "ab:c::"

单个字符a         表示选项a没有参数            格式：-a即可，不加参数
单字符加冒号b:     表示选项b有且必须加参数      格式：-b 100或-b100,但-b=100错
单字符加2冒号c::   表示选项c可以有，也可以无     格式：-c200，其它格式错误
*/
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


// 获取扩展屏幕分辨率
string getScrennResolution()
{
	string str = "xrandr | awk \'{if($2 == \"connected\" && $3 != \"primary\") print $3}\'";
	string result = "";
	int i = execute_cmd(str.c_str(),result);
	if ( i != 0 ){
		cout << "linux命令执行错误！没有获取到主机屏幕分辨率"<<endl;	
		exit(-1);
	}
	result = result.substr(0,result.find_first_of("+"));
	cout << "get substr = "<< result<<endl;
	return result; 
}

// 获取当前主机屏幕分辨率
string getMainResolution(){
	string str = "xrandr | awk \'{if($2 == \"connected\" && $3 == \"primary\") print $4}\'";
	string result = "";
	int i = execute_cmd(str.c_str(),result);
	if ( i != 0 ){
		cout << "linux命令执行错误！没有获取到主机屏幕分辨率"<<endl;	
		exit(-1);
	}
	result = result.substr(0,result.find_first_of("+"));
	cout << "get substr = "<< result<<endl;
	return result; 
}

// 读文件 （以空格为分割符,如a b,读取后转化为字符串）
std::string readFile()
{
	ifstream infile;
	infile.open("/some/path");
	string a,b;
	vector<string> vs;
	while(infile >> a >> b){
		vs.push_back(a);
	}
	string a1;
	for(auto iter = vs.begin(); iter != vs.end(); ++iter){
		if(iter!=vs.end()-1){
			a1.append(*iter+",");
		}else{
			a1.append(*iter);
		}
	}
	return a1;
}

// 调试显示错误信息 
void errorInfor() {
	std::cerr << "Error:\n\tfile:" << __FILE__ << std::endl
	            << "\tfunction: " << __func__ << std::endl
					<< "\tline: " << __LINE__ << std::endl
						<< "\tcompiled on: " << __DATE__
							<< " at " << __TIME__ << std::endl;
}


#endif
