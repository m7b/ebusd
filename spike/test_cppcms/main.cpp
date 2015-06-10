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

#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>

//start the app using: ./mytest -c config.js


class my_hello_world : public cppcms::application {
public:
    my_hello_world(cppcms::service &srv) :
        cppcms::application(srv) 
    {
    }
    virtual void main(std::string url);
};


void my_hello_world::main(std::string /*url*/)
{
    response().out()<<
        "<html>\n"
        "<body>\n"
        "  <h1>Hello Wolf</h1>\n"
        "</body>\n"
        "</html>\n";
}

int main(int argc, char* argv[])
{
	try {
        cppcms::service srv(argc,argv);
        srv.applications_pool().mount(cppcms::applications_factory<my_hello_world>());
        srv.run();
    }
    catch(std::exception const &e) {
        std::cerr<<e.what()<<std::endl;
    }
}
