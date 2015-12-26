/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "debugger.h"
#include "externals/rapidjson/document.h"
#include "externals/rapidjson/prettywriter.h"
#include "externals/rapidjson/stringbuffer.h"

#include <cstring>
#include <iostream>

// Nerve parameters
#define NERVE_VERSION "1.0.0"

using namespace rapidjson;

// Global debugger object
Debugger debugger;

// Serializers
//void dbg_connect(mg_connection *conn);
//void dbg_cpu_threads(mg_connection *conn);

// Mongoose event handler
/*int ev_handler(mg_connection *conn, mg_event ev)
{
    switch (ev) {
    case MG_AUTH:
        return MG_TRUE;

    case MG_REQUEST:
        if (!strcmp(conn->uri, "/connect")) {
            dbg_connect(conn);
        } else if (!strcmp(conn->uri, "/cpu/threads")) {
            dbg_cpu_threads(conn);
        }
        return MG_TRUE;

    default:
        return MG_FALSE;
    }
}*/

/**
 * Debugger serializers
 */
/*void dbg_connect(mg_connection *conn)
{
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    Document doc;

    Value device(kObjectType);
    device.AddMember("name", Value("Nucleus"), doc.GetAllocator());
    device.AddMember("type", Value("emulator"), doc.GetAllocator());
    device.AddMember("version", Value("0.0.1"), doc.GetAllocator());

    Value host(kObjectType);
    host.AddMember("os", Value("Unknown"), doc.GetAllocator());
    host.AddMember("cpu", Value("x86"), doc.GetAllocator());

    doc.SetObject();
    doc.AddMember("version", Value("1.0.0"), doc.GetAllocator());
    doc.AddMember("device", device, doc.GetAllocator());
    doc.AddMember("host", host, doc.GetAllocator());
    doc.Accept(writer);

    mg_send_header(conn, "Access-Control-Allow-Origin", "*");
    mg_printf_data(conn, buffer.GetString());
}

void dbg_cpu_threads(mg_connection *conn)
{
}*/

/**
 * Debugger methods
 */
bool Debugger::init()
{
    /*m_server = mg_create_server(nullptr, ev_handler);
    if (!m_server) {
        std::cerr << "Could not start debugger" << std::endl;
        return false;
    }

    mg_set_option(m_server, "listening_port", "8000");
    m_listener = new std::thread([&](){
        listen();
    });*/

    return true;
}

void Debugger::listen()
{
    /*while (true) {
        mg_poll_server(m_server, 1000);
    }*/
}

bool Debugger::exit()
{
    /*if (m_server) {
        mg_destroy_server(&m_server);
        return true;
    }*/
    return false;
}
