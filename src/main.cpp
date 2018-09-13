#include <iostream>
#include <string>
#include "functions.h"

using namespace std;

/*
 * 简单主函数：
 * 测试functions.h函数的功能
 */
int main(){
	string p_command;
	p_command.clear();
	execute_cmd("ls",p_command);
	cout << p_command<<endl;

	p_command.clear();

	execute_cmd("ls -al",p_command);
	cout << p_command<<endl;
	
	return 0;
}
