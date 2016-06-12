/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window-xcb.h"

// Nucleus
#include "nucleus/nucleus.h"

#include <xcb/xcb.h>

#include <cstdio>
#include <cstring>
#include <thread>

void createWindowXcb(int argc, char **argv) {
    // XCB connection
    int scr;
    xcb_connection_t* connection = xcb_connect(NULL, &scr);
    if (connection == NULL) {
        printf("Cannot open a XCB connection.\nExiting...\n");
        exit(1);
    }

    const xcb_setup_t *setup = xcb_get_setup(connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) {
        xcb_screen_next(&iter);
    }
    xcb_screen_t* screen = iter.data;

    // XCB window properties
    const char* title = "Nucleus";
    unsigned int width = 960;
    unsigned int height = 544;
    uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t value_list[32];
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE |
                    XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    xcb_window_t window = xcb_generate_id(connection);
    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0,
        width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title), title);

    // Notification when window is destroyed
    xcb_intern_atom_cookie_t cookie1 = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie1, 0);
    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t* atom_wm_delete_window = xcb_intern_atom_reply(connection, cookie2, 0);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
        (*reply).atom, 4, 32, 1, &(*atom_wm_delete_window).atom);
    free(reply);

    const uint32_t coords[] = {100, 100};
    xcb_map_window(connection, window);
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    // Events
    bool quit = false;
    while (!quit) {
        xcb_generic_event_t* event;
        event = xcb_poll_for_event(connection);
        if (!event) {
            continue;
        }

        uint8_t code = event->response_type & 0x7f;
        switch (code) {
        case XCB_EXPOSE:
            // TODO: Resize window
            break;
        case XCB_CLIENT_MESSAGE: {
            const auto& msg = *(const xcb_client_message_event_t*)event;
            if (msg.data.data32[0] == atom_wm_delete_window->atom) {
                quit = true;
            }
            break;
        }
        case XCB_KEY_RELEASE: {
            const auto& key = *(const xcb_key_release_event_t*)event;
            // TODO: Handle event
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            const auto& cfg = *(const xcb_configure_notify_event_t*)event;
            // TODO: Handle event
            break;
        }
        default:
            break;
        }

        free(event);
    }

    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);
}
