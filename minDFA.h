#pragma once
#include<set>
#include <iostream>

#define MAX 128

using namespace std;

typedef set<int> IntSet;
typedef set<char> CharSet;

struct Edge			/*定义DFA的转换弧*/
{

	char input;			/*弧上的值*/
	int Trans;			/*弧所指向的状态号*/
};

struct DfaState		/*定义DFA状态*/
{

	bool isEnd;			/*是否为终态，是为true，不是为false*/

	int index;			/*DFA状态的状态号*/
	IntSet closure;		/*NFA的ε-move()闭包*/

	int edgeNum;		/*DFA状态上的射出弧数*/
	Edge Edges[10];		/*DFA状态上的射出弧*/
};

DfaState DfaStates[MAX];		/*DFA状态数组*/
int dfaStateNum = 0;			/*DFA状态总数*/

struct DFA			/*定义DFA结构*/
{

	int startState;				/*DFA的初态*/

	set<int> endStates;			/*DFA的终态集*/
	set<char> terminator;		/*DFA的终结符集*/

	int trans[MAX][26];		/*DFA的转移矩阵*/
};

IntSet s[MAX];					/*划分出来的集合数组*/
DfaState minDfaStates[MAX];		/*minDfa状态数组*/

int minDfaStateNum = 0;			/*minDfa的状态总数，同时也是划分出的集合数*/

struct stateSet			/*划分状态集*/
{

	int index;			/*该状态集所能转换到的状态集标号*/
	IntSet s;			/*该状态集中的dfa状态号*/
};

/*当前划分总数为count，返回状态n所属的状态集标号i*/
int findSetNum(int count, int n)
{

	for (int i = 0; i < count; i++)
	{
		if (s[i].count(n))
		{
			return i;
		}
	}
}

