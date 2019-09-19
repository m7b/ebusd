// RS-232:
// https://ebus-wiki.org/lib/exe/fetch.php/ebus/spec_prot_7_v1_6_3_d.pdf

#define BOOST_TEST_MODULE rs232
#include <boost/test/included/unit_test.hpp>

#include <rs232.h>

BOOST_AUTO_TEST_CASE(Serial_Port)
{
    std::string str_comport_number;
    int comport_number;

#if defined(__linux__) || defined(__FreeBSD__)   /* Linux & FreeBSD */

    str_comport_number = "ttyS0";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 0 for "/dev/ttyS0"
    BOOST_CHECK_EQUAL(comport_number, 0);

    str_comport_number = "ttyAMA0";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 22 for "/dev/ttyAMA0"
    BOOST_CHECK_EQUAL(comport_number, 22);

    str_comport_number = "cuaU3";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 22 for "/dev/cuaU3"
    BOOST_CHECK_EQUAL(comport_number, 37);

#else  /* windows */

    str_comport_number = "COM1";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 0 for "\\\\.\\COM1"
    BOOST_CHECK_EQUAL(comport_number, 0);

    str_comport_number = "COM3";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 2 for "\\\\.\\COM3"
    BOOST_CHECK_EQUAL(comport_number, 2);

    str_comport_number = "COM9";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 8 for "\\\\.\\COM9"
    BOOST_CHECK_EQUAL(comport_number, 8);

    str_comport_number = "COM10";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 9 for "\\\\.\\COM10"
    BOOST_CHECK_EQUAL(comport_number, 9);

    str_comport_number = "COM15";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 14 for "\\\\.\\COM15"
    BOOST_CHECK_EQUAL(comport_number, 14);

    str_comport_number = "COM16";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 15 for "\\\\.\\COM16"
    BOOST_CHECK_EQUAL(comport_number, 15);

    //Ab hier wird es seltsam

    str_comport_number = "COM17";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 16 for "\\\\.\\COM17"
    BOOST_CHECK_EQUAL(comport_number, 16);

    str_comport_number = "COM18";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 17 for "\\\\.\\COM18"
    BOOST_CHECK_EQUAL(comport_number, 17);

    str_comport_number = "COM19";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 18 for "\\\\.\\COM19"
    BOOST_CHECK_EQUAL(comport_number, 18);

    str_comport_number = "COM20";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 19 for "\\\\.\\COM20"
    BOOST_CHECK_EQUAL(comport_number, 19);

    str_comport_number = "COM31";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 30 for "\\\\.\\COM31"
    BOOST_CHECK_EQUAL(comport_number, 30);

    str_comport_number = "COM32";
    comport_number     = RS232_GetPortnr(str_comport_number.c_str());  //should be 31 for "\\\\.\\COM32"
    BOOST_CHECK_EQUAL(comport_number, 31);

#endif
}