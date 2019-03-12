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

BOOST_AUTO_TEST_CASE(Datatype_BIT)
{
	unsigned char cval0[10] = "\x03\xfe\x05\x03iiihi"; // 7. Element = h = 104d = 0
	unsigned char cval1[10] = "\x03\xfe\x05\x03hhhih"; // 7. Element = i = 105d = 1
	unsigned char cval2[10] = "\x03\xfe\x05\x03iiiBi"; // 7. Element = B = 102d = 0
	
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
}
