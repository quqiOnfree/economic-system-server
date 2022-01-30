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
#include "boost/filesystem.hpp"
#include "./json/json.h"
#include "O2.hpp"
#include "inicpp.hpp"
#include "classes.hpp"
#include "input_cmd.hpp"

#include "endecode.hpp"

#pragma comment(lib, "q_random.lib")

namespace socket
{
#	include <WinSock2.h>
#	pragma comment(lib, "ws2_32.lib")
}

using namespace std;
namespace fs = boost::filesystem;

unsigned short ser_port = 1234;            //�˿�
char ser_addr[32] = "127.0.0.1";           //��ַ
unsigned int thread_number = 1024;

inline void run(socket::SOCKET conn, socket::sockaddr_in addr, int th_num);
inline int id_th();
inline void del_th(int th_num);

socket::SOCKET s;                       //��ʼ��ȫ��socket
thread* threads;                        //��ʼ�����߳��б�
int* thread_num{ 0 };                   //��ʼ���߳�id�б�
bool msg_head_mode = true;              //������ر���Ϣͷ
char version[32] = "v0.0.1";			       //�汾
short num_version = 0;                         //���ְ汾
char oldest_version[32] = "v0.0.1";            //���ϰ汾
short oldest_num_version = 0;                  //�������ְ汾

Log log_s;                            //��־ϵͳ

fs::path s_path(fs::current_path());    //����λ��

map<string, money> moneys;              //�ܻ���
map<string, user> users;                //���û�
vector<string> ips;                     //�����ip

inline void reload_config() //��д ./config/data/json
{
	std::fstream json;
	json.open(string("./config/data.json"), std::ios_base::out);
	log_s.write("�ļ� ./config/data.json �����ɹ�");
	Json::Value data;

	data["server_address"] = "127.0.0.1";
	data["server_port"] = 1234;
	data["server_thread_number"] = 1024;
	data["server_msg_head_mode"] = true;
	data["server_whitelist"] = Json::Value(Json::arrayValue);

	json << data.toStyledString() << endl;

	log_s.write("����д ./config/data.json");
	json.close();
	system("pause");
	exit(-1);
}

