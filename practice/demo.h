/**********************************************
 * * Copyright (c)2018,caijun
 * * Filename: demo.h
 * * Description: c++ primer edition 5 demo
 * * Author: Cai Jun <johncai.caijun.cn@gmail.com>
 * * Time: 10:07 星期六 2018/9/15 
 * ***********************************************/

#ifndef DEMO_H
#define DEMO_H

#include <iostream>
#include <string>
#include <cctype>
#include <cstddef>
#include "Sales_item.h"

void demo1(){

	Sales_item book;

	std::cin >> book;

	std::cout << book << std::endl;
}

void demo2(){
	Sales_item item1, item2;
	std::cin >> item1 >> item2;
	std::cout << item1 + item2 << std::endl;
}

int demo3(){
	Sales_item item1, item2;
	std::cin >> item1 >> item2;

	if (item1.isbn() == item2.isbn()){
		std::cout << item1 + item2 << std::endl;
		return 0;		
	
	} else {
		std::cerr << "Data must refer to same ISBN"
					<< std::endl;
		return -1;
	}
}

int demo4(){
	Sales_item total;

	if (std::cin >> total) {
		Sales_item trans;

		while (std::cin >> trans) {
			if (total.isbn() == trans.isbn())
				total += trans;
			else {
				std::cout << total << std::endl;
				total = trans;
			}
		
		}
		std::cout << total << std::endl;
	} else {
		std::cerr << "No data?!" << std::endl;
		return -1;
	}
	
	return 0;
}

// test range for
void demo5(){
	std::string str("some string");
	for (auto c : str) 
		std::cout << c << std::endl;
}

void demo6(){
	std::string str("hello world!!!");
	decltype(str.size()) punct_cnt = 0;
	std::cout << "punct_cnt=" << punct_cnt << std::endl;
	for (auto c : str) 
		if (ispunct(c)) {
			punct_cnt++;				
		}
	std::cout << "punct_cnt=" << punct_cnt << std::endl;
}

void demo7(){
	std::string s("hello world!!!");
	for (auto &c : s) 
		c = toupper(c);
	std::cout << "s=" << s << std::endl;
}

void demo8(){
	std::string s("some string");

	for ( decltype(s.size()) index = 0;
			index != s.size() && !isspace(s[index]); index++)
				s[index] = toupper(s[index]);
	std::cout << s << std::endl;
}

void demo9() {
	std::cout << "char " << sizeof(char) << std::endl;
	std::cout << "int " << sizeof(int) << std::endl;
	std::cout << "float " << sizeof(float) << std::endl;
	std::cout << "double " << sizeof(double) << std::endl;
	std::cout << "string " << sizeof(std::string) << std::endl;

	int a = 10;
	int b[10];
	std::cout << "int a " << sizeof(a) << std::endl;
	std::cout << "int b " << sizeof(b) << std::endl;
	std::cout << "int b[10] " << sizeof(b[10]) << std::endl;
}



#endif

