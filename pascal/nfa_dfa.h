
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

void init () {

	_NFA.clear();
	gather.clear();
	action.clear();
	start.clear();
	_action.clear();
	final.clear();
	_flag.clear();
	_final.clear();
	fstream in ("out.txt");
	in >> final_tag;
	in >> start_tag;
	while (! in . eof ()) {
		transfer nfa;
		in >> nfa . start >> nfa . action >> nfa . final >> nfa . flag;
		_NFA . push_back (nfa);
		if (nfa . action != '@') {
			action . insert (nfa . action);
		}
	}
	in . close ();
}

//获取集合a的闭包,采用BFS算法获得
set<int> closure (set<int> a) {
	set<int> b;          //保存求取闭包后的新集合
	queue<int> q;
	for (set<int>::iterator it = a . begin (); it != a . end (); it ++) {
		b . insert (*it);
		q . push (*it);
	}
	while (! q . empty ()) {
		int temp = q . front ();
		q . pop ();
		for (int i = 0; i < _NFA . size (); i ++) {
			if (_NFA[i] . start == temp && _NFA[i] . action == '@') {
				int c = b . size ();
				b . insert (_NFA[i] . final);
				int d = b . size ();
				if (c != d) {
					q . push (_NFA[i] . final);
				}
			}
		}
	}
	return b;
}

//构造Move函数
set<int> Move (set<int> a, char action) {
	set<int> b;  //作为move后的一个新的集合
	for (set<int>::iterator it = a . begin (); it != a . end (); it ++) {
		for (int i = 0; i < _NFA . size (); i ++) {
			if (_NFA[i] . start == *it && _NFA[i] . action == action) {
				b . insert (_NFA[i] . final);
			}
		}
	}
	return b;
}


void nfa_dfa (string file_name) {
	init ();
	set<int> K0;
	set<int> K1;
	K0 . insert (start_tag);
	K1 = closure (K0);
	gather . insert (pair<set<int>, bool> (K1, false));
	bool flag = true;    //标记gather中是否存在未被访问的集合
	set<int> T;           //做为每次被取出来的集合
	set<int> U;
	while (flag) {
		for (auto it = gather . begin (); it != gather . end (); it ++) {
			if (it -> second == false) {
				T = it -> first;
				it -> second = true;   //标记其遍历过
				break;
			}
		}
		for (auto it = action . begin (); it != action . end (); it ++) {
			set<int> b = Move (T, *it);    //新集合2
			if (b . size () != 0) {
				start . push_back (T);
				_action . push_back (*it);
				U = closure (b);
				final . push_back (U);
				gather . insert (pair<set<int>, bool> (U, false));
				int count = 0;
				for (auto it = U . begin (); it != U . end (); it ++) {
					if (*it == final_tag) {
						_flag . push_back (true);
						break;
					}
					count ++;
				}
				if (count == U . size ()) {
					_flag . push_back (false);
				}
			}
		}
		int num = 0;
		for (auto it = gather . begin (); it != gather . end (); it ++) {
			if (it -> second == true) {
				num ++;
			}
		}
		if (num == gather . size ()) {
			flag = false;
		}
	}
	//cout << "--------------------NFA转DFA-----------------------" << endl;
	int num = 0;
	for (auto it = gather . begin (); it != gather . end (); it ++) {
		for (int i = 0; i < start . size (); i ++) {
			if (it -> first == start[i]) {
				start[i] . clear ();
				start[i] . insert (num);
			}
			if (it -> first == final[i]) {
				final[i] . clear ();
				final[i] . insert (num);
			}
		}
		num ++;
	}
	ofstream out(file_name);
	out << *start[0] . begin () << endl;
	for (int i = 0; i < _flag . size (); i ++) {
		if (_flag[i] == true) {
			_final . insert (*final[i] . begin ());
		}
	}
	for (auto it = _final . begin (); it != _final . end (); it ++) {
		out << *it << " ";
	}
	out << endl;
	for (int i = 0; i < start . size (); i ++) {
		out << *start[i] . begin () << " " << _action[i] << " " << *final[i] . begin () << " " << endl;
		/*
		if (_flag[i] == true) {
			cout << "终态" << endl;
		} else
			cout << "非终态" << endl;
		 */
	}
	out.close();
}

