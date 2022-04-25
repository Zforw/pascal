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
#include <sstream>
#include <map>
#include "nfa_dfa.h"
#include "nfa.h"

const int N = 20000;
std::map<std::string, int> m;
std::string syn[30] = {"#", "begin", "if", "then", "while", "do", "end", "", "", "",
					   "token", "sum", "", "+", "-", "*", "/", ":", ":=", "",
					   "<", "<>", "<=", ">", ">=", "=", ";", "(", ")"};
int keywords_syn[] = {1, 2,3,4,5,6,13,14,16,17,18,20,21,22,23,24,25,26};
class Rule {
public:
	string file_name;
	int tag;
	Rule(string name, int t = 0) {
		tag = t;
		file_name = name;
	}
	struct edge {
		char w;
		int to, next;
	} e[20021];
	int head[N], num_edge = 0, n = 0, start;
	int end[N];

	void add (int u, int v, char w) {
		e[++ num_edge] . next = head[u];
		e[num_edge] . to = v;
		e[num_edge] . w = w;
		head[u] = num_edge;
	}

	void init () {
		memset (head, 0, sizeof (head));
		memset (end, 0, sizeof (end));

		int key = 0;
		std::ifstream inFile (file_name, std::ios::in);
		std::string lineStr;
		getline (inFile, lineStr);
		start = atoi(lineStr.c_str());
		int i = -1;
		while (getline (inFile, lineStr)) {
			if (i == -1) {
				int sum = 0;
				for (char j: lineStr) {
					if (j == ' ') {
						end[sum] = (tag == 0)?keywords_syn[key++]:tag;
						sum = 0;
					} else {
						sum = sum * 10 + j - '0';
					}
				}
				i++;
				continue;
			}
			std::stringstream ss (lineStr);
			std::string str;
			int j = 0;
			// 按照逗号分隔
			int u, v;
			char w;
			while (getline (ss, str, ' ')) {
				if (str . length () > 0) {
					if (j == 0) {
						u = atoi (str . c_str ());
					} else if (j == 1) {
						w = str[0];
					} else {
						v = atoi (str . c_str ());
						add (u, v, w);
					}
					j++;
				}
			}
			i++;
		}
		inFile.close();

	}
	int check(string s) {
		int st = start;
		for(char i : s) {
			bool flag = false;
			for(int j = head[st];j;j=e[j].next) {
				if(e[j].w == i) {

					flag = true;
					st = e[j].to;
					break;
				}
			}
			if(! flag) {
				return 0;
			}
		}
		return end[st];
	}

};
Rule key("keywords.txt"), token("tokens.txt", 10), sum("sums.txt", 11);
void initSim () {
	string keywords = "begin|if|then|while|do|end|+|-|/|:|:=|<|<>|<=|>|>=|=|;";
	keywords = InfixToSuffix (keywords);
	int i, j;
	for (i = 0; i < MAX; i ++) {
		NfaStates[i] . index = i;
		NfaStates[i] . input = '#';
		NfaStates[i] . chTrans = - 1;
	}
	NFA n = StrToNfa (keywords);
	printNFA (n);
	nfa_dfa ("keywords.txt");

	string tokens = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|1|2|3|4|5|6|7|8|9|0)*";
	tokens = InfixToSuffix (tokens);
	for (i = 0; i < MAX; i ++) {
		NfaStates[i] . index = i;
		NfaStates[i] . input = '#';
		NfaStates[i] . chTrans = - 1;
	}
	n = StrToNfa (tokens);
	printNFA (n);
	nfa_dfa ("tokens.txt");

	string sums = "(1|2|3|4|5|6|7|8|9|0)(1|2|3|4|5|6|7|8|9|0)*";
	sums = InfixToSuffix (sums);
	for (i = 0; i < MAX; i ++) {
		NfaStates[i] . index = i;
		NfaStates[i] . input = '#';
		NfaStates[i] . chTrans = - 1;
	}

	n = StrToNfa (sums);
	printNFA (n);
	nfa_dfa ("sums.txt");

	key.init();
	token.init();
	sum.init();
}

int check (string s) {
	if (s == "*") { return 15; }
	else if (s == "(") { return 27; }
	else if (s == ")") { return 28; }
	else if (s == "#") { return 0; }
	else {
		int k = key.check(s);
		if(k == 0) {
			k = token. check (s);

			if(k == 0) {
				k = sum. check (s);
				return k;
			} else {
				return k;
			}
		} else {
			return k;
		}
	}
}

#endif //UNTITLED_PASCAL_SIMDFA_H