void init()
{
	static bool thread_reload = false;
	static bool cmd_reload = false;

	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	rgb_init();
	if (fs::create_directory("./logs"))
	{
		cout << "�ļ��� ./logs �����ɹ�" << endl;
	}
	log_s = Log();
	log_s.write("��־ϵͳ ���سɹ�");

	if (fs::create_directory("./config"))
	{
		log_s.write("�ļ��� ./config �����ɹ�");
		reload_config();
	}
	else
	{
		if (fs::exists("./config/data.json"))
		{
			Json::Value value_j;
			Json::Reader reader;
			string data;
			string data_2;

			std::fstream json;
			json.open(string("./config/data.json"), ios_base::in);
			while (!json.eof())
			{
				json >> data_2;
				data += data_2;
			}
			json.close();

			if (reader.parse(data, value_j))
			{
				if (!value_j["server_address"].isNull() && value_j["server_address"].isString())//����ip
				{
					strcpy(ser_addr, value_j["server_address"].asCString());
				}
				else
				{
					reload_config();
				}
				if (!value_j["server_port"].isNull() && value_j["server_port"].isInt())//���ö˿�
				{
					ser_port = value_j["server_port"].asInt();
				}
				else
				{
					reload_config();
				}
				if (!value_j["server_thread_number"].isNull() && value_j["server_thread_number"].isInt())//�����߳���
				{
					if (!thread_reload)
					{
						thread_number = value_j["server_thread_number"].asUInt();
						threads = new thread[thread_number]{ thread() };
						thread_num = new int[thread_number] {0};
						thread_reload = true;
					}
				}
				else
				{
					reload_config();
				}

				if (!value_j["server_msg_head_mode"].isNull() && value_j["server_msg_head_mode"].isBool())//������Ϣͷ�Ƿ���
				{
					if (value_j["server_msg_head_mode"].asBool())
					{
						msg_head_mode = true;
					}
					else
					{
						msg_head_mode = false;
					}
				}
				else
				{
					reload_config();
				}

				if (!value_j["server_whitelist"].isNull() && value_j["server_whitelist"].isArray())//���ð�����ip
				{
					for (auto i = value_j["server_whitelist"].begin(); i != value_j["server_whitelist"].end(); i++)
					{
						if ((*i).isString())
						{
							ips.push_back((*i).asString());
						}
						else
						{
							reload_config();
						}
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
		else
		{
			reload_config();
		}
	}
	log_s.write("�ļ� ./config/data.json �������");

	if (fs::create_directory("./data"))
	{
		log_s.write("�ļ��� ./data �����ɹ�");
	}

	log_s.write("���ڼ��ػ�����...");
	ini::ifile ini_money("./data/money.ini");
	vector<string> ini_sec;
	ini_money.getSection(ini_sec);
	for (auto i = ini_sec.begin(); i != ini_sec.end(); i++)
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

	if (!cmd_reload)
	{
		log_s.write("���ڼ���ָ��ϵͳ...");
		thread th = thread(input_system);
		th.detach();
		log_s.write("ָ��ϵͳ�������");
		cmd_reload = true;
	}

	log_s.write("��������rsa��Կ...");
	auto pk = getRsaKey(2048);
	writeRsaKey(pk);
	EVP_PKEY_free(pk);
	log_s.write("����rsa��Կ�ɹ�");
}

int init_socket()
{
	socket::WSADATA wsaData;
	if (socket::WSAStartup(514, &wsaData))
	{
		log_s.write("wsadata δ���سɹ�");
		return -1;
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

	log_s.write("����ip��ַ��", socket::inet_ntoa(serv_addr.sin_addr), ":", to_string(short(socket::ntohs(serv_addr.sin_port))).c_str());

	socket::bind(s, (socket::sockaddr*)&serv_addr, sizeof(serv_addr));
	if (socket::listen(s, 1024))
	{
		log_s.write("����ʧ��");
		return -1;
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
	return 0;
}

int main()
{
	init();
	return init_socket();
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

inline string deln(const string str)
{
	string str_ = str;
	string str__;
	for (auto i = str_.begin(); i != str_.end(); i++)
	{
		if (*i != '\n' && *i != '\t')
		{
			str__ += *i;
		}
	}
	return str__;
}

inline void send_msg(const string& json_head, const Json::Value& json_msg, const socket::SOCKET conn, const socket::sockaddr_in addr)//�����Ǵ�����Ϣ�ĵط�
{
	if (msg_head_mode)
	{
		make_head str;
		Json::Value er;
		char* strs;

		er[json_head] = json_msg;
		str = make_head(deln(Json::FastWriter().write(er)).c_str());
		strs = str.get_head_str();
		socket::send(conn, strs, str.get_len(), 0);
		log_s.write("server>>", inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), ">>", deln(Json::FastWriter().write(er)).c_str());

		delete[] strs;
	}
	else
	{
		Json::Value er;
		er[json_head] = json_msg;
		socket::send(conn, deln(Json::FastWriter().write(er)).c_str(), strlen(deln(Json::FastWriter().write(er)).c_str()), 0);
		log_s.write("server>>", inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), ">>", deln(Json::FastWriter().write(er)).c_str());
	}
}

inline bool process(char* msg, const socket::SOCKET conn, const socket::sockaddr_in addr)//��������
{
	Json::Reader reader;
	Json::Value value_j;
	try
	{
		reader.parse(msg, value_j);
		if (!value_j["creat_new_money"].isNull())//��������
		{
			if (!(value_j["creat_new_money"]["value"].isNull() || value_j["creat_new_money"]["name"].isNull() || value_j["creat_new_money"]["creator"].isNull()))
			{
				if (moneys.count(value_j["creat_new_money"]["name"].asCString()))
				{

					send_msg("error", "�û����Ѿ�������", conn, addr);
					return false;
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
				return false;
			}
		}

		else if (!value_j["delete_money"].isNull())//ɾ������
		{
			if (!value_j["delete_money"]["name"].isNull())
			{
				if (!moneys.count(value_j["delete_money"]["name"].asCString()))
				{

					send_msg("error", "û�иû���", conn, addr);
					return false;
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
				return false;
			}
		}

		else if (!value_j["creat_new_user"].isNull())//�����û�
		{
			if (!(value_j["creat_new_user"]["name"].isNull() || value_j["creat_new_user"]["uuid"].isNull() || value_j["creat_new_user"]["creator"].isNull()))
			{
				if (users.count(value_j["creat_new_user"]["name"].asCString()))
				{
					send_msg("error", "���û��Ѿ�������", conn, addr);
					return false;
				}
				user us(value_j["creat_new_user"]["name"].asCString(), value_j["creat_new_user"]["uuid"].asCString(), value_j["creat_new_user"]["creator"].asCString(), moneys);
				users[value_j["creat_new_user"]["name"].asCString()] = us;
				send_msg("succeed", "�����û��ɹ�", conn, addr);
			}
			else
			{
				send_msg("error", "��������", conn, addr);
				return false;
			}
		}

		else if (!value_j["get_exchange_rate"].isNull())//��ȡ����
		{
			if (!(value_j["get_exchange_rate"]["money_name1"].isNull() || value_j["get_exchange_rate"]["money_name2"].isNull()))
			{
				if (!moneys.count(value_j["get_exchange_rate"]["money_name1"].asCString()))
				{
					send_msg("error", "û�д˻���", conn, addr);
					return false;
				}
				if (!moneys.count(value_j["get_exchange_rate"]["money_name2"].asCString()))
				{
					send_msg("error", "û�д˻���", conn, addr);
					return false;
				}

				send_msg("exchange_rate", to_string(moneys[value_j["get_exchange_rate"]["money_name1"].asCString()].get_value() / moneys[value_j["get_exchange_rate"]["money_name2"].asCString()].get_value()), conn, addr);

			}
			else
			{
				send_msg("error", "��������", conn, addr);
				return false;
			}
		}

		else if (!value_j["get_user_info"].isNull())//��ȡ�û��ĸ����ҳ�����
		{
			if (!value_j["get_user_info"]["name"].isNull())
			{
				if (!users.count(value_j["get_user_info"]["name"].asCString()))
				{
					send_msg("error", "û�д��û�", conn, addr);
					return false;
				}

				Json::Value js;
				ini::ifile ini("./data/user.ini");
				for (auto i = moneys.begin(); i != moneys.end(); i++)
				{
					js["money"][i->first] = to_string(ini.getDouble(value_j["get_user_info"]["name"].asCString(), i->first));
				}

				js["uuid"] = ini.getString(value_j["get_user_info"]["name"].asString(), "uuid");
				js["creat_time"] = ini.getString(value_j["get_user_info"]["name"].asString(), "creat_time");
				js["creator"] = ini.getString(value_j["get_user_info"]["name"].asString(), "creator");

				send_msg("user_info", js, conn, addr);
			}
			else
			{
				send_msg("error", "��������", conn, addr);
				return false;
			}
		}

		else if (!value_j["get_version"].isNull())//��ȡ����˰汾��
		{
			Json::Value v;
			v["str_version"] = string(version);
			v["int_version"] = to_string(num_version);
			v["oldest_str_version"] = string(oldest_version);
			v["oldest_int_version"] = to_string(oldest_num_version);
			send_msg("version", v, conn, addr);
		}

		else if (!value_j["change_user_money"].isNull())//�ı��û�ĳ���ҵĳ���������set,add,remove����ָ��
		{
			if (!(value_j["change_user_money"]["name"].isNull() || value_j["change_user_money"]["money_name"].isNull() || (value_j["change_user_money"]["add"].isNull() && value_j["change_user_money"]["remove"].isNull() && value_j["change_user_money"]["set"].isNull())))
			{
				if (!users.count(value_j["change_user_money"]["name"].asCString()))
				{
					send_msg("error", "û�д��û�", conn, addr);
					return false;
				}
				if (!moneys.count(value_j["change_user_money"]["money_name"].asCString()))
				{
					send_msg("error", "û�д˻���", conn, addr);
					return false;
				}
				if (!value_j["change_user_money"]["add"].isNull())//add����
				{
					if (!value_j["change_user_money"]["add"].isString())
					{
						send_msg("error", "add�Ĳ���������string����", conn, addr);
						return false;
					}
					try
					{
						ini::ifile ini("./data/user.ini");
						double siz = ini.getDouble(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString());
						ini.write(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString(), to_string(siz + atof(value_j["change_user_money"]["add"].asCString())));
						send_msg("succeed", "�޸ĳɹ�", conn, addr);
					}
					catch (...)
					{
						send_msg("error", "����ĳЩ��������", conn, addr);
						return false;
					}
				}
				else if (!value_j["change_user_money"]["set"].isNull())//set����
				{
					if (!value_j["change_user_money"]["set"].isString())
					{
						send_msg("error", "set�Ĳ���������string����", conn, addr);
						return false;
					}
					try
					{
						ini::ifile ini("./data/user.ini");
						ini.write(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString(), to_string(atof(value_j["change_user_money"]["set"].asCString())));
						send_msg("succeed", "�޸ĳɹ�", conn, addr);
					}
					catch (...)
					{
						send_msg("error", "����ĳЩ��������", conn, addr);
						return false;
					}
				}
				else if (!value_j["change_user_money"]["remove"].isNull())//remove����
				{
					if (!value_j["change_user_money"]["remove"].isString())
					{
						send_msg("error", "remove�Ĳ���������string����", conn, addr);
						return false;
					}
					try
					{
						ini::ifile ini("./data/user.ini");
						double siz = ini.getDouble(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString());
						ini.write(value_j["change_user_money"]["name"].asCString(), value_j["change_user_money"]["money_name"].asCString(), to_string(siz - atof(value_j["change_user_money"]["remove"].asCString())));
						send_msg("succeed", "�޸ĳɹ�", conn, addr);
					}
					catch (...)
					{
						send_msg("error", "����ĳЩ��������", conn, addr);
						return false;
					}
				}
			}
			else
			{
				send_msg("error", "��������", conn, addr);
				return false;
			}
		}
		else
		{
			send_msg("error", "û�д�ָ��", conn, addr);
			return false;
		}
	}
	catch (...)
	{
		send_msg("error", "�������޷���������ò�Ʒ����˴���Ĳ���", conn, addr);
		return false;
	}
	value_j.clear();
	return true;
}

inline void run(socket::SOCKET conn, socket::sockaddr_in addr, int th_num)//ÿ�����̵Ĵ���
{
	char msg[1025] = { 0 };//��ʼ��
	char* strs;
	size_t size;
	unpack_head recv_msg;
	make_head str;

	log_s.write(inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), "���������������߳�id:", to_string(th_num).c_str());
	
	while (true)
	{
		size = socket::recv(conn, msg, sizeof(msg), 0);//�����Ƿ����ӶϿ�
		if (!size || !strlen(msg))
		{
			log_s.write(inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), "�Ͽ�����");
			break;
		} 

		if (!count(ips.begin(), ips.end(), string(inet_ntoa(addr.sin_addr))))
		{
			send_msg("error", "�㲻�ڷ�����������", conn, addr);
			log_s.write(inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), "���ڰ��������Զ��Ͽ�����");
			socket::closesocket(conn);
			return;
		}

		if (msg_head_mode)
		{
			try
			{
				recv_msg = unpack_head(msg, size);//������Ϣͷ
			}
			catch (...)
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

			do
			{
				strs = recv_msg.get_char();
				log_s.write(inet_ntoa(addr.sin_addr), ":", to_string(short(socket::ntohs(addr.sin_port))).c_str(), ">>server>>", strs);
				strcpy(msg, strs);
				delete[] strs;

				process(msg, conn, addr);

			} while (recv_msg.can_get());
		}
		else
		{
			process(msg, conn, addr);
		}

		std::memset(&msg, 0, sizeof(msg));//��msg����ȫ����Ϊ0
	}
	socket::closesocket(conn);//�ر�socket
	del_th(th_num);//������Ҫɾ���Ľ���id
	return;
}
