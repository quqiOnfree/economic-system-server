#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>

#include "head_msg.hpp"
#include "log_system.hpp"
#include "./json/json.h"
#include "boost/filesystem.hpp"
#include "O2.hpp"
#include "q_random.hpp"
#include "inicpp.hpp"
#include "classes.hpp"

#pragma comment(lib, "q_random.lib")

namespace socket
{
#	include <WinSock2.h>
#	pragma comment(lib, "ws2_32.lib")
}

using namespace std;
namespace fs = boost::filesystem;

short ser_port = 1234;            //�˿�
char ser_addr[32] = "127.0.0.1";  //��ַ

inline void run(socket::SOCKET conn, socket::sockaddr_in addr, int th_num);
inline int id_th();
inline void del_th(int th_num);

static socket::SOCKET s;                       //��ʼ��ȫ��socket
static thread threads[1024]{ thread() };       //��ʼ�����߳��б�
static int thread_num[1024]{ 0 };              //��ʼ���߳�id�б�

static Log log_s;
static boost::filesystem::path s_path(boost::filesystem::current_path());

static map<string, money> moneys;
static map<string, user> users;

inline void reload_config() //��д ./config/data/json
{
	std::fstream json;
	json.open(string("./config/data.json"), std::ios_base::out);
	log_s.write("�ļ� ./config/data.json �����ɹ�");
	Json::Value data;

	data["server_address"] = "127.0.0.1";
	data["server_port"] = 1234;

	json << data.toStyledString() << endl;

	log_s.write("����д ./config/data.json");
	json.close();
	system("pause");
	exit(0);
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	if (boost::filesystem::create_directory("./logs"))
	{
		cout << "�ļ��� ./logs �����ɹ�" << endl;
	}
	log_s = Log();
	log_s.write("��־ϵͳ ���سɹ�");

	if (boost::filesystem::create_directory("./config"))
	{
		log_s.write("�ļ��� ./config �����ɹ�");
		reload_config();
	}
	else
	{
		if (boost::filesystem::exists("./config/data.json"))
		{
			Json::Value value_j;
			Json::Reader reader;
			string data;
			string data_2;

			std::fstream json;
			json.open(string("./config/data.json"),ios_base::in);
			while (!json.eof())
			{
				json >> data_2;
				data += data_2;
			}
			json.close();
			
			if (reader.parse(data,value_j))
			{
				if (!value_j["server_address"].isNull())
				{
					strcpy(ser_addr, value_j["server_address"].asCString());
				}
				else
				{
					reload_config();
				}
				if (!value_j["server_port"].isNull())
				{
					ser_port = value_j["server_port"].asInt();
				}
				else
				{
					reload_config();
				}
			}
			else
			{
				reload_config();
			}
		}
		else
		{
			reload_config();
		}
	}
	log_s.write("�ļ� ./config/data.json �������");

	if (boost::filesystem::create_directory("./data"))
	{
		log_s.write("�ļ��� ./data �����ɹ�");
	}

	log_s.write("���ڼ��ػ�����...");
	ini::ifile ini_money ("./data/money.ini");
	vector<string> ini_sec;
	ini_money.getSection(ini_sec);
	for (auto i = ini_sec.begin();i!=ini_sec.end();i++)
	{
		money mon((*i).c_str());
		moneys[(*i)] = mon;
	}
	log_s.write("���Ҽ������");
	ini_sec.~vector();
	ini_money.~ifile();

	log_s.write("���ڼ����û���...");
	ini::ifile ini_user("./data/user.ini");
	vector<string> ini_us;
	ini_user.getSection(ini_us);
	for (auto i = ini_us.begin(); i != ini_us.end(); i++)
	{
		user us((*i).c_str(), moneys);
		users[(*i)] = us;
	}
	log_s.write("�û��������");
	ini_us.~vector();
	ini_user.~ifile();

	socket::WSADATA wsaData;
	if (socket::WSAStartup(514, &wsaData))
	{
		log_s.write("wsadata δ���سɹ�");
		return 0;
	}
	else
	{
		log_s.write("wsadata ���سɹ�");
	}

	log_s.write("���ڴ��� socket�׽���");
	s = socket::socket(AF_INET, SOCK_STREAM, socket::IPPROTO_TCP);

	socket::sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;                          //ģʽ
	serv_addr.sin_addr.s_addr = socket::inet_addr(ser_addr);         //ip
	serv_addr.sin_port = socket::htons(ser_port);                    //�˿�

	log_s.write("socket�׽��� �����ɹ�");

	socket::bind(s, (socket::sockaddr*)&serv_addr, sizeof(serv_addr));
	if (socket::listen(s, 1024))
	{
		log_s.write("����ʧ��");
		return 0;
	}
	else
	{
		log_s.write("��ʼ����");
	}

	log_s.write("�������� socket���ղ���");
	socket::sockaddr_in client_addr;
	std::memset(&client_addr, 0, sizeof(client_addr));
	int len = sizeof(client_addr);
	socket::SOCKET conn;
	thread th;
	int can;

	log_s.write("socket���ղ��� �����ɹ�");
	log_s.write("ȫ�������Ѿ����\n");
	while (true)
	{
		conn = socket::accept(s, (socket::sockaddr*)&client_addr, &len);
		while (true)
		{
			can = id_th();
			if (can)
			{
				threads[can - 1] = thread(run, conn, client_addr, can - 1);//�����߳�
				threads[can - 1].detach();
				break;
			}
		}
		std::memset(&client_addr, 0, sizeof(client_addr));
	}
	system("pause");
	return 0;
}

