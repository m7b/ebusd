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

#include <ebusd/serial.h>

C_serial2_com::C_serial2_com(const std::shared_ptr<spdlog::logger> log)
: port(0), baudrate(2400), mode(new char [4])
{
    strcpy(mode, "8N1\0");
	this->log = log;
}

C_serial2_com::~C_serial2_com()
{
	delete mode;
}

void C_serial2_com::set_port(const int port)
{
    this->port = port;
}

int C_serial2_com::com_open(void)
{
    int ret_val_fct;
    int ret_val;
    
    ret_val_fct = RS232_OpenComport(port, baudrate, mode);

    if (ret_val_fct > 0)
		log->error("Can not open port {}!", port);
    else
        ret_val = 1;

    return ret_val;
}

int C_serial2_com::read(unsigned char *buf, int size)
{
    return RS232_PollComport(port, buf, size);
}