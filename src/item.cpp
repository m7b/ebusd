//===-- item.cpp - Header for the console application -----------*- C++ -*-===//
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
#include "item.h"


C_item::C_item()
: f_act_val(0.0), f_last_val(0.0), b_new(false)
{
}


C_item::~C_item()
{
}


C_item::C_item(const C_item &m)
{
    //printf("kopierer\n");
    par = m.par;

    f_act_val  = m.f_act_val;            //Actual value
    f_last_val = m.f_last_val;           //Last value
    b_new      = m.b_new;
}


C_item& C_item::operator=(const C_item& rhs)
{
    //printf("zuweisen\n");
    par = rhs.par;

    f_act_val  = rhs.f_act_val;            //Actual value
    f_last_val = rhs.f_last_val;           //Last value
    b_new      = rhs.b_new;

    return *this;
}


float C_item::get_val(void)
{
    return f_act_val;
}


float C_item::get_last_val(void)
{
    return f_last_val;
}


void C_item::set_par(const param *p)
{
    par = p;
}


void C_item::set_val(float f_new_val)
{
    f_last_val = f_act_val;
    f_act_val = f_new_val;
    b_new = true;
}


void C_item::set_val(const unsigned char *start)
{
    unsigned int pos = par.ui_pos;
    unsigned int bit_pos = par.ui_bit_pos;
    en_byte_order en_bo = par.en_bo;

    float f_new_val;

    if (!is_filtered(start))
    {

        switch (par.en_dt)
        {
            case BIT:
                f_new_val = byte_to_bool(&start[pos], bit_pos);
                break;

            case UNSIGNED_CHAR:
                f_new_val = (float) ((unsigned char) start[pos]);
                break;

            case SIGNED_CHAR:
                f_new_val = (float) ((signed char) start[pos]);
                break;

            case BCD:
                f_new_val = byte_to_bcd(&start[pos]);
                break;

            case DATA1B:
                f_new_val = byte_to_DATA1b(&start[pos]);
                break;

            case DATA1C:
                f_new_val = byte_to_DATA1c(&start[pos]);
                break;

            case DATA2B:
                f_new_val = byte_to_DATA2b(&start[pos], en_bo);
                break;

            case DATA2C:
                f_new_val = byte_to_DATA2c(&start[pos], en_bo);
                break;

            default:
                return;

        }

        if (check_limit(f_new_val))
        {
            f_last_val = f_act_val;
            f_act_val = f_new_val;
            b_new = true;

            //TODO: enabling via parameter
            //printf("%s new %.1f, old %.1f [%s]\n", par.name.c_str(), f_act_val, f_last_val, par.unit.c_str());
        }
    }
}


bool C_item::check_limit(float f_new_val)
{
    bool b_ret_val = false;

    if ((f_new_val > (f_act_val + par.f_pos_tol)) || (f_new_val < (f_act_val - par.f_neg_tol)))
        b_ret_val = true;

    return b_ret_val;
}


bool C_item::is_filtered(const unsigned char *start)
{
    unsigned char uc_QQ = start[0];   //Source
    unsigned char uc_ZZ = start[1];   //Destination
    unsigned char uc_PB = start[2];   //Primary Order
    unsigned char uc_SB = start[3];   //Secondary Order

    bool b_condition_1 = false;
    bool b_condition_2 = false;
    bool b_condition_3 = false;
    bool b_condition_4 = false;

    bool rc = true;

    if ((par.uc_QQ == 0x00) || (uc_QQ == par.uc_QQ))
        b_condition_1 = true;

    if ((par.uc_ZZ == 0x00) || (uc_ZZ == par.uc_ZZ))
        b_condition_2 = true;

    if ((par.uc_PB == 0x00) || (uc_PB == par.uc_PB))
        b_condition_3 = true;

    if ((par.uc_SB == 0x00) || (uc_SB == par.uc_SB))
        b_condition_4 = true;

    if (b_condition_1 && b_condition_2 && b_condition_3 && b_condition_4)
        rc = false;

    return rc;
}


float C_item::byte_to_bool(const unsigned char *uc_byte, unsigned int ui_bit_pos)
{
    unsigned char uc_temp = *uc_byte;

    uc_temp >>= ui_bit_pos;
    uc_temp &= (unsigned char) 0b00000001;

    return (float)(uc_temp);
}


float C_item::byte_to_bcd(const unsigned char *uc_byte)
{
    unsigned char uc_high_nibble = *uc_byte & 0xf0;
    unsigned char uc_low_nibble  = *uc_byte & 0x0f;

    uc_high_nibble = uc_high_nibble >> 4;

    if (uc_high_nibble > 9 || uc_low_nibble > 9)
        return -1.0f;

    return (float) (uc_high_nibble * 10.0f) + uc_low_nibble;
}


float C_item::byte_to_DATA1b(const unsigned char *uc_byte)
{
    signed char ch_val = *uc_byte;
    return (float)(ch_val);
}


float C_item::byte_to_DATA1c(const unsigned char *uc_byte)
{
    return (float) *uc_byte / 2.0f;
}


float C_item::byte_to_DATA2b(const unsigned char *uc_byte, en_byte_order en_byteorder)
{
    unsigned char c_low_byte, c_high_byte;

    switch (en_byteorder)
    {
        case NOT_RELEVANT:
            return -1.0f;
        case LE:
            c_high_byte = uc_byte[0];
            c_low_byte = uc_byte[1];
            break;
        case BE:
            c_high_byte = uc_byte[1];
            c_low_byte = uc_byte[0];
            break;
    }

    if ((c_high_byte & 0x80) == 0x80)
    {
        //We have a negative number
        signed char sc_high_byte = c_high_byte;
        return sc_high_byte + (c_low_byte / 256.0f);
    }
    else
    {
        //We have a positive number
        return c_high_byte + (c_low_byte / 256.0f);
    }
}


float C_item::byte_to_DATA2c(const unsigned char *uc_byte, en_byte_order en_byteorder)
{
    unsigned char uc_high_byte, uc_low_nibble, uc_high_nibble;

    switch (en_byteorder)
    {
        case NOT_RELEVANT:
            return -1.0f;
        case LE:
            uc_high_byte   = uc_byte[0];
            uc_low_nibble  = uc_byte[1] & 0x0f;
            uc_high_nibble = uc_byte[1] & 0xf0;
            uc_high_nibble >>= 4;
            break;
        case BE:
            uc_high_byte   = uc_byte[1];
            uc_low_nibble  = uc_byte[0] & 0x0f;
            uc_high_nibble = uc_byte[0] & 0xf0;
            uc_high_nibble >>= 4;
            break;
    }

    if ((uc_high_byte & 0x80) == 0x80)
    {
        //We have a negative number
        signed char sc_high_byte = uc_high_byte;
        return (sc_high_byte * 16.0f) + uc_high_nibble + (uc_low_nibble / 16.0f);
    }
    else
    {
        //We have a positive number
        return (uc_high_byte * 16.0f) + uc_high_nibble + (uc_low_nibble / 16.0f);
    }
}


bool C_item::check_new_val(void)
{
    return b_new;
}


void C_item::reset_new_val(void)
{
    b_new = false;
}


string C_item::get_name(void)
{
    return par.name;
}


string C_item::get_db_table(void)
{
    return par.s_db_table;
}