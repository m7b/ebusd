//===-- ebus.cpp - Entry point for the console application ------*- C++ -*-===//
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
#include "ebus.h"
//#include "serial.h"

#include <rs232.h>
#include "msg.h"
/////
#include <signal.h>
#include <sys/types.h>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


//Database
MYSQL        *mysql1;


int main(int argc, char* argv[])
{
    int comport_number = 22;
	int fd, res, retval;
	unsigned char buf[255];
	deamon_settings ds;
	
	switch (argc)
	{
        case 1:
            printf("Usage: ebusd settings.xml\n");
            save_example(&ds);
            exit (-1);
        case 2:
			retval = ds.load(argv[1]);
            if (retval < 0)
                exit (-1);
            break;
    }

    //Install signal handler for SIGINT signals
    signal(SIGINT, signal_handler);

    //open rs232 port for listening
    fd = rs232_open(&ds);
    (fd > 0)
	{
        exit (-1);
	}

    //connect to database
	mysql_connect(&ds);

    //Object to analyze the ebus messages
	C_ebus_message msg;

	//register items for analyzing
	register_items(&msg, &ds, mysql1);

	/* loop while waiting for input. normally we would do something
	 * useful here. */
    printf("Start main loop!");

	while(true)
	{
		res = RS232_PollComport(comport_number, buf, 255);
		if (res > 0)
		{
			check_bus_state(&msg, buf, res);
		}
		
		/* sleep for 100 milliSeconds */
        #ifdef _WIN32
            Sleep(100);
        #else
            usleep(100000);
        #endif
	}

    return 0;
}


void signal_handler(int signum)
{
	switch (signum)
	{
		case SIGINT:
                        printf("Signal SIGINT received %d\n", signum);
			break;

		default:
        		printf("Signal received %d\n", signum);
			break;
	}

	/* restore old settings and close port */
    rs232_close();

    /* disconnect from database */
	mysql_disconnect();

	exit(signum);
}



int rs232_open(deamon_settings *ds)
{
    int fd;
	
	std::string port = ds->ser_port;
	int comport_number = 22;
	int baudrate = 2400;
	char mode[]={'8','N','1',0};
	
	fd = RS232_OpenComport(comport_number, baudrate, mode);

    return fd;
}



void rs232_close(void)
{	
	int comport_number = 22;
    RS232_CloseComport(comport_number);
    printf("Serial port closed.\n");
}



void mysql_connect(deamon_settings *ds)
{
	
	std::string db_server        = ds->db_server;
	std::string db_name          = ds->db_name;
	std::string db_user_name     = ds->db_user_name;
	std::string db_user_password = ds->db_user_password;

	//Initialize MYSQL object for connections
	mysql1 = mysql_init(NULL);

	if (mysql1 == NULL)
	{
		printf("%s\n", mysql_error(mysql1));
		printf("    server: %s\n",  db_server.c_str());
		printf("    db name: %s\n", db_name.c_str());
		printf("    db user: %s\n", db_user_name.c_str());
		return;
	}

	//Connect to the database
	if (mysql_real_connect(mysql1, db_server.c_str(), db_user_name.c_str(), db_user_password.c_str(), db_name.c_str(), 0, NULL, 0) == NULL)
	{
		printf("%s\n", mysql_error(mysql1));
		printf("    server: %s\n",  db_server.c_str());
		printf("    db name: %s\n", db_name.c_str());
		printf("    db user: %s\n", db_user_name.c_str());
		return;
	}
	else
	{
		printf("Database connection successful.\n");
	}

}



void mysql_disconnect(void)
{
	mysql_close(mysql1);
	printf("Disconnected from database.\n");
}


