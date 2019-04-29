//===-- ebus.h - Header for the console application -------------*- C++ -*-===//
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
#ifndef _main_h_
#define _main_h_

#include "msg.h"
#include "item.h"

#include <rs232.h>

#include <signal.h>
#include <sys/types.h>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

struct deamon_settings
{
    //serial port settings
    std::string ser_port;

    //database settings
	std::string db_server;
	std::string db_name;
	std::string db_user_name;
	std::string db_user_password;
    
    //parameter of relevant items to fetch
    std::vector<C_item::param> other_item;
    
    //methods to load and save
	int load(const std::string &filename);
	void save(const std::string &filename);
};

void close_logger(void);
void signal_handler(int);
int rs232_open(deamon_settings *);
void rs232_close(void);
void mysql_connect(deamon_settings *);
void mysql_disconnect(void);
void register_items(C_ebus_message *, deamon_settings *, MYSQL *);
void check_bus_state(C_ebus_message *, unsigned char *, int);
void save_example(deamon_settings *);
void my_sleep_ms(unsigned int);

#endif //_main_h_