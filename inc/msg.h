//===-- msg.h - Header for the console application -------------*- C++ -*-===//
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
#ifndef _msg_h_
#define _msg_h_

#include "item.h"

#include <cstring>
#include <stdlib.h>
#include <cstdarg>

#include <vector>
#include <sys/time.h>
#include <ctime>
#include <mutex>
#include <mariadb/mysql.h>

using namespace std;

inline std::tm localtime_xp(std::time_t timer);

class C_ebus_message{

private:
    size_t len;            //Length of message
    unsigned char *start;  //Pointer to message
    timeval timestamp;     //Timestamp of message at receiving time

    char *strbuf;          //Pointer to string. Return value of function
                           //get_timestamp_cstr()

    vector <C_item> item_vec;

    void check_items(void);

public:
    C_ebus_message(); //Default-Konstruktor
    C_ebus_message(const unsigned char *);     //allg. Konstruktor
    C_ebus_message(const C_ebus_message &);    //Kopierkonstruktor

    //Überladene Operator-Funktion
    C_ebus_message& operator= (const C_ebus_message& src);

    ~C_ebus_message();                         //Destruktor

    void assign(const C_ebus_message &m);
    void assign(const unsigned char *);
    void assign(const char *);
    void assign(const int, ...);

    void add(const unsigned char);
    void undo_subst(void);
    void analyse(void);
    void write_db(MYSQL *);
    size_t get_len(void);
    void clear(void);
    void print(void);


    unsigned char get_N(void);
    unsigned char get_CRC(void);
    unsigned char calc_CRC(void);
    bool get_CRC_Ok(void);

    char *get_timestamp_cstr(void );

    void register_item(const C_item::param *);
};

#endif //_msg_h_