void register_items(C_ebus_message *msg, deamon_settings *ds, MYSQL *db)
{
	char create_table[1024];
	
    BOOST_FOREACH(const C_item::param &it, ds->other_item)
    {
        msg->register_item(it);
        
        switch (it.en_dt)
        {
        	case C_item::BIT:
        
                sprintf(create_table, "CREATE TABLE IF NOT EXISTS `%s` ( \
                    `lfd` BIGINT(20) UNSIGNED NOT NULL AUTO_INCREMENT, \
                    `date` DATETIME NOT NULL, \
                    `val` SMALLINT(1) UNSIGNED NOT NULL, \
                    PRIMARY KEY (`lfd`) \
                    ) \
                    COLLATE='utf8_general_ci' \
                    ENGINE=InnoDB \
                    ROW_FORMAT=COMPACT;", it.s_db_table.c_str());
                break;
						
			default:
        
                sprintf(create_table, "CREATE TABLE IF NOT EXISTS `%s` ( \
                    `lfd` BIGINT(20) UNSIGNED NOT NULL AUTO_INCREMENT, \
                    `date` DATETIME NOT NULL, \
                    `val` FLOAT NOT NULL, \
                    PRIMARY KEY (`lfd`) \
                    ) \
                    COLLATE='utf8_general_ci' \
                    ENGINE=InnoDB \
                    ROW_FORMAT=COMPACT;", it.s_db_table.c_str());
                break;
							
		}
		mysql_query(db, create_table);
    }   
}



void check_bus_state(C_ebus_message *msg, unsigned char *buf, int length)
{
	int i;

	for(i=0; i<length; i++)
	{
		switch (buf[i])
		{
            // bus is in idle state [0xaa]
			case 0xaa:

                // a message has been built
				if (msg->get_len() > 0)
				{
					msg->undo_subst();
				//	msg->print();
					msg->analyse();
					msg->write_db(mysql1);
					msg->clear();
				}
				break;

            // there is a message data [!= 0xaa]
			default:
				msg->add(buf[i]);
				break;
		}
	}
}



int deamon_settings::load(const std::string &filename)
{
	using boost::property_tree::ptree;
	ptree pt;
    
    C_item::param item;
	
    try
    {
        read_xml(filename, pt);
    }
    catch(boost::exception const& ex)
    {
        printf("Can't init settings.\n");
        return -1;
    }
    
    
    ser_port         = pt.get<std::string>("ebusd.serial.port");
	
	db_server        = pt.get<std::string>("ebusd.database.server");
	db_name          = pt.get<std::string>("ebusd.database.name");
	db_user_name     = pt.get<std::string>("ebusd.database.user_name");
	db_user_password = pt.get<std::string>("ebusd.database.user_password");
	
	BOOST_FOREACH(ptree::value_type &v, pt.get_child("ebusd.elements"))
	{
        item.name       = v.second.get<std::string>("name");
        item.unit       = v.second.get<std::string>("unit");
        item.uc_QQ      = v.second.get<unsigned char>("QQ");
        item.uc_ZZ      = v.second.get<unsigned char>("ZZ");
        item.uc_PB      = v.second.get<unsigned char>("PB");
        item.uc_SB      = v.second.get<unsigned char>("SB");
        item.ui_pos     = v.second.get<unsigned int>("pos");
        item.ui_bit_pos = v.second.get<unsigned int>("bit_pos");
        item.en_dt      = (C_item::en_data_type) v.second.get<unsigned int>("data_type");
        item.en_bo      = (C_item::en_byte_order) v.second.get<unsigned int>("byte_order");
        item.f_pos_tol  = v.second.get<float>("pos_tol");
        item.f_neg_tol  = v.second.get<float>("neg_tol");
        item.s_db_table = v.second.get<std::string>("db_table");
        
        other_item.push_back(item);
	}

    return 0;
}

void deamon_settings::save(const std::string &filename)
{	
	using boost::property_tree::ptree;
	ptree pt;
	ptree element;
    
	pt.put("ebusd.serial.port", ser_port);
    
	pt.put("ebusd.database.server",        db_server);
	pt.put("ebusd.database.name",          db_name);
	pt.put("ebusd.database.user_name",     db_user_name);
	pt.put("ebusd.database.user_password", db_user_password);
    
    BOOST_FOREACH(const C_item::param &it, other_item)
    {
        element.put("name",       it.name);
        element.put("unit",       it.unit);
        element.put("QQ",         it.uc_QQ);
        element.put("ZZ",         it.uc_ZZ);
        element.put("PB",         it.uc_PB);
        element.put("SB",         it.uc_SB);
        element.put("pos",        it.ui_pos);
        element.put("bit_pos",    it.ui_bit_pos);
        element.put("data_type",  it.en_dt);
        element.put("byte_order", it.en_bo);
        element.put("pos_tol",    it.f_pos_tol);
        element.put("neg_tol",    it.f_neg_tol);
        element.put("db_table",   it.s_db_table);
        
        pt.add_child("ebusd.elements.element", element);
    }

	write_xml(filename, pt);
}


