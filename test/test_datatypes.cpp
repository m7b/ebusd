// Definition datatypes:
// https://ebus-wiki.org/lib/exe/fetch.php/ebus/spec_prot_7_v1_6_3_d.pdf

#define BOOST_TEST_MODULE datatypes
#include <boost/test/included/unit_test.hpp>

#include "item.h"

BOOST_AUTO_TEST_CASE(FloatBoolConversionTest)
{
    BOOST_CHECK_EQUAL(.0f, (float) false);
    BOOST_CHECK_EQUAL(1.0f, (float) true);	
    BOOST_CHECK_EQUAL(false, (bool) .0f);
    BOOST_CHECK_EQUAL(true, (bool) 1.0f);	
}

BOOST_AUTO_TEST_CASE(EndianTest)
{
    //https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program

    int num = 1;
    if(*(char *)&num == 1)
    {
        BOOST_TEST_MESSAGE("Little-Endian");
    }
    else
    {
        BOOST_TEST_MESSAGE("Big-Endian");
    }
}

BOOST_AUTO_TEST_CASE(Copy_Assign)
{
	C_item A;

    float soll = 3.14, act;
    A.set_val(soll);

    //Test copy constructor
	C_item B(A);
    act = B.get_val();
    BOOST_CHECK_EQUAL(soll, act);	

    //Test assign operator
	C_item C;
    C = B;

    act = C.get_val();
    BOOST_CHECK_EQUAL(soll, act);	
}

BOOST_AUTO_TEST_CASE(got_new_val)
{
    float val = 3.14;

	C_item A;
    BOOST_CHECK_EQUAL(A.check_new_val(), false);	
    A.set_val(val);
    BOOST_CHECK_EQUAL(A.check_new_val(), true);	
    A.reset_new_val();
    BOOST_CHECK_EQUAL(A.check_new_val(), false);	
}

BOOST_AUTO_TEST_CASE(parameters)
{
	C_item item;
	C_item::param par;

    std::string soll = "Zustand LDW";
    std::string ist;
    
    par.name = soll;
	item.set_par(par);

    ist = item.get_name();
    BOOST_CHECK_EQUAL(soll, ist);

    soll = "tTestTable";
    par.s_db_table = soll;
	item.set_par(par);

    ist = item.get_db_table();
    BOOST_CHECK_EQUAL(soll, ist);
}

BOOST_AUTO_TEST_CASE(Datatype_BIT)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iiihi"; // 7. Element = h = 104d = 0
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhhih"; // 7. Element = i = 105d = 1
	unsigned char cval2[10] = "\x03\xfe\x05\x03iiiBi"; // 7. Element = B =  66d = 0
	
	float val      = .0f;
    float last_val = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);
	
	
	item.set_val(cval0);
    val      = item.get_val(); // h = 0 false
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
	
	item.set_val(cval1);
    val      = item.get_val(); // i = 1 true
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(1.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval2);
    val      = item.get_val(); // B = 0 false
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(1.0f, last_val);

    C_item item2 = item;
    val      = item2.get_val(); // B = 0 false
    last_val = item2.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(1.0f, last_val);

    item2.set_val(12.32);
    val      = item2.get_val(); // B = 0 false
    last_val = item2.get_last_val();
    BOOST_CHECK_EQUAL(12.32f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
}

