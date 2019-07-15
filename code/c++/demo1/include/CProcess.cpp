/**********************************************
* Copyright (c)2018,caijun
* Filename: CProcess.cpp
* Description: .cpp文件，ssh功能函数
* Author: Cai Jun <johncai.caijun.cn@gmail.com>
* Time: 
***********************************************/

#ifndef CPROCESS_CPP
#define CPROCESS_CPP

#include "CProcess.h"

// 初始化
CProcess::CProcess(){
	#ifndef NDEBUG
	cout << "CProcess 类初始化" <<endl;
	#endif
	m_bExitDesktop = true;						// 默认存在desktop-session选项
	m_strFlag = "D";							// 默认标志桌面程序是desktop-session 
	m_strFullName = "";							// 空参数 
	m_resolution="800x600";       				// 设置分辨率默认为当前主机分辨率
	m_strXinerama = "no";						// 默认不支持多屏
	m_monitor = "null";
}
// ssh连接，封装了各种认证
bool CProcess::checkSsConn()
{
    if (true == m_cSshSession.sshConnect())
    {
		#ifndef NDEBUG
        printf("new_ssh connect success\n");
		#endif
    }
    else
    {
        printf("connect fail\n");
        return false;
    }

    int state = m_cSshSession.serverAuth();
    if (state != SSH_SERVER_KNOWN_OK)
    {
        ssh_write_knownhost(m_cSshSession.getSession());
        m_cSshSession.sshDisconnect();
        printf("server authenticate fail\n");
        return false;
    }
    else
    {
		#ifndef NDEBUG
        printf("server authenticate success\n");
		#endif
    }
        
    if (SSH_AUTH_SUCCESS != m_cSshSession.userAuth())
    {
        m_cSshSession.sshDisconnect();
        printf("user authenticate fail\n");
        return false;
    }
    else
    {
		#ifndef NDEBUG
        printf("user authenticate success\n");
		#endif
    }
    
    if (m_cSshSession.checkLogin() != false)    
    {
		#ifndef NDEBUG
        printf("check success\n");
		#endif
    }
    else
    {
        m_cSshSession.sshDisconnect();
        printf("check fail\n");
        return false;
    }
    return true;
}

// 将命令封装成channelInfo，插入channelConnections
void CProcess::executeCommand(const string& cmd)
{
    uuid_t uuid;
    char str[36] = {0};
    uuid_generate(uuid);
    uuid_unparse(uuid, str);

    string strpath = "export PATH=\"/usr/local/bin:/usr/bin:/bin\";";
    string shcmd = string("bash -l -c ") + "\'echo \"X2GODATABEGIN:" + str + "\"; " + strpath + "export TERM=\"dumb\";" + cmd + ";" + "echo \"X2GODATAEND:" + str + "\"\';";
    ChannelInfo con;
    con.channel = NULL;
    con.sock = -1;
    con.command = shcmd;

	#ifndef NDEBUG
	cout << "\n\n接收命令\npush_back\n" << endl;
	#endif
    //pthread_mutex_lock(&mutex);
    m_cSshSession.m_channelConnections.push_back(con);
    //pthread_mutex_unlock(&mutex);
}

// 线程，监听本地端口
// 如果有连接，则将socket封装到channelInfo，并插入channelConnections
void* tunnelLoop(void *arg)
{
	CProcess *cProcess = (CProcess*) arg;
	CSshSession &cSshSession = cProcess->getCSsion();
	Tool tool = cProcess->getTool();
	pthread_mutex_t lock = cProcess->getMutex();
	ChannelInfo& m_cinfo = cSshSession.getCInfo();
    string forwardHost = m_cinfo.forwardHost;
    int forwardPort = m_cinfo.forwardPort;
    string localHost = m_cinfo.localHost;
    int localPort = m_cinfo.localPort;
    int lstsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lstsock <= 0)
    {
        printf("error in socket\n");
    }
    const int y = 1;
    setsockopt(lstsock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int)); // 地址复用
    setsockopt(lstsock, IPPROTO_TCP, TCP_NODELAY, &y, sizeof(int)); // 禁用尼古拉算法
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(m_cinfo.localPort);
    if (bind(lstsock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        printf("error in bind\n");
        return (void*)NULL;
    }
    listen(lstsock, 5);

	#ifndef NDEBUG
    printf("start listen local port : %d\n", m_cinfo.localPort);
	#endif
    
	// 去执行nxproxy命令
	cProcess->executeCommand("do_nxproxy");

    while (1)
    {
        fd_set rfds;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(lstsock, &rfds);

        if (select(lstsock+1, &rfds, NULL, NULL, &tv) <= 0)
        {
            continue;
        }
        printf("select > 0\n");
        struct sockaddr_in cntaddr;
        socklen_t socklen = sizeof(cntaddr);    
        int cntsock = accept(lstsock,
							(struct sockaddr*)&cntaddr,
							&socklen);

        ChannelInfo con;
        con.sock = cntsock;
        con.forwardHost = forwardHost;
        con.forwardPort = forwardPort;
        con.localHost = localHost;
        con.localPort = localPort;

        pthread_mutex_lock(&lock);
        cSshSession.m_channelConnections.push_back(con);
        pthread_mutex_unlock(&lock);
		#ifndef NDEBUG
        printf("select end\n");
		#endif
    }

    return (void*)NULL;
}
// 建立Tunnel
// 正向: tunnelLoop
// 反向; 暂未实现
int CProcess::startTunnel(const string& forwardHost,
							const int forwardPort,
							const string& localHost, 
							int localPort, bool reverse)
{
    pthread_t tid;

	// 设置cinfo
	m_cSshSession.setCInfo(forwardHost,
							forwardPort,
							localHost,
							localPort);

    // 先考虑正向
    if (!reverse)
    {
		#ifndef NDEBUG
		cout << "开始tunnelLoop线程" <<endl;
		#endif
        pthread_create(&tid, NULL, tunnelLoop, &m_cSshSession);
    }
}


