//===-- serial.h - Header definitions for the console application -*- C++ -*-=//
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

#ifndef _serial_h_
#define _serial_h_

#include <string>
#include <termios.h>
#include <fcntl.h>

class C_serial_com{

private:

    int fd;                     //File descriptor
    struct termios *oldtio;
    struct termios *newtio;
    std::string port;

public:

    C_serial_com();     //Konstruktor
    ~C_serial_com();    //Destruktor

    void set_port(std::string);
    int com_open(void);
    void set_async(void);
    void save_current_port_settings(void);
    void set_port_settings(int);

};

#endif
