//===-- main.h - Header definitions for the console application -*- C++ -*-===//
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


#include <spdlog/spdlog.h>
#include <string>

class A
{
    private:
    
        std::shared_ptr<spdlog::logger> log;
        
    public:
    
        A(const std::shared_ptr<spdlog::logger> log);
        ~A();
};

A::A(const std::shared_ptr<spdlog::logger> log)
{
    this->log = log;
    log->info("A successfully constructed!");
}

A::~A()
{
    log->info("A successfully destructed!");
}




class B
{
    private:
    
        std::shared_ptr<spdlog::logger> log;
        
    public:
    
        B(const std::shared_ptr<spdlog::logger> log);
        ~B();
};

B::B(const std::shared_ptr<spdlog::logger> log)
{
    this->log = log;
    log->info("B successfully constructed!");
    
    log->trace("TRACE!");
    log->debug("DEBUG!");
    log->info("INFO!");
    log->notice("NOTICE!");
    log->warn("WARN!");
    log->error("ERR!");
    log->critical("CRITICAL!");
    log->alert("ALERT!");
    log->emerg("EMERG!");
 // log->off("OFF!");

}

B::~B()
{
    log->info("B successfully destructed!");
}
