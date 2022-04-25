#include<iostream>
#include<cstring>
#include<string>
#include<stack>
#include<fstream>
#include<vector>
#include<set>
#include<queue>
#include "nfa_dfa.h"
#include "simDFA.h"

int main () {
	initSim();
	string str = "begin x := 9 : if x > 9 then x := 2 * x + 1 / 3 ; end #";
	vector<string> src;
	string s;
	istringstream is(str);
	while(is >> s) {
		src. push_back (s);
		cout << s << " ";
	}
	cout << endl;
	for(auto i : src) {
		cout << "(" << check(i) << ", " << i << ") ";
	}
	return 0;
}