BOOST_AUTO_TEST_CASE(Datatype_UNSIGNED_CHAR)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iiihi"; // 7. Element = h = 104d = 0
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhhih"; // 7. Element = i = 105d = 1
	unsigned char cval2[10] = "\x03\xfe\x05\x03iiiBi"; // 7. Element = B =  66d = 0
	unsigned char cval3[10] = "\x03\xfe\x05\x03iii\x0i"; // 7. Element = B =  0d = 0
	unsigned char cval4[10] = "\x03\xfe\x05\x03iii\xffi"; // 7. Element = B =  255d = 0
	
	float val      = .0f;
    float last_val = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::UNSIGNED_CHAR;  //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);
	
	
	item.set_val(cval0);
    val      = item.get_val(); // h = 104d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(104.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval1);
    val      = item.get_val(); // i = 105d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(105.0f, val);
    BOOST_CHECK_EQUAL(104.0f, last_val);
	
	item.set_val(cval2);
    val      = item.get_val(); //  B = 66d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(66.0f, val);
    BOOST_CHECK_EQUAL(105.0f, last_val);
	
	item.set_val(cval3);
    val      = item.get_val(); //  0d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(66.0f, last_val);
	
	item.set_val(cval4);
    val      = item.get_val(); //  255d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(255.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
}

BOOST_AUTO_TEST_CASE(Datatype_SIGNED_CHAR)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iii\x0i";  // 7. Element = 0d
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhh\x7fi"; // 7. Element = 127d
	unsigned char cval2[10] = "\x03\xfe\x05\x03iii\x80i"; // 7. Element = 128d
	unsigned char cval3[10] = "\x03\xfe\x05\x03iii\xffi"; // 7. Element = 255d
	
	float val      = .0f;
    float last_val = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::SIGNED_CHAR;    //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);

	
	item.set_val(cval0);
    val      = item.get_val(); // 0d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval1);
    val      = item.get_val(); // 127d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(127.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval2);
    val      = item.get_val(); //  128d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(-128.0f, val);
    BOOST_CHECK_EQUAL(127.0f, last_val);
	
	item.set_val(cval3);
    val      = item.get_val(); //  255d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(-1.0f, val);
    BOOST_CHECK_EQUAL(-128.0f, last_val);
}


BOOST_AUTO_TEST_CASE(Datatype_BCD)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iii\x00i"; // 7. Element = 0x00
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhh\x01i"; // 7. Element = 0x01
	unsigned char cval2[10] = "\x03\xfe\x05\x03iii\x09i"; // 7. Element = 0x09
	unsigned char cval3[10] = "\x03\xfe\x05\x03iii\x10i"; // 7. Element = 0x10
	unsigned char cval4[10] = "\x03\xfe\x05\x03iii\x20i"; // 7. Element = 0x20
	unsigned char cval5[10] = "\x03\xfe\x05\x03iii\x90i"; // 7. Element = 0x90
	unsigned char cval6[10] = "\x03\xfe\x05\x03iii\x99i"; // 7. Element = 0x99
	unsigned char cval7[10] = "\x03\xfe\x05\x03iii\x0ai"; // 7. Element = 0x0a
	unsigned char cval8[10] = "\x03\xfe\x05\x03iii\xa0i"; // 7. Element = 0xa0
	
	float val      = .0f;
    float last_val = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::BCD;    //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);

	
	item.set_val(cval0);
    val      = item.get_val(); // 0x00
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval1);
    val      = item.get_val(); // 0x01
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(1.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval2);
    val      = item.get_val(); //  0x09
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(9.0f, val);
    BOOST_CHECK_EQUAL(1.0f, last_val);
	
	item.set_val(cval3);
    val      = item.get_val(); //  0x10
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(10.0f, val);
    BOOST_CHECK_EQUAL(9.0f, last_val);
	
	item.set_val(cval4);
    val      = item.get_val(); //  0x20
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(20.0f, val);
    BOOST_CHECK_EQUAL(10.0f, last_val);
	
	item.set_val(cval5);
    val      = item.get_val(); //  0x90
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(90.0f, val);
    BOOST_CHECK_EQUAL(20.0f, last_val);
	
	item.set_val(cval6);
    val      = item.get_val(); //  0x99
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(99.0f, val);
    BOOST_CHECK_EQUAL(90.0f, last_val);
	
  	item.set_val(cval7);
    val      = item.get_val(); //  0x0a
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(99.0f, last_val);
	
	item.set_val(cval8);
    val      = item.get_val(); //  0xa0
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(99.0f, last_val);
}

