// RS-232:
// https://ebus-wiki.org/lib/exe/fetch.php/ebus/spec_prot_7_v1_6_3_d.pdf

#define BOOST_TEST_MODULE rs232
#include <boost/test/included/unit_test.hpp>

#include <rs232.h>

BOOST_AUTO_TEST_CASE(Serial_Port)
{
    std::string str_comport_number;

#if defined(__linux__) || defined(__FreeBSD__)   /* Linux & FreeBSD */

    str_comport_number = "ttyAMA0";
    int comport_number = RS232_GetPortnr(str_comport_number.c_str());  //should be 22 for "/dev/ttyAMA0"
    BOOST_CHECK_EQUAL(comport_number, 22);

#else  /* windows */

    str_comport_number = "COM3";
    int comport_number = RS232_GetPortnr(str_comport_number.c_str());  //should be 2 for "\\\\.\\COM3"
    BOOST_CHECK_EQUAL(comport_number, 2);

#endif
}