/*最小化DFA*/
DFA minDFA(DFA d)
{

	int i, j;
	cout << endl << "*************     minDFA     **************" << endl << endl;

	DFA minDfa;
	minDfa.terminator = d.terminator;		/*把dfa的终结符集赋给minDfa*/

	memset(minDfa.trans, -1, sizeof(minDfa.trans));		/*初始化minDfa转移矩阵*/

	/*做第一次划分，即将终态与非终态分开*/
	bool endFlag = true;					/*判断dfa的所有状态是否全为终态的标志*/
	for (i = 0; i < dfaStateNum; i++)	/*遍历dfa状态数组*/
	{
		if (DfaStates[i].isEnd == false)			/*如果该dfa状态不是终态*/
		{

			endFlag = false;						/*标志应为false*/
			minDfaStateNum = 2;						/*第一次划分应该有两个集合*/

			s[1].insert(DfaStates[i].index);		/*把该状态的状态号加入s[1]集合中*/
		}
		else									/*如果该dfa状态是终态*/
		{
			s[0].insert(DfaStates[i].index);		/*把该状态的状态号加入s[0]集合中*/
		}
	}

	if (endFlag)					/*如果标志为真，则所有dfa状态都是终态*/
	{
		minDfaStateNum = 1;			/*第一次划分结束应只有一个集合*/
	}

	bool cutFlag = true;		/*上一次是否产生新的划分的标志*/
	while (cutFlag)				/*只要上一次产生新的划分就继续循环*/
	{

		int cutCount = 0;			/*需要产生新的划分的数量*/
		for (i = 0; i < minDfaStateNum; i++)			/*遍历每个划分集合*/
		{

			CharSet::iterator it;
			for (it = d.terminator.begin(); it != d.terminator.end(); it++)		/*遍历dfa的终结符集*/
			{

				int setNum = 0;				/*当前缓冲区中的状态集个数*/
				stateSet temp[20];			/*划分状态集“缓冲区”*/

				IntSet::iterator iter;
				for (iter = s[i].begin(); iter != s[i].end(); iter++)		/*遍历集合中的每个状态号*/
				{

					bool epFlag = true;			/*判断该集合中是否存在没有该终结符对应的转换弧的状态*/
					for (j = 0; j < DfaStates[*iter].edgeNum; j++)		/*遍历该状态的所有边*/
					{

						if (DfaStates[*iter].Edges[j].input == *it)		/*如果该边的输入为该终结符*/
						{

							epFlag = false;			/*则标志为false*/

							/*计算该状态转换到的状态集的标号*/
							int transNum = findSetNum(minDfaStateNum, DfaStates[*iter].Edges[j].Trans);

							int curSetNum = 0;			/*遍历缓冲区，寻找是否存在到达这个标号的状态集*/
							while ((temp[curSetNum].index != transNum) && (curSetNum < setNum))
							{
								curSetNum++;
							}

							if (curSetNum == setNum)		/*缓冲区中不存在到达这个标号的状态集*/
							{

								/*在缓冲区中新建一个状态集*/
								temp[setNum].index = transNum;		/*该状态集所能转换到的状态集标号为transNum*/
								temp[setNum].s.insert(*iter);		/*把当前状态添加到该状态集中*/

								setNum++;		/*缓冲区中的状态集个数加一*/
							}
							else			/*缓冲区中存在到达这个标号的状态集*/
							{
								temp[curSetNum].s.insert(*iter);	/*把当前状态加入到该状态集中*/
							}
						}
					}

					if (epFlag)		/*如果该状态不存在与该终结符对应的转换弧*/
					{

						/*寻找缓冲区中是否存在转换到标号为-1的状态集
						这里规定如果不存在转换弧，则它所到达的状态集标号为-1*/
						int curSetNum = 0;
						while ((temp[curSetNum].index != -1) && (curSetNum < setNum))
						{
							curSetNum++;
						}

						if (curSetNum == setNum)			/*如果不存在这样的状态集*/
						{

							/*在缓冲区中新建一个状态集*/
							temp[setNum].index = -1;			/*该状态集转移到的状态集标号为-1*/
							temp[setNum].s.insert(*iter);		/*把当前状态加入到该状态集中*/

							setNum++;		/*缓冲区中的状态集个数加一*/
						}
						else			/*缓冲区中存在到达这个标号的状态集*/
						{
							temp[curSetNum].s.insert(*iter);	/*把当前状态加入到该状态集中*/
						}
					}
				}

				if (setNum > 1)	/*如果缓冲区中的状态集个数大于1，表示同一个状态集中的元素能转换到不同的状态集，则需要划分*/
				{

					cutCount++;		/*划分次数加一*/

					/*为每组划分创建新的dfa状态*/
					for (j = 1; j < setNum; j++)		/*遍历缓冲区，这里从1开始是将第0组划分留在原集合中*/
					{

						IntSet::iterator t;
						for (t = temp[j].s.begin(); t != temp[j].s.end(); t++)
						{

							s[i].erase(*t);						/*在原来的状态集中删除该状态*/
							s[minDfaStateNum].insert(*t);		/*在新的状态集中加入该状态*/
						}

						minDfaStateNum++;		/*最小化DFA状态总数加一*/
					}
				}
			}
		}

		if (cutCount == 0)		/*如果需要划分的次数为0，表示本次不需要进行划分*/
		{
			cutFlag = false;
		}
	}

	/*遍历每个划分好的状态集*/
	for (i = 0; i < minDfaStateNum; i++)
	{

		IntSet::iterator y;
		for (y = s[i].begin(); y != s[i].end(); y++)		/*遍历集合中的每个元素*/
		{

			if (*y == d.startState)			/*如果当前状态为dfa的初态，则该最小化DFA状态也为初态*/
			{
				minDfa.startState = i;
			}

			if (d.endStates.count(*y))		/*如果当前状态是终态，则该最小化DFA状态也为终态*/
			{

				minDfaStates[i].isEnd = true;
				minDfa.endStates.insert(i);		/*将该最小化DFA状态加入终态集中*/
			}

			for (j = 0; j < DfaStates[*y].edgeNum; j++)		/*遍历该DFA状态的每条弧，为最小化DFA创建弧*/
			{

				/*遍历划分好的状态集合，找出该弧转移到的状态现在属于哪个集合*/
				for (int t = 0; t < minDfaStateNum; t++)
				{
					if (s[t].count(DfaStates[*y].Edges[j].Trans))
					{

						bool haveEdge = false;		/*判断该弧是否已经创建的标志*/
						for (int l = 0; l < minDfaStates[i].edgeNum; l++)	/*遍历已创建的弧*/
						{					/*如果该弧已经存在*/
							if ((minDfaStates[i].Edges[l].input == DfaStates[*y].Edges[j].input) && (minDfaStates[i].Edges[l].Trans == t))
							{
								haveEdge = true;		/*标志为真*/
							}
						}

						if (!haveEdge)		/*如果该弧不存在，则创建一条新的弧*/
						{

							minDfaStates[i].Edges[minDfaStates[i].edgeNum].input = DfaStates[*y].Edges[j].input;	/*弧的值与DFA的相同*/
							minDfaStates[i].Edges[minDfaStates[i].edgeNum].Trans = t;	/*该弧转移到的状态为这个状态集的标号*/

							minDfa.trans[i][DfaStates[*y].Edges[j].input - 'a'] = t;	/*更新转移矩阵*/

							minDfaStates[i].edgeNum++;		/*该状态的弧的数目加一*/
						}

						break;
					}
				}
			}
		}
	}

	return minDfa;
}