// 销毁一个channel，以及将它对应的channelInfo从channelConnections中移除
void CProcess::finalize(int item)
{
    vector<ChannelInfo> &channelConnections = m_cSshSession.m_channelConnections;
    int tcpSocket = channelConnections[item].sock;
    ssh_channel channel = channelConnections[item].channel;
    if (channel)
    {
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }
    if (tcpSocket > 0)
    {
        shutdown(tcpSocket, SHUT_RDWR);
        close(tcpSocket);
    }
    
    channelConnections.erase(channelConnections.begin() + item);
	#ifndef NDEBUG
    printf("one channel close\n");
	#endif
}

// 解析出sessionId
char* CProcess::getSessionId(const string& all_buf)
{
	const char *buf = all_buf.c_str();
	struct Userinfo& m_uinfo = m_cSshSession.getUInfo();

    const char *lh = buf, *rh = lh;

    lh = strstr(buf, m_uinfo.user);
    if (lh == NULL)
    {
	#ifndef NDEBUG
	cout<< "\n\nin getSeesionId, buf=" << buf << endl;
	cout<< "\n\nin getSessionId , user=" <<m_uinfo.user <<endl;
	#endif
        return NULL;
    }
    rh = strstr(lh, "|");
    if (rh == NULL)
    {
        return NULL;
    }
    int len = rh - lh;
    char *res = new char[len+1];
    strncpy(res, lh, len);
    res[len] = '\0';
    return res;
}


// 执行nxproxy命令
void CProcess::doNxproxy() {
	struct x2goSession resume_session = m_tool.getResumeSession();	
    pid_t pid;
    if ((pid = fork()) == 0)    // 子进程调用nxproxy
    {
			#ifndef NDEBUG
			cout << "-display: " << resume_session.display<< endl;
			#endif

			// 設置nxproxy命令選項，從resume_session文件讀取options
            string cmd = string("nx/nx,options=")+m_tool.getHomeDir()+
								string("/.x2go/S-")+ 
								resume_session.sessionId+ 
								string("/options:")+resume_session.display;
            const char *shcmd = cmd.c_str();
			#ifndef NDEBUG
            printf("nxproxy 命令 : %s\n", shcmd);
			#endif


			// 獲取當前目錄
			string work_dir = m_tool.getPwd()+"/tlrd-client";
			#ifndef NDEBUG
			cout << "當前的目錄爲：" << work_dir << endl;
			#endif

			setenv("NX_CLIENT", work_dir.c_str(), false);
			
			#ifndef NDEBUG
            printf("NX_CLIENT的環境變量：%s\n", getenv("NX_CLIENT"));
			#endif
            execlp("nxproxy", "nxproxy", "-S", shcmd, NULL);    //执行nxproxy命令
            exit(0);
    }
		
	// 判断是否是单应用程序 
	if ( m_strCmd == "CINNAMON") {
		m_strFullName = " cinnamon-session nosnd "+m_strFlag;
	}else if ( m_strCmd == "WWWBROWSER"){
		m_strFullName = " WWWBROWSER nosnd "+m_strFlag;
	}else if ( m_strCmd == "MAILCLIENT"){
		m_strFullName = " MAILCLIENT nosnd "+m_strFlag;
	}
	string pulse_cmd =string("export PULSE_CLIENTCONFIG=\"${HOME}/.x2go/C-")+
						resume_session.sessionId+
						string("/.pulse-client.conf\";setsid /usr/bin/x2goruncommand ")+
						resume_session.display+" "+ resume_session.pid+" "+ 
						resume_session.sessionId+" "+resume_session.sndPort+
						m_strFullName+string(" 1> /dev/null 2> /dev/null & exit");
	#ifndef NDEBUG
	cout << "\nstart pulse_cmd:\n" << endl;
	#endif
	executeCommand(pulse_cmd);
}

