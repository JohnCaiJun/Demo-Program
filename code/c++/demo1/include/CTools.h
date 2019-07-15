/**********************************************************************   
 * * Copyright (c)2018, caijun
 * * Filename: CTools.h
 * * Description：工具类头文件   
 * * Author：Cai Jun  <johncai.caijun.cn@gmail.com> 
 * * Time: 4:15 PM Aug 24, 2018 
 * ************************************************************************/ 

#ifndef FILE_TOOLES_H
#define FILE_TOOLES_H

#include <pwd.h>
#include <string>
#include <string.h>
#include <vector>
#include <cctype>
using namespace std;

// 存儲獲取的x2gosession
struct x2goSession
{
	string pid;
	string sessionId; 					// 唯一標識的sessionId
	string display;						// 應用程序窗口顯示號碼
	string cookie;						// 唯一標示的cookie
	string clientIp;					// 客戶端ip
	string grPort;						// 圖形端口
	string sndPort;						// 聲音端口
	string fsPort;						// 文件端口 
	int colorDepth;						// 色彩深度
	bool fullscreen;					// 是否全屏

};

// 存储命令行参数
struct commandArgs
{
	string fileName;
	string user;
	string host;
	string appCommand;
	string port;
	string passwd;
	string resolution;
	string xinerama;
	string monitor;
};


/*自己封裝的處理數據的庫*/
class Tool{
	public:
		struct commandArgs m_tCmdArgs;
		struct x2goSession resume_session;
		struct option* long_options;
		bool flag;
		bool flag2;
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
	public:
		Tool();
		~Tool(){};
		string getHomeDir();												// 獲取家用戶目錄
		string getPwd();													// 獲取當前目錄
		string getUserName();												// 获取用户名
		void getOptLong(int argc, char**, string);							// 处理命令行参数
		void displayHelp();													// 显示帮助信息
		void setResumingSession(char*);										// 獲取遠端發過來的消息
		void setResumingSession(string);									// 保存执行resume-session命令后返回的信息
		void setStartAgentSession(vector<string>);							// 保存执行startagent命令后返回的信息
		void writeResumingsessionFile(x2goSession&);						// 將遠端發過來的消息進行保存
		vector<string> split(string, string);								// 分割字符串，保存在容器中
		int execute_cmd(const char*, string&);								// 执行linux命令并获取返回信息
		int execute_cmd(const char*);										// 只执行linux命令，不返回信息结果
		string getScrennResolution();										// 获取扩展屏幕分辨率
		string getMainResolution();											// 获取当前屏幕分辨率
		void choiceMonitor(const string&);											// 指定屏幕显示
		x2goSession getResumeSession(){return resume_session;}				// 返回保存的遠端信息(resume_session)
		void kprocess(const string);										// 杀死占用端口的进程

};

// 锁
class CMutex{
	public:
		CMutex();
		~CMutex(){};
		void Lock();
		void Unlock();
	private:
		pthread_mutex_t mutex;
};

#endif
