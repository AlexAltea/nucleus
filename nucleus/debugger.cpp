/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "debugger.h"

#include <cstring>
#include <iostream>

// Global debugger object
Debugger debugger;

// Mongoose event handler
int ev_handler(struct mg_connection *conn, enum mg_event ev)
{
    switch (ev) {
    case MG_AUTH:
        return MG_TRUE;

    case MG_REQUEST:
        if (!strcmp(conn->uri, "/connect")) {
        }
        return MG_TRUE;

    default:
        return MG_FALSE;
    }
}

bool Debugger::init()
{
    m_server = mg_create_server(nullptr, ev_handler);
    if (!m_server) {
        std::cerr << "Could not start debugger" << std::endl;
        return false;
    }

    mg_set_option(m_server, "listening_port", "8000");
    m_listener = new std::thread([&](){
        listen();
    });

    return true;
}

void Debugger::listen()
{
    while (true) {
        mg_poll_server(m_server, 1000);
    }
}


bool Debugger::exit()
{
    if (m_server) {
        mg_destroy_server(&m_server);
        return true;
    }
    return false;
}