// 查找是否存在啊gr_port
bool CProcess::findGrport(const string& s) {
	if (s.find("gr_port") != string::npos) {return true;}
	return false;
}
// 根据执行的命令进行信息处理
void CProcess::handleCommand(const string& m_command, const string& all_buffer) {
	if (m_command == "x2golistsessions") {
		#ifndef NDEBUG
		cout << "string_buffer="<<all_buffer<<endl;
		#endif
		m_pId = getSessionId(all_buffer);
		if (m_pId == NULL){
			if (!m_bExitDesktop) {
				cout << "not found application command! " << endl;
				m_tool.displayHelp();
				exit(-1);
			}
			// 执行startagent命令
			string startagent = string("X2GODPI=118 X2GO_XINERAMA=")+
									m_strXinerama+string(" x2gostartagent ")+
									m_resolution+string(" adsl 16m-jpeg-9 unix-kde-depth_24 us auto 1 ")+
									m_strFlag+" "+m_strCmd+string(" both");
			executeCommand(startagent);
		}else {
			if (!m_bExitDesktop)
			{ 
				// 將獲取到的Buffer信息存入resumingSession
				m_tool.setResumingSession(all_buffer);
			
				// 执行resume-session命令
				executeCommand("x2goresume-session " + string(m_pId)+" "+m_resolution+string(" adsl 16m-jpeg-9 us auto 1 both no"));
				#ifndef NDEBUG
					cout << "afer resume-session:" << string(m_pId) <<endl << m_resolution << endl;
				#endif
				
			} else {
				// 执行x2goterminate-session	
				#ifndef NDEBUG
				cout << "\n\n存在resume-session,终结掉，startagent\n"<<endl;
				#endif
				string terminate = "x2goterminate-session "+string(m_pId);
				executeCommand(terminate);
			}
			
		}
	} else if (m_command == "x2goterminate-session") {
		// do nothing
		if ( m_bExitDesktop ) {
			// 执行startagent命令
			string startagent = string("X2GODPI=118 X2GO_XINERAMA=")+
									m_strXinerama+string(" x2gostartagent ")+
									m_resolution+string(" adsl 16m-jpeg-9 unix-kde-depth_24 us auto 1 ")+
									m_strFlag+" "+m_strCmd+string(" both");
			executeCommand(startagent);
		
		}
	} else if (m_command == "x2goresume-session") {
		bool flag = findGrport(all_buffer);
		if (flag) {
				// 获取端口后进行转发
				struct x2goSession resume_session = m_tool.getResumeSession();	
				int forwardport = atoi(resume_session.grPort.c_str());
				int local_port = forwardport+1000;

				// 端口进行绑定后执行nxproxy命令
				startTunnel("localhost", forwardport, "localhost", forwardport+1000, false);
			
		}
	} else if (m_command == "x2gostartagent") {
		// 将startAgent获得的参数信息存入resumingSession,并写入文件
		vector<string> result = m_tool.split(all_buffer,"\n");
		m_tool.setStartAgentSession(result);	
		// 获取端口后进行转发
		struct x2goSession resume_session = m_tool.getResumeSession();	
		int forwardport = atoi(resume_session.grPort.c_str());
		int local_port = forwardport+1000;

		// 端口进行绑定后执行nxproxy命令
		startTunnel("localhost", forwardport, "localhost", forwardport+1000, false);

	} else if (m_command == "do_nxproxy") {
		doNxproxy();	
	} else if (m_command == "x2goruncommand") {
		// do nothing		
	}

}

