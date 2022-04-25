#pragma once

#include<iostream>
#include<vector>
#include<map>
#include<queue>
#include<set>
#include<string>
#include<fstream>

using namespace std;

struct transfer {
	int start;      //起始状态
	char action;    //所执行的动作
	int final;      //执行action后的状态
	int flag = 0;   //判断final是否为终止元素
};


int num_1 = 0, num_2 = 0;     //分别用于统计状态数，以及弧的数量
int final_tag;
int start_tag;

//保存nfa中的元素
vector<transfer> _NFA;
//统计gather中的集合是否被标记
map<set<int>, bool> gather;
//用于保存所有可执行的动作，@除外
set<char> action;
//用于保存新的转换情况dfa
vector<set<int>> start;
vector<char> _action;
vector<set<int>> final;
vector<bool> _flag;
//保存终态结果
set<int> _final;

vector<int> start_minDFA;
vector<int> final_minDFA;
vector<char> action_minDFA;
vector<bool> flag_minDFA;


int trans[128][80];		/*DFA的转移矩阵*/
int startState;				/*DFA的初态*/


void init() {

	_NFA.clear();
	gather.clear();
	action.clear();
	start.clear();
	_action.clear();
	final.clear();
	_flag.clear();
	_final.clear();


	start_minDFA.clear();
	final_minDFA.clear();
	action_minDFA.clear();
	flag_minDFA.clear();

	fstream in("out.txt");
	in >> final_tag;
	in >> start_tag;
	while (!in.eof()) {
		transfer nfa;
		in >> nfa.start >> nfa.action >> nfa.final >> nfa.flag;
		_NFA.push_back(nfa);
		if (nfa.action != '@') {
			action.insert(nfa.action);
		}
	}
	in.close();
}

//获取集合a的闭包,采用BFS算法获得
set<int> closure(set<int> a) {
	set<int> b;          //保存求取闭包后的新集合
	queue<int> q;
	for (set<int>::iterator it = a.begin(); it != a.end(); it++) {
		b.insert(*it);
		q.push(*it);
	}
	while (!q.empty()) {
		int temp = q.front();
		q.pop();
		for (int i = 0; i < _NFA.size(); i++) {
			if (_NFA[i].start == temp && _NFA[i].action == '@') {
				int c = b.size();
				b.insert(_NFA[i].final);
				int d = b.size();
				if (c != d) {
					q.push(_NFA[i].final);
				}
			}
		}
	}
	return b;
}

//构造Move函数
set<int> Move(set<int> a, char action) {
	set<int> b;  //作为move后的一个新的集合
	for (set<int>::iterator it = a.begin(); it != a.end(); it++) {
		for (int i = 0; i < _NFA.size(); i++) {
			if (_NFA[i].start == *it && _NFA[i].action == action) {
				b.insert(_NFA[i].final);
			}
		}
	}
	return b;
}



bool judge(const set<int>& s, int x)
{
	bool flag = false;
	for (auto it = s.begin(); it != s.end(); it++)
	{
		if (*it == x)
		{
			flag = true;
			break;
		}
	}
	return flag;
}