void save_example(deamon_settings *ds)
{
	C_item::param     par;
    
    ds->ser_port         = "ttyAMA0";
    
    ds->db_server        = "db-server";
    ds->db_name          = "db-name";
    ds->db_user_name     = "db-username";
    ds->db_user_password = "db-username-password";
    
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
    
    ds->other_item.push_back(par);

    par.name        = "Zustand GDW";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 1;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_gdw"; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Zustand WS";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 2;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ws "; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Zustand Flamme";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 3;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_flamme"; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Zustand Ventil 1";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 4;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ventil_1"; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Zustand Ventil 2";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 5;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_ventil_2"; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Zustand UWP";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 6;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_uwp"; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Zustand Alarm";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 7;             //Bit Position
    par.en_dt       = C_item::BIT     ;     //Data type
    par.en_bo       = C_item::NOT_RELEVANT; //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0503_zustand_alarm"; //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Kessel Temperatur";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 9;             //Position (M10)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA1C;        //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.5;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.5;           //negative tolerance for entry new value
    par.s_db_table  = "0503_kessel_temp";   //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Ruecklauftemperatur";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::SIGNED_CHAR;   //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.5;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.5;           //negative tolerance for entry new value
    par.s_db_table  = "0503_ruecklauf_temp";   //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Boiler Temperatur";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 11;            //Position (M12)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::SIGNED_CHAR;   //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.5;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.5;           //negative tolerance for entry new value
    par.s_db_table  = "0503_boiler_temp";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Aussentemperatur";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x03;          //Source filter
    par.uc_ZZ       = 0xfe;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x03;          //Secondary Order filter
    par.ui_pos      = 12;            //Position (M13)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::SIGNED_CHAR;   //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.5;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.5;           //negative tolerance for entry new value
    par.s_db_table  = "0503_aussen_temp";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Statuswaermeanforderung";
    par.unit        = "Wert";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x07;          //Secondary Order filter
    par.ui_pos      = 5;             //Position (M6)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::UNSIGNED_CHAR;   //Data type
    par.en_bo       = C_item::NOT_RELEVANT;    //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0507_statuswaermeanforderung";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Kesselsollwert-Temperatur";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x05;          //Primary Order filter
    par.uc_SB       = 0x07;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA2C; //Data type
    par.en_bo       = C_item::LE;     //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0507_kesselsollwert_temp";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Kessel Sollwert";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 5;             //Position (M6)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA2B;   //Data type
    par.en_bo       = C_item::LE;       //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_kessel_sollwert";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Aussentemperatur Modul";
    par.unit        = "Grad Celsius";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 7;             //Position (M8)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::DATA2B;   //Data type
    par.en_bo       = C_item::LE;       //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_aussentemp_modul";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status BWR";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 0;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bwr";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Heizkreis";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 1;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_heizkreis";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Bit 2";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 2;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bit2";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Bit 3";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 3;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bit3";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Bit 4";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 4;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bit4";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Bit 5";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 5;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bit5";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Bit 6";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 6;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bit6";  //table of db
    
    ds->other_item.push_back(par);

    par.name        = "Status Bit 7";
    par.unit        = "an/aus";
    par.uc_QQ       = 0x10;          //Source filter
    par.uc_ZZ       = 0x03;          //Destination filter
    par.uc_PB       = 0x08;          //Primary Order filter
    par.uc_SB       = 0x00;          //Secondary Order filter
    par.ui_pos      = 10;            //Position (M11)
    par.ui_bit_pos  = 7;             //Bit Position
    par.en_dt       = C_item::BIT;           //Data type
    par.en_bo       = C_item::NOT_RELEVANT;  //Byte order
    par.f_pos_tol   = 0.0;           //positive tolerance for entry new value
    par.f_neg_tol   = 0.0;           //negative tolerance for entry new value
    par.s_db_table  = "0800_status_bit7";  //table of db
    
    ds->other_item.push_back(par);
    
    ds->save("settings_example.xml");
}