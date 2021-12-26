#include "input_cmd.hpp"

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

#include "log_system.hpp"

using namespace std;

void rgb_init() {										// 初始化
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);		//输入句柄
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);		//输出句柄
	DWORD dwInMode, dwOutMode;
	GetConsoleMode(hIn, &dwInMode);						//获取控制台输入模式
	GetConsoleMode(hOut, &dwOutMode);					//获取控制台输出模式
	dwInMode |= 0x0200;									//更改
	dwOutMode |= 0x0004;
	SetConsoleMode(hIn, dwInMode);						//设置控制台输入模式
	SetConsoleMode(hOut, dwOutMode);					//设置控制台输出模式
}

void rgb_set_wr(int wr, int wg, int wb) {
	printf("\033[38;2;%d;%d;%dm", wr, wg, wb);
}

void rgb_set_bg(int br, int bg, int bb) {
	printf("\033[48;2;%d;%d;%dm", br, bg, bb);
}

vector<string> split(const char* str,const size_t size_)
{
	vector<string> re;
	char name[1024]{ 0 };
	long long size = 0;
	for (long long i = 0; i < size_ - 1; i++)
	{
		if (!str[i] || str[i] == 32)
		{
			if (str[i + 1])
			{
				size = 0;
				re.push_back(name);
				memset(&name, 0, sizeof(name));
			}
			
		}
		else
		{
			name[size] = str[i];
			size++;
		}
	}
	re.push_back(name);

	return re;
}

#include "classes.hpp"

extern char version[32];
extern void init();
extern map<string, money> moneys;              //总货币
extern map<string, user> users;                //总用户

void input_system()
{
	char str[1024]{ 0 };
	vector<string> cmd;
	size_t size = 0;
	char s = 0;
	while (true)
	{
		size = 0;
		s = 0;

		try
		{
			while (true)
			{
				s = cin.get();
				if (s == 10)
				{
					break;
				}
				str[size] = s;
				size++;
			}
			if (!cin || cin.fail() && cin.eof())
			{
				cin.clear();
				while (!isspace(cin.get())) continue;
				rgb_set_wr(255, 0, 0);
				cout << "输入了奇怪的指令，引发错误" << endl;
				rgb_set_wr(204, 204, 204);
				continue;
			}
			cmd = split(str, sizeof(str));
			
			if (cmd[0] == "help")
			{
				rgb_set_wr(255,246,143);
				cout << "\thelp\t\t\t--帮助" << endl
					<< "\tversion\t\t\t--版本" << endl
					<< "\treload\t\t\t--重载" << endl
					<< "\tlist\t\t\t--列表" << endl;
				rgb_set_wr(204, 204, 204);
			}
			else if (cmd[0] == "version")
			{
				rgb_set_wr(192,255,62);
				cout << "此服务端的版本是：" << version << endl;
				rgb_set_wr(204, 204, 204);
			}
			else if (cmd[0] == "reload")
			{
				moneys.clear();
				users.clear();
				init();
				rgb_set_wr(0, 255, 0);
				cout << "重载成功！" << endl;
				rgb_set_wr(204, 204, 204);
			}
			else if (cmd[0] == "list")
			{
				if (cmd.size() > 1)
				{
					if (cmd[1] == "user")
					{
						rgb_set_wr(192, 255, 62);
						cout << '\t' << '[';
						for (auto i = users.begin(); i != users.end(); i++)
						{
							cout << i->first << ',';
						}
						cout << ']' << endl;
						rgb_set_wr(204, 204, 204);
					}
					else if (cmd[1] == "money")
					{
						rgb_set_wr(192, 255, 62);
						cout << '\t' << '[';
						for (auto i = moneys.begin(); i != moneys.end(); i++)
						{
							cout << i->first << ',';
						}
						cout << ']' << endl;
						rgb_set_wr(204, 204, 204);
					}
				}
				else
				{
					rgb_set_wr(255, 246, 143);
					cout << "\tlist user\t\t--用户列表" << endl
						<< "\tlist money\t\t--货币列表" << endl;
					rgb_set_wr(204, 204, 204);
				}
			}
			else
			{
				rgb_set_wr(255, 0, 0);
				cout << "没有此指令" << endl;
				rgb_set_wr(204, 204, 204);
			}
			memset(&str, 0, sizeof(str));
		}
		catch (...)
		{
			rgb_set_wr(255, 0, 0);
			cout << "没有此指令" << endl;
			rgb_set_wr(204, 204, 204);
		}
	}
}
