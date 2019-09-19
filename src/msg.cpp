//===-- msg.cpp - Header for the console application -----------*- C++ -*--===//
//
//                                eBus deamon
//
// This file is distributed under the Open Source License.
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Entry point for the console application. This application acts as a
/// deamon process and read out the data telegrams of an ebus system.
///
//===----------------------------------------------------------------------===//
#include "msg.h"


// Hilfsfunktion: String kopieren (um #include<cstring> zu vermeiden)
static void copy(unsigned char *ziel, const unsigned char *quelle)
{
	while((*ziel++ = *quelle++));
}


// Hilfsfunktion: Länge eines C-Strings ermitteln
static size_t laenge(const unsigned char *s)
{
	size_t sl = 0;
	while(*s++) ++sl;
	return sl;
}

inline std::tm localtime_xp(std::time_t timer)
{
    std::tm bt {};
#if defined(__unix__)
    localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
    localtime_s(&bt, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&timer);
#endif
    return bt;
}


void C_ebus_message::check_items(void)
{
	vector<C_item>::iterator it;
	
	for (it = item_vec.begin(); it != item_vec.end(); ++it)
	{
		it->set_val(start);
	}
}


C_ebus_message::C_ebus_message()
: len(0), start(new unsigned char[1]), strbuf(new char [1])
{
	*start = 0x00;
}


C_ebus_message::C_ebus_message(const unsigned char *s)
: len(laenge(s)), start(new unsigned char[len+1])
{
	copy(start, s);
}


C_ebus_message::C_ebus_message(const C_ebus_message &m)
: len(m.len), start(new unsigned char[len+1])
{
	copy(start, m.start);
}


// Überladener Zuweisungsoperator
C_ebus_message& C_ebus_message::operator= (const C_ebus_message& src)
{
    // Zuweisung auf sich selbst abprüfen!
    if (this == &src)
        return *this;

    delete [] start;
    start = new unsigned char[src.len+1];
    copy(start, src.start);
	len = src.len;
    return *this;
}


C_ebus_message::~C_ebus_message()
{
	delete [] start;
	delete [] strbuf;
}


void C_ebus_message::assign(C_ebus_message const &m)
{
	unsigned char *p = new unsigned char[m.len+1];
	copy(p, m.start);
	delete [] start;
	len = m.len;
	start = p;
}


void C_ebus_message::assign(unsigned char const *s)
{
	size_t L = laenge(s);
	unsigned char *p = new unsigned char[L+1];
	copy(p, s);
	delete [] start;
	len = L;
	start = p;
}


void C_ebus_message::assign(const char *s)
{
	size_t L = laenge((unsigned char*)s);
	unsigned char *p = new unsigned char[L+1];
	copy(p, (unsigned char*)s);
	delete [] start;
	len = L;
	start = p;
}


void C_ebus_message::assign(const int amount, ...)
{
	int i;
	va_list vl;
	va_start(vl, amount);
	unsigned char uch_tmp_val;

	delete [] start;

	len = (size_t) amount;
	start = new unsigned char[amount];

	for(i=0; i<amount; i++)
	{
		uch_tmp_val = (unsigned char) va_arg(vl, int);
		start[i] = uch_tmp_val;
	}
	va_end(vl);
}


void C_ebus_message::add(const unsigned char uch_val)
{
    //Save timestamp at beginnig of message
	if (len == 0)
		gettimeofday(&timestamp, 0);

	//printf("start ::add\n");
	start = (unsigned char *) realloc(start, ++len * sizeof(unsigned char));
	//printf("end ::add\n");
	start[len-1] = uch_val;
}


void C_ebus_message::undo_subst(void)
{
	size_t i, j;
	for (i=0; i<=len; i++)
	{
		if (start[i] == 0xA9)
		{
			start[i] = start[i] + start[i+1];
			for (j=i+1; j<=len-1; j++)
				start[j] = start[j+1];
			len--;

			//printf("start ::undo_subst\n");
			start = (unsigned char *) realloc(start, len * sizeof(unsigned char));
			//printf("end ::undo_subst\n");
		}
	}
}


void C_ebus_message::analyse(void)
{
    check_items();
}


void C_ebus_message::write_db(MYSQL *db)
{
	bool b_ret_val;
	char q[1024];
    
    string table;
    
    vector<C_item>::iterator it;

	if (db != NULL)
	{
		for(it = item_vec.begin(); it != item_vec.end(); ++it)
		{
			if(it->check_new_val())
			{
				table = it->get_db_table();

				sprintf(q, "INSERT INTO %s (%s, %s) VALUES ('%s', %.1f)",
					table.c_str(),
					"date",
					"val",
					get_timestamp_cstr(), it->get_val());
					
				b_ret_val = mysql_query(db, q);
				
				//printf("%s\n", q);

				printf("inserting new %s into db table %s (ret val %d)\n", it->get_name().c_str(), table.c_str(), b_ret_val);
				it->reset_new_val();
			}
		}
	}
}


size_t C_ebus_message::get_len(void)
{
	return len;
}


void C_ebus_message::clear(void)
{
	//printf("start ::clear\n");
	start = (unsigned char *) realloc(start, 0 * sizeof(unsigned char));
	//printf("end ::clear\n");
	len = 0;
}


void C_ebus_message::print(void)
{
	size_t i;

	printf("%s| ", get_timestamp_cstr());

	for(i=0; i<len; i++)
	{
		if (start[i] < 0x10)
			printf("0");
		printf("%x ", start[i]);
	}
	printf("\n");
}


unsigned char C_ebus_message::get_N(void)
{
	return start[4];
}


unsigned char C_ebus_message::get_CRC(void)
{
	return 0;
}


unsigned char C_ebus_message::calc_CRC(void)
{
	return 0;
}


bool C_ebus_message::get_CRC_Ok(void)
{
	return 0;
}


char *C_ebus_message::get_timestamp_cstr(void)
{
	time_t nowtime;
	char tmbuf[64];
	char buf[128];
	struct tm *nowtm;

	nowtime = timestamp.tv_sec;
	nowtm = localtime(&nowtime);

	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, timestamp.tv_usec);

	//printf("start ::get_timestamp\n");
	strbuf = (char *) realloc(strbuf, sizeof buf * sizeof(char));
	//printf("end ::get_timestamp\n");
	strcpy(strbuf, buf);

	return strbuf;
}


void C_ebus_message::register_item(C_item::param par)
{
	//printf("register item %s\n", par.name.c_str());
	C_item item;
	
	//printf("set_par %s\n", par.name.c_str());
	item.set_par(par);
	
	//printf("pushback %s\n", par.name.c_str());
	item_vec.push_back(item);
}