inline void del_th(int th_num)
{
	//log_s.write("�����̣߳��߳�id��", to_string(th_num));
	thread_num[th_num] = 0;
	return;
}

inline int id_th()//��ȡ����id������
{
	for (int i = 0; i < sizeof(thread_num) / sizeof(int); i++)
	{
		if (!thread_num[i])
		{
			thread_num[i] = 1;
			//log_s.write("�����̣߳��߳�id��", to_string(i));
			return i+1;
		}
	}
	return NULL;
}

inline void send_msg(const string& json_head, const string& json_msg, const socket::SOCKET conn, const socket::sockaddr_in addr)
{
	make_head str;
	Json::Value er;
	char* strs;

	er[json_head] = json_msg;
	str = make_head(er.toStyledString().c_str());
	strs = str.get_head_str();
	socket::send(conn, strs, str.get_len(), 0);
	log_s.write("server>>", inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), ">>", er.toStyledString().c_str());

	delete[] strs;
}

inline void run(socket::SOCKET conn, socket::sockaddr_in addr, int th_num)//ÿ�����̵Ĵ���
{
	char msg[1025] = { 0 };//��ʼ��
	char* strs;
	size_t size;
	unpack_head recv_msg;
	make_head str;
	Json::Reader reader;
	Json::Value value_j;

	log_s.write(inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), "���������������߳�id:", to_string(th_num).c_str());
	
	while (true)
	{
		size = socket::recv(conn, msg, sizeof(msg), 0);//�����Ƿ����ӶϿ�
		if (!size || !strlen(msg))
		{
			log_s.write(inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), "�Ͽ�����");
			break;
		} 

		try
		{
			recv_msg = unpack_head(msg, size);//������Ϣͷ
		}
		catch (const std::bad_alloc&)
		{
			send_msg("error", "�㷢��ʲô����", conn, addr);
			return;
		}

		if (!recv_msg.can_get())
		{
			do
			{
				size = socket::recv(conn, msg, sizeof(msg), 0);
				recv_msg.add_str(msg, size);
			} while (!recv_msg.can_get());
		}

		do //�����ﴦ������ awa
		{
			strs = recv_msg.get_char();
			strcpy(msg, strs);
			delete[] strs;
			try
			{
				reader.parse(string(msg), value_j);
				if (!value_j["creat_new_money"].isNull())
				{
					if (!(value_j["creat_new_money"]["value"].isNull() || value_j["creat_new_money"]["name"].isNull() || value_j["creat_new_money"]["creator"].isNull()))
					{
						if (moneys.count(value_j["creat_new_money"]["name"].asCString()))
						{

							send_msg("error", "�û����Ѿ�������", conn, addr);
							break;
						}
						money new_m(value_j["creat_new_money"]["name"].asCString(), atof(value_j["creat_new_money"]["value"].asString().c_str()), value_j["creat_new_money"]["creator"].asCString());
						moneys[value_j["creat_new_money"]["name"].asCString()] = new_m;
						send_msg("succeed", "�������ҳɹ�", conn, addr);
						for (auto i = users.begin(); i != users.end(); i++)
						{
							ini::ifile ini("./data/user.ini");
							ini.write(i->first, new_m.get_name(), "0");
						}
						moneys[value_j["creat_new_money"]["name"].asCString()].reload();
					}
					else
					{
						send_msg("error", "��������", conn, addr);
						break;
					}
				}

				else if (!value_j["delete_money"].isNull())
				{
					if (!value_j["delete_money"]["name"].isNull())
					{
						if (!moneys.count(value_j["delete_money"]["name"].asCString()))
						{

							send_msg("error", "û�иû���", conn, addr);
							break;
						}
						moneys[value_j["delete_money"]["name"].asCString()].delete_file();
						for (auto i = users.begin(); i != users.end(); i++)
						{
							ini::ifile ini("./data/user.ini");
							ini.deleteKey(i->first.c_str(), value_j["delete_money"]["name"].asCString());
						}
						moneys.erase(value_j["delete_money"]["name"].asCString());
						send_msg("succeed", "ɾ�����ҳɹ�", conn, addr);
					}
					else
					{
						send_msg("error", "��������", conn, addr);
						break;
					}
				}

				else if (!value_j["creat_new_user"].isNull())
				{
					if (!(value_j["creat_new_user"]["name"].isNull() || value_j["creat_new_user"]["uuid"].isNull()))
					{
						if (users.count(value_j["creat_new_user"]["name"].asCString()))
						{
							send_msg("error", "���û��Ѿ�������", conn, addr);
							break;
						}
						user us(value_j["creat_new_user"]["name"].asCString(), value_j["creat_new_user"]["uuid"].asCString(), moneys);
						users[value_j["creat_new_user"]["name"].asCString()] = us;
						send_msg("succeed", "�����û��ɹ�", conn, addr);
					}
					else
					{
						send_msg("error", "��������", conn, addr);
						break;
					}
				}

				else if (!value_j["get_exchange_rate"].isNull())
				{
					if (!(value_j["get_exchange_rate"]["money_name1"].isNull() || value_j["get_exchange_rate"]["money_name2"].isNull()))
					{
						if (!moneys.count(value_j["get_exchange_rate"]["money_name1"].asCString()))
						{
							send_msg("error", "û�д˻���", conn, addr);
							break;
						}
						if (!moneys.count(value_j["get_exchange_rate"]["money_name2"].asCString()))
						{
							send_msg("error", "û�д˻���", conn, addr);
							break;
						}

						send_msg("exchange_rate", to_string(moneys[value_j["get_exchange_rate"]["money_name1"].asCString()].get_value()/moneys[value_j["get_exchange_rate"]["money_name2"].asCString()].get_value()), conn, addr);

					}
					else
					{
						send_msg("error", "��������", conn, addr);
						break;
					}
				}
				
				else if (!value_j["change_user_money"].isNull())
				{
					if (!(value_j["change_user_money"]["name"].isNull() || value_j["change_user_money"]["money_name"].isNull() || (value_j["change_user_money"]["add"].isNull() && value_j["change_user_money"]["remove"].isNull() && value_j["change_user_money"]["set"].isNull())))
					{
						if (!users.count(value_j["change_user_money"]["name"].asCString()))
						{
							send_msg("error", "û�д��û�", conn, addr);
							break;
						}
						if (!moneys.count(value_j["change_user_money"]["money_name"].asCString()))
						{
							send_msg("error", "û�д˻���", conn, addr);
							break;
						}
						if (!value_j["change_user_money"]["add"].isNull())
						{
							try
							{
								ini::ifile ini("./data/user.ini");
								int siz = ini.getInt(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString());
								ini.write(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString(), (siz + value_j["change_user_money"]["add"].asInt()));
								send_msg("succeed", "�޸ĳɹ�", conn, addr);
							}
							catch (const std::exception&)
							{
								send_msg("error", "����ĳЩ��������", conn, addr);
								break;
							}
						}
						else if (!value_j["change_user_money"]["set"].isNull())
						{
							try
							{
								ini::ifile ini("./data/user.ini");
								ini.write(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString(), (value_j["change_user_money"]["set"].asInt()));
								send_msg("succeed", "�޸ĳɹ�", conn, addr);
							}
							catch (const std::exception&)
							{
								send_msg("error", "����ĳЩ��������", conn, addr);
								break;
							}
						}
						else if (!value_j["change_user_money"]["remove"].isNull())
						{
							try
							{
								ini::ifile ini("./data/user.ini");
								int siz = ini.getInt(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString());
								ini.write(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString(), (siz-value_j["change_user_money"]["remove"].asInt()));
								send_msg("succeed", "�޸ĳɹ�", conn, addr);
							}
							catch (const std::exception&)
							{
								send_msg("error", "����ĳЩ��������", conn, addr);
								break;
							}
						}
					}
					else
					{
						send_msg("error", "��������", conn, addr);
						break;
					}
				}
				else
				{
					send_msg("error", "û�д�ָ��", conn, addr);
					break;
				}
			}
			catch (const std::exception&)
			{
				send_msg("error", "�������޷���������", conn, addr);
			}

		} while (recv_msg.can_get());

		std::memset(&msg, 0, sizeof(msg));//��msg����ȫ����Ϊ0
	}
	socket::closesocket(conn);//�ر�socket
	del_th(th_num);//������Ҫɾ���Ľ���id
	return;
}