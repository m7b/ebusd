//===-- item.h - Header for the console application -------------*- C++ -*-===//
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
#ifndef _item_h_
#define _item_h_

#include <stdio.h>
#include <string>

using namespace std;

class C_item
{
public:

    //Enumaration for byte order
    enum en_byte_order
    {
        NOT_RELEVANT,   //Not relevant
        LE,             //Little endian
        BE              //Big endian
    };

    //Enumeration for data type
    enum en_data_type
    {
        BIT,            //Value is to interpret as BIT
        UNSIGNED_CHAR,  //Value is to interpret as UNSIGNED_CHAR
        SIGNED_CHAR,    //Value is to interpret as SIGNED_CHAR
        BCD,            //Value is to interpret as BCD
        DATA1B,         //Value is to interpret as DATA1B
        DATA1C,         //Value is to interpret as DATA1C
        DATA2B,         //Value is to interpret as DATA2B
        DATA2C          //Value is to interpret as DATA2C
    };

    //Struct for item parameter
    struct param
    {
        string name;                //name of item
        string unit;                //unit of item
        unsigned char uc_QQ;        //Filter Source
        unsigned char uc_ZZ;        //Filter Destination
        unsigned char uc_PB;        //Filter Primary Order
        unsigned char uc_SB;        //Filter Secondary Order
        unsigned int  ui_pos;       //Position
        unsigned int  ui_bit_pos;   //Bit Position
        en_data_type  en_dt;        //Data type
        en_byte_order en_bo;        //Byte order
        float f_pos_tol;            //positive tolerance for entry new value
        float f_neg_tol;            //negative tolerance for entry new value
        string s_db_table;          //table of db
    };

private:

    param     par;              //Parameter of the item

    float f_act_val;            //Actual value
    float f_last_val;           //Last value
    bool  b_new;                //Flag for act value has changed

    bool check_limit(float);    //check new value against limits, returns true
                                //by exceeding

    bool is_filtered(const unsigned char *);  //check message is filtered

    bool  byte_to_bool(const unsigned char *, unsigned int);
    unsigned char  byte_to_bcd(const unsigned char *);
    char  byte_to_DATA1b(const unsigned char *);
    float byte_to_DATA1c(const unsigned char *);
    float byte_to_DATA2b(const unsigned char *, en_byte_order);
    float byte_to_DATA2c(const unsigned char *, en_byte_order);

public:

    C_item();                              //Konsturktor
    ~C_item();                             //Destruktor
    C_item(const C_item &);                //Kopierkonstruktor
    C_item& operator=(const C_item& rhs);  //Assignment operator

    void set_par(const param &p);
    void set_val(float);
    void set_val(const unsigned char *);

    float get_val(void);
    float get_last_val(void);

    bool check_new_val(void);
    void reset_new_val(void);

    string get_name(void);
    string get_db_table(void);

};

#endif //_item_h_