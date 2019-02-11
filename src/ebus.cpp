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
#include <ebusd/ebus.h>
#include <ebusd/serial.h>
#include <ebusd/msg.h>
/////
#include <signal.h>
#include <sys/types.h>


#include <mysql/mysql.h>

//#include <iostream>
//#include <cstring>
//#include <cstdlib>
//#include <fstream>
//#include <time.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

//Logging system
std::shared_ptr<spdlog::logger> filelog;
std::shared_ptr<spdlog::logger> console;

//Serial port
C_serial2_com *rs232;

//Database
MYSQL        *mysql1;


int main(int argc, char* argv[])
{

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
            
    //Init logging system
    init_logger(spdlog::level::info, "ebusd.log");

    //Install signal handler for SIGINT signals
	signal(SIGINT, signal_handler);

    //open rs232 port for listening
    fd = rs232_open(&ds);
    if (fd < 0)
	{
        console->error("Daemon terminated!");
        exit (-1);
	}

    //connect to database
	mysql_connect(&ds);

    //Object to analyze the ebus messages
	C_ebus_message msg(filelog);

	//register items for analyzing
	register_items(&msg, &ds, mysql1);

	/* loop while waiting for input. normally we would do something
	 * useful here. */
    filelog->info() << "Start main loop!";

	while(true)
	{

        #ifdef _WIN32
            Sleep(10);
        #else
            usleep(10000);  /* sleep for 10 milliSeconds */
        #endif

		//res = read(fd, buf, 255);
        res = rs232->read(buf, 255);
        
		switch (res)
		{
			case -1:
				//Error by read()
				break;
			case 0:
				//Nothing received
				break;
			case 1:
				//One byte received
				check_bus_state(&msg, buf, res);
				break;
			default:
				//More than one byte received
				check_bus_state(&msg, buf, res);
				break;
		}
	}

    return 0;
}


void init_logger(const spdlog::level::level_enum level, const std::string& logfile)
{
    try
    {
        //Create console, multithreaded logger
        auto console = spdlog::stdout_logger_mt("console");
        console->set_level(spdlog::level::info);
        
        //Create async file logger
        size_t q_size = 1048576; //queue size must be power of 2
        spdlog::set_async_mode(q_size);
        
        auto file_logger = spdlog::rotating_logger_mt("ebusd", logfile, 1048576 * 5, 3);
        file_logger->set_level(level);
        
		console = spdlog::get("console");
		filelog = spdlog::get("ebus");
	
        console->info() << "Logging initialized";
        filelog->info() << "Logging initialized";
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
        exit (-1);
    }
}


void close_logger(void)
{
    spdlog::drop_all();
}


void signal_handler(int signum)
{
	switch (signum)
	{
		case SIGINT:
            console->info() << "Signal SIGINT received {}" << signum;
			break;

		default:
            console->info() << "Signal received {}" << signum;
			break;
	}

	/* restore old settings and close port */
    rs232_close();

    /* disconnect from database */
	mysql_disconnect();
    
    /* close logging system */
    close_logger();

	exit(signum);
}



int rs232_open(deamon_settings *ds)
{
	int port = ds->ser_port;
    int ret_val;

    //Initialize C_serial_com object for connection
    rs232 = new C_serial2_com(filelog);
    
    rs232->set_port(port);

    ret_val = rs232->com_open();
    if (ret_val > 0)
    {
        delete rs232;
        return 0;
    }

    return 1;
}



void rs232_close(void)
{	
    delete rs232;
    
	filelog->info("Serial port closed.");
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
		filelog->error("{}", mysql_error(mysql1));
		filelog->error("    server: {}",  db_server);
		filelog->error("    db name: {}", db_name);
		filelog->error("    db user: {}", db_user_name);
		return;
	}

	//Connect to the database
	if (mysql_real_connect(mysql1, db_server.c_str(), db_user_name.c_str(), db_user_password.c_str(), db_name.c_str(), 0, NULL, 0) == NULL)
	{
		filelog->error("{}", mysql_error(mysql1));
		filelog->error("    server: {}",  db_server);
		filelog->error("    db name: {}", db_name);
		filelog->error("    db user: {}", db_user_name);
		return;
	}
	else
	{
		filelog->info("Database connection successful.");
	}

}



void mysql_disconnect(void)
{
	mysql_close(mysql1);
	filelog->info("Disconnected from database.");
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
					msg->print();
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
    
    
    ser_port         = pt.get<int>("ebusd.serial.port");
	
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
    
    ds->ser_port         = 22; //ttyAMA0
    
    
    ds->db_server        = "sammelplatz";
    ds->db_name          = "wolf";
    ds->db_user_name     = "wolf";
    ds->db_user_password = "Spacemanwolf";
    

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
