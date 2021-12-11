#include "Log_system.hpp"

using namespace std;

Log::Log()
{
	time_t* t_t = new time_t;
	*t_t = time(NULL);
	tm* time_ = localtime(t_t);
	name = "./logs/" + to_string(1900 + time_->tm_year) 
		+ "." + to_string(time_->tm_mon + 1) + "." 
		+ to_string(time_->tm_mday) + ".log";
	//file.open(name, A);
	delete t_t;
	have_write_time = false;
	have_open = false;
}

Log::Log(const char* file_name) : name(file_name)
{
	have_write_time = false;
	have_open = true;
}

Log::~Log()
{

}

Log& Log::operator=(const Log& l)
{
	if (this == &l)
	{
		return *this;
	}

	if (file)
	{
		fclose(file);
	}
	
	name = l.name;
	have_write_time = l.have_write_time;
	have_open = l.have_open;
	if (have_open)
	{
		file=fopen(name.c_str(),"a");
	}
	
	return *this;
}