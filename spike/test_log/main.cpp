//===-- main.cpp - Entry point for the console application ------*- C++ -*-===//
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

#define _WIN32_WINNT 0x0501
#define __USE_W32_SOCKETS

#include "main.h"
#include <spdlog/spdlog.h>
//#include "crow_all.h"

void init_logger(const spdlog::level::level_enum level, const std::string& logfile);
void close_logger(void);

int main(int argc, char* argv[])
{
    /*
    trace = 0,
    debug = 1,
    info = 2,
    notice = 3,
    warn = 4,
    err = 5,
    critical = 6,
    alert = 7,
    emerg = 8,
    off = 9
    */
    
    init_logger(spdlog::level::info, "log");
    
    spdlog::get("console")->info() << "App started";

    A TestObj_A(spdlog::get("ebusd"));
    B TestObj_B(spdlog::get("ebusd"));
    
    close_logger();

    return 0;
}




void init_logger(const spdlog::level::level_enum level, const std::string& logfile)
{
    try
    {
        //Create console, multithreaded logger
        auto console = spdlog::stdout_logger_mt("console");
        console->set_level(spdlog::level::info);
        
        size_t q_size = 1048576; //queue size must be power of 2
        spdlog::set_async_mode(q_size);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%l] %v");
        
        auto file_logger = spdlog::rotating_logger_mt("ebusd", logfile, 1048576 * 5, 3);
        file_logger->set_level(level);
        
        file_logger->info() << "This is async log.. " << "Should be very fast!";
          
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}

void close_logger(void)
{
    spdlog::drop_all();
}