BOOST_AUTO_TEST_CASE(Datatype_DATA1B)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iii\x80i"; // 7. Element = 0x80 = -128d
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhh\x81h"; // 7. Element = 0x81 = -127d
	unsigned char cval2[10] = "\x03\xfe\x05\x03iii\xffi"; // 7. Element = 0xff = -1d
	unsigned char cval3[10] = "\x03\xfe\x05\x03iii\x00i"; // 7. Element = 0x00 = 0d
	unsigned char cval4[10] = "\x03\xfe\x05\x03iii\x01i"; // 7. Element = 0x01 = 1d
	unsigned char cval5[10] = "\x03\xfe\x05\x03iii\x7ei"; // 7. Element = 0x7e = 126d
	unsigned char cval6[10] = "\x03\xfe\x05\x03iii\x7fi"; // 7. Element = 0x7f = 127d
	
	float val      = .0f;
    float last_val = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA1B;        //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);


	item.set_val(cval0);
    val      = item.get_val(); // 0x80 = -128d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(-128.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval1);
    val      = item.get_val(); // 0x81 = -127d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(-127.0f, val);
    BOOST_CHECK_EQUAL(-128.0f, last_val);
	
	item.set_val(cval2);
    val      = item.get_val(); // 0xff = -1d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(-1.0f, val);
    BOOST_CHECK_EQUAL(-127.0f, last_val);
	
	item.set_val(cval3);
    val      = item.get_val(); // 0x00 = 0d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(-1.0f, last_val);
	
	item.set_val(cval4);
    val      = item.get_val(); // 0x01 = 1d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(1.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval5);
    val      = item.get_val(); // 0x7e = 126d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(126.0f, val);
    BOOST_CHECK_EQUAL(1.0f, last_val);
	
	item.set_val(cval6);
    val      = item.get_val(); // 0x7f = 127d
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(127.0f, val);
    BOOST_CHECK_EQUAL(126.0f, last_val);
}

BOOST_AUTO_TEST_CASE(Datatype_DATA1C)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iiihi"; // 7. Element = h = 104d /2 = 52
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhhih"; // 7. Element = i = 105d /2 = 52.5
	unsigned char cval2[10] = "\x03\xfe\x05\x03iiiBi"; // 7. Element = B =  66d /2 = 33
	unsigned char cval3[10] = "\x03\xfe\x05\x03iii\x0i"; // 7. Element = 0x0 /2    =  0
	unsigned char cval4[10] = "\x03\xfe\x05\x03iii\xffi"; // 7. Element =  255d /2 = 127.5
	
	float val      = .0f;
    float last_val = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA1C;  //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);


	item.set_val(cval0);
    val      = item.get_val(); // h = 104d /2 = 52
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(52.0f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
	
	item.set_val(cval1);
    val      = item.get_val(); // i = 105d /2 = 52.5
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(52.5f, val);
    BOOST_CHECK_EQUAL(52.0f, last_val);
	
	item.set_val(cval2);
    val      = item.get_val(); // 66d /2 = 33
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(33.0f, val);
    BOOST_CHECK_EQUAL(52.5f, last_val);
	
	item.set_val(cval3);
    val      = item.get_val(); // 0x0 /2    =  0
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(.0f, val);
    BOOST_CHECK_EQUAL(33.0f, last_val);
	
	item.set_val(cval4);
    val      = item.get_val(); // 255d /2 = 127.5
    last_val = item.get_last_val();
    BOOST_CHECK_EQUAL(127.5f, val);
    BOOST_CHECK_EQUAL(.0f, last_val);
}

