/**********************************************************************   
 * * Copyright (c)2018, caijun
 * * Filename: main.cpp
 * * Description：主文件    
 * * Author：Cai Jun  <johncai.caijun.cn@gmail.com> 
 * * Time: 10:54 AM Sep 16, 2018 
 * ************************************************************************/ 

#ifndef MAIN_CPP
#define MAIN_CPP

#include <iostream>
//#include "demo.h"
#include "TextQuery.h"

using namespace std;

void runQueries(ifstream &infile) {
	TextQuery tq(infile);

	while(true) {
		cout << "Enter word to look for, or q to quit:";
		string s;
		s.clear();

		if (!(cin >> s) || s = "q") break;

		print(cout,tq.query(s)) << endl;
	}
}
int main(){
//	demo1();
//	demo2();
//	demo3();
//	demo4();
//	demo5();
//	demo6();
//	demo7();
//	demo8();
//	demo8();
//	demo9();
//	demo10();

	string pwd = getPwd();
	cout << "pwd = " << pwd << endl;
	ifstream file("wordfile");

	
	runQueries(file);
	return 0;
}

#endif