void nfa_dfa(string file_name) {

	for (int i = 0; i < 128; i++)
		for (int j = 0; j < 80; j++)
			trans[i][j] = 100;


	init();
	set<int> K0;
	set<int> K1;
	K0.insert(start_tag);
	K1 = closure(K0);
	gather.insert(pair<set<int>, bool>(K1, false));
	bool flag = true;    //标记gather中是否存在未被访问的集合
	set<int> T;           //做为每次被取出来的集合
	set<int> U;
	while (flag) {
		for (auto it = gather.begin(); it != gather.end(); it++) {
			if (it->second == false) {
				T = it->first;
				it->second = true;   //标记其遍历过
				break;
			}
		}
		for (auto it = action.begin(); it != action.end(); it++) {
			set<int> b = Move(T, *it);    //新集合2
			if (b.size() != 0) {
				start.push_back(T);
				_action.push_back(*it);
				U = closure(b);
				final.push_back(U);
				gather.insert(pair<set<int>, bool>(U, false));
				int count = 0;
				for (auto it = U.begin(); it != U.end(); it++) {
					if (*it == final_tag) {
						_flag.push_back(true);
						break;
					}
					count++;
				}
				if (count == U.size()) {
					_flag.push_back(false);
				}
			}
		}
		int num = 0;
		for (auto it = gather.begin(); it != gather.end(); it++) {
			if (it->second == true) {
				num++;
			}
		}
		if (num == gather.size()) {
			flag = false;
		}
	}
	//cout << "--------------------NFA转DFA-----------------------" << endl;
	int num = 0;
	for (auto it = gather.begin(); it != gather.end(); it++) {
		for (int i = 0; i < start.size(); i++) {
			if (it->first == start[i]) {
				start[i].clear();
				start[i].insert(num);
			}
			if (it->first == final[i]) {
				final[i].clear();
				final[i].insert(num);
			}
		}
		num++;
	}


	ofstream out(file_name);
	out << *start[0].begin() << endl;
	for (int i = 0; i < _flag.size(); i++) {
		if (_flag[i] == true) {
			_final.insert(*final[i].begin());
		}
	}
	for (auto it = _final.begin(); it != _final.end(); it++)
	{
		out << *it << " ";
	}

	out << endl;

	for (int i = 0; i < start.size(); i++) {
		trans[*start[i].begin()][_action[i] - '+'] = *final[i].begin();

		/*
		if (_flag[i] == true) {
			cout << "终态" << endl;
		} else
			cout << "非终态" << endl;
		 */
	}

	//minDFA

	set<int> S;
	set<int> F;
	for (int i = 0; i < start.size(); i++) {
		S.insert(*start[i].begin());
		S.insert(*final[i].begin());
		if (_flag[i] == true) {
			F.insert(*final[i].begin());
		}

	}

	set<int> temp;
	set_difference(S.begin(), S.end(), F.begin(), F.end(), insert_iterator<set<int>>(temp, temp.begin()));
	vector<int> del;


	for (unsigned int i = 0; i < S.size(); i++)
	{

		if (judge(temp, i))
		{
			int arr[80];
			for (int j = 0; j < 80; ++j)
				arr[j] = trans[i][j];


			for (int j = i + 1; j < S.size(); j++)
			{
				if (judge(temp, j))
				{
					int count = 0;
					for (int k = 0; k < 80; k++)
					{
						if (arr[k] == trans[j][k])
							count++;
					}
					if (count == 80)
					{
						del.push_back(j);
					}
				}
				else
					continue;
			}
		}
		else
		{
			int arr[80];
			for (int j = 0; j < 80; ++j)
				arr[j] = trans[i][j];

			for (int j = i + 1; j < S.size(); j++)
			{
				if (judge(temp, j))
				{
					continue;
				}
				else
				{
					int count = 0;
					for (int k = 0; k < 80; k++)
					{
						if (arr[k] == trans[j][k])
							count++;
					}
					if (count == 80)
					{
						del.push_back(j);
					}
				}
			}
		}
	}


	for (unsigned int i = 0; i < S.size(); i++)
	{
		bool flag = true;
		for (unsigned int j = 0; j < del.size(); j++)
		{
			if (del[j] == i)
				flag = false;
		}
		if (flag == true)
		{
			for (int j = 0; j < 80; j++)
			{
				if (trans[i][j] == 100)
					continue;
				else
				{
					start_minDFA.push_back(i);
					final_minDFA.push_back(trans[i][j]);
					action_minDFA.push_back(j + '+');
					if (judge(F, trans[i][j]))
						flag_minDFA.push_back(true);
					else
						flag_minDFA.push_back(false);
				}
			}
		}

	}


	for (int i = 0; i < start_minDFA.size(); i++) {
		out << start_minDFA[i] << " " << action_minDFA[i] << " " << final_minDFA[i] << " " << endl;
		/*
		if (_flag[i] == true) {
			cout << "终态" << endl;
		} else
			cout << "非终态" << endl;
		 */
	}

	out.close();
}

