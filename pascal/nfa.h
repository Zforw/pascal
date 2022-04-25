#include<iostream>
#include<cstring>
#include<string>
#include<stack>
#include<fstream>
#include<vector>
#include<set>
#include<queue>
#define MAX 1280

using namespace std;

bool isValid(char ch) {
	if(isalnum(ch)) return true;
	else if(ch == '+') return true;
	else if(ch == '-') return true;
	else if(ch == '/') return true;
	else if(ch == ':') return true;
	else if(ch == '=') return true;
	else if(ch == '<') return true;
	else if(ch == '>') return true;
	else if(ch == ';') return true;
	return false;
}

typedef set<int> IntSet;

struct NfaState {
	int index;    //状态编号
	char input;    //NFA状态弧上用于转移的值
	int chTrans;    //NFA状态弧转移到的状态号
	IntSet epTrans;    //当前状态通过ε转移到的状态号集合
};

struct NFA {
	NfaState *head;
	NfaState *tail;
};

NfaState NfaStates[MAX];    //NFA状态数组
int nfaStateNum = 0;        //NFA状态总数

//从状态s1到状态s2添加一条弧，弧上的值为ch
void addArc (NfaState *s1, NfaState *s2, char ch) {
	s1 -> input = ch;
	s1 -> chTrans = s2 -> index;
}

//从状态s1到状态s2添加一条弧，弧上的值为ε
void addArc (NfaState *s1, NfaState *s2) {
	s1 -> epTrans . insert (s2 -> index);
}

//新建一个NFA（即从NFA状态数组中取出两个状态）
NFA creatNFA (int sum) {
	NFA n;

	n . head = &NfaStates[sum];
	n . tail = &NfaStates[sum + 1];

	return n;
}

//在字符串s第n位后面插入字符ch
void insert (string &s, int n, char ch) {

	s += '#';

	for (int i = s . size () - 1; i > n; i --) {
		s[i] = s[i - 1];
	}

	s[n] = ch;
}

//对字符串s进行预处理，在第一位是字母、‘*’或‘)’且第二位是字母或‘(’之间加入连接符‘&’即ab为a&b
void predo (string &s) {

	int i = 0, length = s . size ();

	while (i < length) {
		if (isValid (s[i]) || (s[i] == '*') || (s[i] == ')')) {
			if (isValid (s[i + 1]) || s[i + 1] == '(') {
				insert (s, i + 1, '&');
				length ++;
			}
		}
		i ++;
	}
}

//中缀转后缀时的优先级
int priority (char ch) {

	if (ch == '*') {
		return 3;
	}

	if (ch == '&') {
		return 2;
	}

	if (ch == '|') {
		return 1;
	}

	if (ch == '(') {
		return 0;
	}
}

//中缀表达式转后缀表达式
string InfixToSuffix (string s) {

	predo (s);    //对字符串进行预处理

	string str;        //要输出的后缀字符串
	stack<char> Ope;    //运算符栈

	for (int i = 0; i < s . size (); i ++) {
		if (isValid (s[i]))    //如果是操作数直接输出
		{
			str += s[i];
		} else {
			if (s[i] == '(')    //遇到左括号直接压栈
			{
				Ope . push (s[i]);
			} else if (s[i] == ')')//遇到右括号，寻找左括号
			{
				char ch = Ope . top ();
				while (ch != '(')    //将栈中元素出栈，直到栈顶为左括号
				{
					str += ch;
					Ope . pop ();
					ch = Ope . top ();
				}
				Ope . pop ();
			} else {
				if (! Ope . empty ()) {
					char ch = Ope . top ();
					while (priority (ch) >= priority (s[i]))    //弹出栈中优先级大于等于当前运算符的运算符
					{
						str += ch;
						Ope . pop ();
						if (Ope . empty ()) {
							break;
						} else ch = Ope . top ();
					}

					Ope . push (s[i]);        //再将当前运算符入栈
				} else                //如果栈为空，直接将运算符入栈
				{
					Ope . push (s[i]);
				}
			}
		}
	}
	//最后如果栈不为空，则出栈并输出到字符串
	while (! Ope . empty ()) {
		char ch = Ope . top ();
		Ope . pop ();
		str += ch;
	}
	/*cout << "中缀表达式为：" << s << endl << endl;
	cout << "后缀表达式为：" << str << endl << endl;*/

	return str;
}

