#pragma once
#ifndef CLASSES_HPP
#define CLASSES_HPP

#include <string>
#include <map>
#include <ctime>
#include "inicpp.hpp"

class money
{
private:
	std::string name;
	long double value;
	std::string creat_time;
	std::string creator;
	ini::ifile inifile;
public:
	money() : name("")
	{
		value = 1;
	}

	money(const char* name_) : name(name_)
	{
		inifile = ini::ifile("./data/money.ini");
		value = inifile.getDouble(name_, "value");
		creat_time = inifile.getCStr(name_, "creat_time");
		creator = inifile.getString(name_, "creator");
	}

	money(const char* name_, double value_, const char* creator_) : name(name_), creator(creator_)
	{
		inifile = ini::ifile("./data/money.ini");
		inifile.write(name_, "value", std::to_string(value_).c_str());
		inifile.write(name_, "creator", creator_);

		value = inifile.getDouble(name_, "value");

		time_t now = time(NULL);
		char* dt = ctime(&now);
		creat_time = dt;
		inifile.write(name_, "creat_time", dt);
	}

	~money()
	{

	}

	const void delete_file()
	{
		inifile.deleteSection(name);
	}

	const std::string& get_name() const
	{
		return name;
	}

	const long double get_value() const
	{
		return static_cast<long double>(value);
	}

	void reload()
	{
		money(name);
	}
};

class user
{
private:
	std::string name;
	std::string uuid;
	std::string creat_time;
	ini::ifile inifile;
public:
	user() :name(""), uuid("")
	{

	}
	user(const std::string& name_) : name(name_)
	{
		inifile = ini::ifile("./data/user.ini");
		uuid = inifile.getCStr(name_, "uuid");
		creat_time = inifile.getCStr(name_, "creat_time");
	}
	user(const std::string& name_, const std::map<std::string, money>& mons) : name(name_)
	{
		inifile = ini::ifile("./data/user.ini");
		uuid = inifile.getCStr(name_, "uuid");
		creat_time = inifile.getCStr(name_, "creat_time");
		for (auto i = mons.begin(); i != mons.end(); i++)
		{
			if (!inifile.getInt(name_, i->first))
			{
				inifile.write(name_, i->first, 0);
			}
		}
	}
	user(const std::string& name_, const std::string& uuid_) : name(name_), uuid(uuid_)
	{
		inifile = ini::ifile("./data/user.ini");
		inifile.write(name, "uuid", uuid);

		time_t now = time(NULL);
		char* dt = ctime(&now);
		creat_time = dt;
		inifile.write(name_, "creat_time", dt);
	}
	user(const std::string& name_, const std::string& uuid_, const std::map<std::string, money>& mons) : name(name_), uuid(uuid_)
	{
		inifile = ini::ifile("./data/user.ini");
		inifile.write(name, "uuid", uuid);

		time_t now = time(NULL);
		char* dt = ctime(&now);
		creat_time = dt;
		inifile.write(name_, "creat_time", dt);
		for (auto i = mons.begin(); i != mons.end(); i++)
		{
			inifile.write(name_, (*i).first, 0);
		}
	}
	bool deletekey(const std::string& str)
	{
		return inifile.deleteKey(name, str);
	}
	bool deleteuser()
	{
		return inifile.deleteSection(name);
	}
	bool add_money(const money& mon, const int number)
	{
		int mon_size = inifile.getInt(name, mon.get_name());
		return inifile.write(name, mon.get_name(), std::to_string(mon_size + number));
	}
	bool remove_money(const money& mon, const int number)
	{
		int mon_size = inifile.getInt(name, mon.get_name());
		return inifile.write(name, mon.get_name(), std::to_string(mon_size - number));
	}
	bool set_money(const money& mon, const int number)
	{
		return inifile.write(name, mon.get_name(), std::to_string(number));
	}
	user& operator=(const user& us)
	{
		if (this == &us)
		{
			return *this;
		}

		name = us.name;
		uuid = us.uuid;
		creat_time = us.creat_time;
		inifile = ini::ifile(name.c_str());
		return *this;
	}
};

#endif // !CLASSES_HPP
