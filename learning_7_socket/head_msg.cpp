#define _CRT_SECURE_NO_WARNINGS

#include "head_msg.hpp"
#include <cstring>

void ch_head(char* head, const char* heads)//��Ϣͷ��ֵ
{
	strcpy(head, heads);
}

make_head::make_head()//��ʼ��Ϊ��
{
	str = "";
	len = str.size();
	head = new char[8]{ 0 };
}

make_head::make_head(const char* str_)//��ʼ��
{
	head = new char[8]{ 0 };
	str = str_;
	len = str.size();
	make_long ml {len};
	char heads[8]{ 0 };
	memcpy(&heads, &ml, sizeof(ml));
	ch_head(head, heads);
}

//make_head::make_head(const make_head& mk)//���ƹ��캯����û���Թ������ã�
//{
//	str = mk.str;
//	len = str.size();
//	make_long ml{ len };
//	char heads[8]{ 0 };
//	memcpy(&heads, &ml, sizeof(ml));
//	ch_head(head, heads);
//}

make_head::~make_head()//����
{
	delete[] head;
}

//make_head& make_head::operator=(const char* str_)//��ֵ��class��û���Թ������ã�
//{
//	str = str_;
//	len = str.size();
//	make_long ml{ len };
//	char heads[8]{ 0 };
//	memcpy(&heads, &ml, sizeof(ml));
//	ch_head(head, heads);
//	return *this;
//}

make_head& make_head::operator=(const make_head& mk)//��ֵ��class
{
	if (this == &mk)
	{
		return *this;
	}
	str = mk.str;
	strcpy(head, mk.head);
	len = mk.len;
	return *this;
}

//make_head& make_head::operator+(const make_head& mk) const//�ӷ���ûɶ�ã�
//{
//	make_head newm;
//	newm.str = str + mk.str;
//	newm.len = str.size();
//	make_long ml{ newm.len };
//	char heads[8]{ 0 };
//	memcpy(&heads, &ml, sizeof(ml));
//	ch_head(newm.head, heads);
//	return newm;
//}

//std::ostream& operator<<(std::ostream& os, const make_head& mk)//��ӡ��ûɶ�ã�
//{
//	os << mk.head << mk.str;
//	return os;
//}

unsigned long long make_head::get_len() const//��ȡ����Ϣͷ��ĳ��ȣ����Ҫ��Ϣͷ�������ں��� ��ȥ8
{
	return (len + 8LL);
}

char* make_head::get_char() const//��ȡ������Ϣͷ������
{
	char* str_ = new char[str.size() + 1]{ 0 };
	strcpy(str_, str.c_str());
	return str_;
}

char* make_head::get_head() const//��ȡ��Ϣͷ
{
	return head;
}

char* make_head::get_head_str() const//��ȡ������Ϣͷ������
{
	char head_[9]{ 0 };
	char st;
	strcpy(head_, head);

	char* strs = new char[make_head::get_len()+1]{ 0 };
	strcpy(strs, head_);
	size_t length = len;
	for (size_t i = 0; i < length; i++)
	{
		st = str[i];
		strs[i+8] = st;
	}
	return strs;
}

unpack_head::unpack_head()//��ʼ��Ϊ��
{
	str = new char[1]{ 0 };
	ml.len = 0;
	len = 0;
}

unpack_head::unpack_head(const char* str_,size_t size)//���ó�ʼ������
{
	if (!size || !strlen(str_))//�������Ϊ��
	{
		*this = unpack_head();
		return;
	}
	len = size - 8;
	size_t length = 8;
	char strs[1024]{ 0 };

	for (size_t i = 0; i < length; i++)
	{
		head[i] = str_[i];
	}
	memcpy(&ml, &head, sizeof(head));

	for (size_t i = 8; i < size; i++)
	{
		strs[i - 8] = str_[i];
	}

	str = new char[size - 8]{ 0 };
	length = size - 8;
	for (size_t i = 0; i < length; i++)
	{
		str[i] = strs[i];
	}
}

unpack_head::~unpack_head()//��������
{
	delete[] str;
}

void unpack_head::add_str(const char* str_,size_t size)//���������
{
	char* str_n = new char[size + len]{ 0 };

	for (size_t i = 0; i < len; i++)
	{
		str_n[i] = str[i];
	}
	for (size_t i = 0; i < size; i++)
	{
		str_n[len + i] = str_[i];
	}

	len = size + len;

	delete[] str;
	str = str_n;
}

bool unpack_head::can_get() const//�ж��Ƿ��ܻ�ȡ����
{
	if (len>=ml.len)
	{
		return true;
	}
	return false;
}

char* unpack_head::get_char()//��ȡ��Ϣͷ֮�������
{
	if (unpack_head::can_get())
	{
		char* str_ = new char[ml.len + 1]{ 0 };
		for (size_t i = 0; i < ml.len; i++)
		{
			str_[i] = str[i];
		}

		if (len - ml.len)
		{
			char* str_n = new char[len - ml.len]{ 0 };
			for (size_t i = 0; i < len - ml.len; i++)
			{
				str_n[i] = str[i + ml.len];
			}
			delete[] str;
			str = str_n;
		}

		len = len - ml.len;

		if (len > 8)//�����ȡ������len��Ϊ8����ȡ�µ���Ϣͷ
		{
			*this = unpack_head(str, len);
		}

		return str_;
	}
	char* str_ = new char[1]{ 0 };
	return str_;
}

unpack_head& unpack_head::operator=(const unpack_head& uh) //��ֵclass
{
	if (&uh==this)
	{
		return *this;
	}

	strcpy(head, uh.head);
	delete[] str;
	str = new char[uh.len]{ 0 };
	for (size_t i = 0; i < uh.len; i++)
	{
		str[i] = uh.str[i];
	}
	ml.len = uh.ml.len;
	len = uh.len;
	return *this;
}