BOOST_AUTO_TEST_CASE(Datatype_DATA2B)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iiihi"; // ab 7. Element: h = 104d, i = 105d => 104.41015625f
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhhih"; // ab 7. Element: i = 105d, h = 104d => 105.40625f
	unsigned char cval2[10] = "\x03\xfe\x05\x03iii\x00\x00"; // ab 7. Element: x00, x00 => 0.0
	unsigned char cval3[10] = "\x03\xfe\x05\x03iii\x00\x01"; // ab 7. Element: x00, x01 => 0.00390625 (1/256)
	unsigned char cval4[10] = "\x03\xfe\x05\x03iii\xff\xff"; // ab 7. Element: xff, xff => -0.00390625 (-1/256)
	unsigned char cval5[10] = "\x03\xfe\x05\x03iii\xff\x00"; // ab 7. Element: xff, x00 => -1.0
	unsigned char cval6[10] = "\x03\xfe\x05\x03iii\x80\x00"; // ab 7. Element: x80, x00 => -128.0 (Ersatzwert)
	unsigned char cval7[10] = "\x03\xfe\x05\x03iii\x80\x01"; // ab 7. Element: x80, x01 => -127.996
	unsigned char cval8[10] = "\x03\xfe\x05\x03iii\x7f\xff"; // ab 7. Element: x7f, xff => 127.996
	
	float val      = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA2B;         //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);


    // ************************************************************************
    // NOT_RELEVANT endian cases
    // ************************************************************************
	item.set_val(cval0);
    val = item.get_val(); // h = 104d / 105d
    BOOST_CHECK_EQUAL(-1.0f, val);

    // ************************************************************************
    // LITTLE_ENDIAN cases
    // ************************************************************************
    par.en_bo = C_item::LE;   //Byte order
	item.set_par(par);

	item.set_val(cval0);
    val = item.get_val(); // h = 104d, i = 105d => 104.41015625f
    BOOST_CHECK_EQUAL(104.41015625f, val);

	item.set_val(cval1);
    val = item.get_val(); // i = 105d, h = 104d => 105.40625f
    BOOST_CHECK_EQUAL(105.40625f, val);

	item.set_val(cval2);
    val = item.get_val(); // x00, x00 => 0.0
    BOOST_CHECK_EQUAL(.0f, val);

	item.set_val(cval3);
    val = item.get_val(); // x00, x01 => 0.00390625 (1/256)
    BOOST_CHECK_EQUAL(0.00390625f, val);

	item.set_val(cval4);
    val = item.get_val(); // xff, xff => -0.00390625 (-1/256)
    BOOST_CHECK_EQUAL(-0.00390625f, val);

	item.set_val(cval5);
    val = item.get_val(); // xff, x00 => -1.0
    BOOST_CHECK_EQUAL(-1.0f, val);

	item.set_val(cval6);
    val = item.get_val(); // x80, x00 => -128.0 (Ersatzwert)
    BOOST_CHECK_EQUAL(-128.0f, val);

	item.set_val(cval7);
    val = item.get_val(); // x80, x01 => -127.996
    BOOST_REQUIRE_CLOSE(-127.996f, val, 0.0001);

	item.set_val(cval8);
    val = item.get_val(); //  x7f, xff => 127.996
    BOOST_REQUIRE_CLOSE(127.996f, val, 0.0001);
    

    // ************************************************************************
    // BIG_ENDIAN cases
    // ************************************************************************
    par.en_bo = C_item::BE;   //Byte order
	item.set_par(par);
    
	unsigned char cval10[10] = "\x03\xfe\x05\x03iiiih"; // ab 7. Element: h = 104d, i = 105d => 104.41015625f
	unsigned char cval11[10] = "\x03\xfe\x05\x03hhhhi"; // ab 7. Element: i = 105d, h = 104d => 105.40625f
	unsigned char cval12[10] = "\x03\xfe\x05\x03iii\x00\x00"; // ab 7. Element: x00, x00 => 0.0
	unsigned char cval13[10] = "\x03\xfe\x05\x03iii\x01\x00"; // ab 7. Element: x01, x00 => 0.00390625 (1/256)
	unsigned char cval14[10] = "\x03\xfe\x05\x03iii\xff\xff"; // ab 7. Element: xff, xff => -0.00390625 (-1/256)
	unsigned char cval15[10] = "\x03\xfe\x05\x03iii\x00\xff"; // ab 7. Element: x00, xff => -1.0
	unsigned char cval16[10] = "\x03\xfe\x05\x03iii\x00\x80"; // ab 7. Element: x00, x80 => -128.0 (Ersatzwert)
	unsigned char cval17[10] = "\x03\xfe\x05\x03iii\x01\x80"; // ab 7. Element: x01, x80 => -127.996
	unsigned char cval18[10] = "\x03\xfe\x05\x03iii\xff\x7f"; // ab 7. Element: xff, x7f => 127.996

	item.set_val(cval10);
    val = item.get_val(); // h = 105d, i = 104d => 104.41015625f
    BOOST_CHECK_EQUAL(104.41015625f, val);

	item.set_val(cval11);
    val = item.get_val(); // i = 104d, h = 105d => 105.40625f
    BOOST_CHECK_EQUAL(105.40625f, val);

	item.set_val(cval12);
    val = item.get_val(); // x00, x00 => 0.0
    BOOST_CHECK_EQUAL(.0f, val);

	item.set_val(cval13);
    val = item.get_val(); // x01, x00 => 0.00390625 (1/256)
    BOOST_CHECK_EQUAL(0.00390625f, val);

	item.set_val(cval14);
    val = item.get_val(); // xff, xff => -0.00390625 (-1/256)
    BOOST_CHECK_EQUAL(-0.00390625f, val);

	item.set_val(cval15);
    val = item.get_val(); // x00, xff => -1.0
    BOOST_CHECK_EQUAL(-1.0f, val);

	item.set_val(cval16);
    val = item.get_val(); // x00, x80 => -128.0 (Ersatzwert)
    BOOST_CHECK_EQUAL(-128.0f, val);

	item.set_val(cval17);
    val = item.get_val(); // x01, x80 => -127.996
    BOOST_REQUIRE_CLOSE(-127.996f, val, 0.0001);

	item.set_val(cval18);
    val = item.get_val(); //  xff, x7f => 127.996
    BOOST_REQUIRE_CLOSE(127.996f, val, 0.0001);
}

