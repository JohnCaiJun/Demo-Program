/**********************************************
 * * Copyright (c)2018,caijun
 * * Filename:
 * * Description:
 * * Author: Cai Jun <johncai.caijun.cn@gmail.com>
 * * Time: 
 * ***********************************************/

#ifndef TEXTQUERY_H
#define TEXTQUERY_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <sstream>

using namespace std;

class QueryResult;
class TextQuery {
public:
	using line_no = std::vector<std::string>::size_type;
	TextQuery(std::ifstream&);
	QueryResult query(const std::string&) const;
private:
	std::shared_ptr<std::vector<std::string>> file;
	std::map<std::string,std::shared_ptr<std::set<line_no>>> wm;
};

class QueryResult {
friend std::ostream& print(std::ostream&,const QueryResult&);
public:
	using line_no = std::vector<std::string>::size_type;
	QueryResult(std::string s,
				std::shared_ptr<std::set<line_no>> p,
				std::shared_ptr<std::vector<std::string>> f):sought(s),lines(p),file(f){}
private:
	std::string sought;
	std::shared_ptr<std::set<line_no>> lines;
	std::shared_ptr<std::vector<std::string>> file;

};

TextQuery::TextQuery(std::ifstream& is):file(new std::vector<string>)
{
	std::string text;
	while(getline(is,text)) {
		file->push_back(text);
		int n =file->size() - 1;
		std::istringstream line(text);
		std::string word;
		while(line>>word) {
			auto &lines = wm[word];
			if(!lines)
				lines.reset(new std::set<line_no>);
			lines->insert(n);
		}
	}
};

QueryResult
TextQuery::query(const string &sought) const
{
	static shared_ptr<set<line_no>> nodata(new std::set<line_no>);
	auto loc=wm.find(sought);
	if(loc == wm.end())
		return QueryResult(sought,nodata,file);
	else
		return QueryResult(sought,loc->second,file);

}

ostream &print(ostream & os, const QueryResult &qr)
{
	os << qr.sought << " occurs " << qr.lines->size() << " "
	   << make_plural(qr.lines->size(),"time","s") << endl;
	for (auto num: *qr.lines)
		os << "\t (line " << num+1 << ") "
			<< *(qr.file->begin() * num) <<endl;

	return os;
}

#endif
