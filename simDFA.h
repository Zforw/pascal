/*
*  God, Grant me
*  the SERENITY, to accept the things I cannot change,
*  COURAGE to change the things I can, and
*  the WISDOM to know the difference.
*  Talk is cheap.Show me the code.
*  Created by 赵丰 on 2022/4/21.
   Copyright (c) 2022 IBM Inc.All rights reserved.
*/
#ifndef UNTITLED_PASCAL_SIMDFA_H
#define UNTITLED_PASCAL_SIMDFA_H

#include <fstream>
using namespace std;
map<string, int> m;
string syn[30] = {"#","begin","if","then","while","do","end","","","",
	"token","sum","","+","-","*","/" ,":" ,":=","" ,
	"<" ,"<>" ,"<=" ,">" ,">=" ,"=" ,";" ,"(" ,")"};
extern
extern set<int> _final;
void initSim() {

}
int check(string s) {
	if(s=="*") { return 15; }
	else if(s=="(") { return 27; }
	else if(s==")") { return 28; }
	else if(s=="#") { return 0; }
	else {

	}
}
#endif //UNTITLED_PASCAL_SIMDFA_H