BOOST_AUTO_TEST_CASE(Datatype_DATA2C)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03xxx\x00\x00"; // x00, x00 => 0.0
	unsigned char cval1[10] = "\x03\xfe\x05\x03xxx\x00\x01"; // x00, x01 => 0.0625 (1/16)
	unsigned char cval2[10] = "\x03\xfe\x05\x03xxx\xff\xff"; // xff, xff => -0.0625 (-1/16)
	unsigned char cval3[10] = "\x03\xfe\x05\x03xxx\xff\xf0"; // xff, xf0 => -1.0
	unsigned char cval4[10] = "\x03\xfe\x05\x03xxx\x80\x00"; // x80, x00 => -2048.0 (Ersatzwert)
	unsigned char cval5[10] = "\x03\xfe\x05\x03xxx\x80\x01"; // x80, x01 => -2047.9
	unsigned char cval6[10] = "\x03\xfe\x05\x03xxx\x7f\xff"; // x7f, xff => 2047.9
	
	float val      = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA2C;         //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);


    // ************************************************************************
    // NOT_RELEVANT endian cases
    // ************************************************************************
	item.set_val(cval0);
    val = item.get_val(); // x00, x00 => 0.0
    BOOST_CHECK_EQUAL(-1.0f, val);

    // ************************************************************************
    // LITTLE_ENDIAN cases
    // ************************************************************************
    par.en_bo = C_item::LE;   //Byte order
	item.set_par(par);

	item.set_val(cval0);
    val = item.get_val(); // x00, x00 => 0.0
    BOOST_CHECK_EQUAL(.0f, val);

	item.set_val(cval1);
    val = item.get_val(); // x00, x01 => 0.0625 (1/16)
    BOOST_CHECK_EQUAL(0.0625f, val);

	item.set_val(cval2);
    val = item.get_val(); // xff, xff => -0.0625 (-1/16)
    BOOST_CHECK_EQUAL(-0.0625f, val);

	item.set_val(cval3);
    val = item.get_val(); // xff, xf0 => -1.0
    BOOST_CHECK_EQUAL(-1.0f, val);

	item.set_val(cval4);
    val = item.get_val(); // x80, x00 => -2048.0 (Ersatzwert)
    BOOST_CHECK_EQUAL(-2048.0f, val);

	item.set_val(cval5);
    val = item.get_val(); // x80, x01 => -2047.9375
    BOOST_CHECK_EQUAL(-2047.9375f, val);

	item.set_val(cval6);
    val = item.get_val(); // x7f, xff => 2047.9375
    BOOST_CHECK_EQUAL(2047.9375f, val);
    

    // ************************************************************************
    // BIG_ENDIAN cases
    // ************************************************************************
    par.en_bo = C_item::BE;   //Byte order
	item.set_par(par);
    
	unsigned char cval10[10] = "\x03\xfe\x05\x03xxx\x00\x00"; // x00, x00 => 0.0
	unsigned char cval11[10] = "\x03\xfe\x05\x03xxx\x01\x00"; // x01, x00 => 0.0625 (1/16)
	unsigned char cval12[10] = "\x03\xfe\x05\x03xxx\xff\xff"; // xff, xff => -0.0625 (-1/16)
	unsigned char cval13[10] = "\x03\xfe\x05\x03xxx\xf0\xff"; // xf0, xff => -1.0
	unsigned char cval14[10] = "\x03\xfe\x05\x03xxx\x00\x80"; // x00, x80 => -2048.0 (Ersatzwert)
	unsigned char cval15[10] = "\x03\xfe\x05\x03xxx\x01\x80"; // x01, x80 => -2047.9
	unsigned char cval16[10] = "\x03\xfe\x05\x03xxx\xff\x7f"; // xff, x7f => 2047.9

	item.set_val(cval10);
    val = item.get_val(); // x00, x00 => 0.0
    BOOST_CHECK_EQUAL(.0f, val);

	item.set_val(cval11);
    val = item.get_val(); // x01, x00 => 0.0625 (1/16)
    BOOST_CHECK_EQUAL(0.0625f, val);

	item.set_val(cval12);
    val = item.get_val(); // xff, xff => -0.0625 (-1/16)
    BOOST_CHECK_EQUAL(-0.0625f, val);

	item.set_val(cval13);
    val = item.get_val(); // xf0, xff => -1.0
    BOOST_CHECK_EQUAL(-1.0f, val);

	item.set_val(cval14);
    val = item.get_val(); // x00, x80 => -2048.0 (Ersatzwert)
    BOOST_CHECK_EQUAL(-2048.0f, val);

	item.set_val(cval15);
    val = item.get_val(); // x01, x80 => -2047.9375
    BOOST_CHECK_EQUAL(-2047.9375f, val);

	item.set_val(cval16);
    val = item.get_val(); // xff, x7f => 2047.9375
    BOOST_CHECK_EQUAL(2047.9375f, val);
}



BOOST_AUTO_TEST_CASE(Datatype_not_set)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03xxx\x00\x00"; // x00, x00 => 0.0
	
	float val      = .0f;
	
	C_item item;
	C_item::param par;

    par.name        = "Zustand LDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::UNKNOWN;        //Data type
    par.en_bo       = C_item::NOT_RELEVANT;   //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ldw"; //table of db
	
	item.set_par(par);

	item.set_val(cval0);
    val = item.get_val();
    BOOST_CHECK_EQUAL(.0f, val);
}