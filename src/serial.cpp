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

#include "serial.h"

C_serial_com::C_serial_com(const std::shared_ptr<spdlog::logger> log)
: fd(0), oldtio(new struct termios), newtio(new struct termios)
{
    port = "";
	
	this->log  = log;

	/* set new default port settings for canonical input processing */
	newtio->c_cflag = B2400 | CS8 | CLOCAL | CREAD;
	newtio->c_iflag = 0;
	newtio->c_oflag = 0;
	newtio->c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);
	newtio->c_cc[VMIN] = 1;
	newtio->c_cc[VTIME] = 0;
}

C_serial_com::~C_serial_com()
{
	delete oldtio;
	delete newtio;
}

void C_serial_com::set_port(const std::string port)
{
    this->port = port;
}

int C_serial_com::com_open(void)
{
	fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
		log->error("Can not open port {}!", port.c_str());
	
    return fd;
}

void C_serial_com::set_async(void)
{
    /* Make the file descriptor asynchronous (the manual page says only
	 * O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    if (fd != 0)
        fcntl(fd, F_SETFL, O_NONBLOCK);
}

void C_serial_com::save_current_port_settings(void)
{
	/* save current port settings */
    if (fd != 0)
        tcgetattr(fd, oldtio);
}

void C_serial_com::set_port_settings(int i_setting)
{
    struct termios param;

    if (fd != 0)
    {
        //flush data received in buffer but not read.
        tcflush(fd, TCIFLUSH);

        //choose parameter to set
        switch (i_setting)
        {
            case 0:
                param = *oldtio;
                break;

            case 1:
                param = *newtio;
                break;
        }

        //set choosen parameter
        tcsetattr(fd, TCSANOW, &param);
    }
}