//后缀表达式转nfa
NFA StrToNfa (string s) {
	stack<NFA> NfaStack;
	for (int i = 0; i < s . size (); i ++) {
		if (isValid (s[i])) {
			//字母s[i]位于状态状态转化弧上
			NFA n = creatNFA (nfaStateNum);
			nfaStateNum += 2;    //NFA状态总数加2
			addArc (n . head, n . tail, s[i]);
			NfaStack . push (n);
		} else if (s[i] == '*') {
			//如果是闭包运算则将栈顶nfa进行修改
			NFA n1 = creatNFA (nfaStateNum);
			nfaStateNum += 2;
			NFA n2 = NfaStack . top ();
			NfaStack . pop ();

			//改变相应弧对应的连接
			//n1可以通过ε转移到n1之前或之后的状态
			addArc (n2 . tail, n1 . head);
			addArc (n2 . tail, n1 . tail);
			addArc (n1 . head, n2 . head);
			addArc (n1 . head, n1 . tail);

			NfaStack . push (n1);    //将修改后的NFA入栈
		} else if (s[i] == '|') {
			//遇到或运算符从栈顶弹出两个nfa
			//从栈中弹出两个nfa，栈顶为n2，次栈顶为n1
			NFA n1, n2;
			n2 = NfaStack . top ();
			NfaStack . pop ();

			n1 = NfaStack . top ();
			NfaStack . pop ();

			NFA n = creatNFA (nfaStateNum);
			nfaStateNum += 2;

			//n可以通过ε转移到n1或n2状态
			addArc (n . head, n1 . head);
			addArc (n . head, n2 . head);
			addArc (n1 . tail, n . tail);
			addArc (n2 . tail, n . tail);

			NfaStack . push (n);
		} else if (s[i] == '&') {
			NFA n1, n2, n;
			n2 = NfaStack . top ();
			NfaStack . pop ();

			n1 = NfaStack . top ();
			NfaStack . pop ();

			addArc (n1 . tail, n2 . head);
			n . head = n1 . head;
			n . tail = n2 . tail;

			NfaStack . push (n);
		}
	}

	return NfaStack . top ();//栈顶即为NFA的头
}

//打印NFA
void printNFA (NFA nfa) {
	ofstream out ("out.txt");
	//cout << "-------------------     NFA    -------------------" << endl << endl;
	//cout << "NFA共有" << nfaStateNum << "个状态" << endl;
	//cout << "初态为" << nfa . head -> index << "，终态为" << nfa . tail -> index << endl << endl << "转移函数为：" << endl;
	out << nfa . tail -> index << endl;    //终态
	out << nfa . head -> index << endl;

	for (int i = 0; i < nfaStateNum; i ++) {
		if (NfaStates[i] . input != '#') {
			//cout << NfaStates[i] . index << "-->'" << NfaStates[i] . input << "'-->" << NfaStates[i] . chTrans;
			out << NfaStates[i] . index << " " << NfaStates[i] . input << " " << NfaStates[i] . chTrans << " ";
			if (NfaStates[i] . chTrans != nfa . tail -> index) {
				out << 0 << endl;
			} else
				out << 1 << endl;
		}
		IntSet::iterator it;//输出该状态经过ε到达的状态
		for (it = NfaStates[i] . epTrans . begin (); it != NfaStates[i] . epTrans . end (); it ++) {
			//cout << NfaStates[i] . index << "-->'" << ' ' << "'-->" << *it;
			out << NfaStates[i] . index << " " << '@' << " " << *it << " ";
			if (*it != nfa . tail -> index) {
				out << 0 << endl;
			} else {
				out << 1 << endl;
			}
		}
		//cout << endl;
	}
	out . close ();
}