// 判断执行哪一个命令
int CProcess::jcommand(const string s, string& command) {
	string mcommand;
	mcommand.clear();
	if (s.find("x2golistsessions") != string::npos) {
		mcommand = string("x2golistsessions");	
	} else if (s.find("x2goterminate-session") != string::npos) {
		mcommand = string("x2goterminate-session");
	} else if (s.find("x2goresume-session") != string::npos) {
		mcommand = string("x2goresume-session");
	} else if (s.find("x2gostartagent") != string::npos) {
		mcommand = string("x2gostartagent");
	} else if (s.find("do_nxproxy") != string::npos) {
		mcommand = string("do_nxproxy");
	} else if (s.find("x2goruncommand") != string::npos) {
		mcommand = string("x2goruncommand");
	} else {
		mcommand = string("nocommand");
	}	
	command = mcommand;
	return 0;
}


// 根据命令添加相应的前缀
string CProcess::judgeCommand(string cmd){
	if ( cmd == "KDE-SESSION" || cmd == "kde-session"){
		cmd = "KDE";
	}else if (cmd == "GNOME-SESSION" || cmd == "gnome-session"){
		cmd = "GNOME";
	}else if (cmd == "LXDE-SESSION" || cmd == "lxde-session"){
		cmd = "LXDE";
	}else if (cmd == "MATE-SESSION" || cmd == "mate-session"){
		cmd = "MATE";
	}else if (cmd == "UNITY-SESSION" || cmd == "unity-session"){
		cmd = "UNITY";
	}else if (cmd == "CINNAMON-SESSION" || cmd == "cinnamon-session"){
		cmd = "CINNAMON";
	}else if (cmd == "TRINITY-SESSION" || cmd == "trinity-session"){
		cmd = "trinity";
	}else if (cmd == "ICEWM-SESSION" || cmd == "icewm-session"){
		cmd = "ICEWM";
	}else if (cmd == "internet browser" || cmd == "firefox" || cmd == "FIREFOX"){
		m_strFlag = "R";
		#ifndef NDEBUG
		cout << "\nm_strFlag:"<<m_strFlag<<endl;
		#endif
		cmd = "WWWBROWSER";
		#ifndef NDEBUG
		cout << "cmd" << cmd<<endl;
		#endif
	}else if (cmd == "email" || cmd == "EMAIL" || cmd == "Email client"){
		m_strFlag = "R";
		#ifndef NDEBUG
		cout << "\nm_strFlag:"<<m_strFlag<<endl;
		#endif
		cmd = "MAILCLIENT";
		#ifndef NDEBUG
		cout << "cmd" << cmd<<endl;
		#endif
	}else if (cmd == ""){
		m_bExitDesktop = false;
	}else{
		cmd = "no";
	}
	return cmd;
}

// 设置ssh连接信息
void CProcess::setCmdArgs(commandArgs cmdArgs){
	CSshSession &m_cSshSession = getCSsion();
	struct Userinfo &m_uinfo = m_cSshSession.getUInfo();
	m_uinfo.user = (char*)cmdArgs.user.c_str();
	m_uinfo.server_ip = (char*)cmdArgs.host.c_str();
	m_uinfo.port = (char*)cmdArgs.port.c_str();
	m_uinfo.passwd = (char*)cmdArgs.passwd.c_str();
	m_uinfo.command = (char*)cmdArgs.appCommand.c_str();
	this->m_resolution =cmdArgs.resolution;
	this->m_strXinerama = cmdArgs.xinerama;
	this->m_monitor = cmdArgs.monitor;
	
	// 根据命令添加前缀
	this->m_strCmd = judgeCommand(m_uinfo.command);
	
	#ifndef NDEBUG
	cout<<m_uinfo.user<<endl;
	cout<<m_uinfo.server_ip<<endl;
	cout << m_uinfo.passwd<<endl;
	cout<<m_uinfo.command<<endl;
	#endif

}

/*
string optstr = "ab:c::"

单个字符a         表示选项a没有参数            格式：-a即可，不加参数
单字符加冒号b:     表示选项b有且必须加参数      格式：-b 100或-b100,但-b=100错
单字符加2冒号c::   表示选项c可以有，也可以无     格式：-c200，其它格式错误
*/

// 设置参数命令
void CProcess::setCommandOpt(int argc, char* argv[]){
	m_tool.getOptLong(argc,argv,"fhu:v:p:c:");
	if ( argc <= 1 ) {
		m_tool.displayHelp();
		exit(-1);
	}
	// 设置命令行参数
	setCmdArgs(m_tool.m_tCmdArgs);

	if (m_strCmd=="no"){
		cout << "\n\t appcommand not exits"<<m_strCmd<<endl;
		exit(-1);
	}
	#ifndef NDEBUG
	cout << "\n\treturn appcommand:"<<m_strFlag+" "+m_strCmd<<endl;
	#endif

	if ( !m_bExitDesktop ) {
		cout << "\nm_strCmd为空" << endl;
	} 
}








#endif
