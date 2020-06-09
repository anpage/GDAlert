#ifndef GDNATIVEALERT_H
#define GDNATIVEALERT_H

#include <Godot.hpp>
#include <PoolArrays.hpp>
#include <Node.hpp>
#include <String.hpp>

#include "CNCDll.h"

#define GAME_BUFFER_SIZE 0xFFFFFFF // 255 MB, yay overkill!

namespace godot {

    class GDNativeAlert : public Node {
        GODOT_CLASS(GDNativeAlert, Node);

    private:
        static GDNativeAlert* callback_instance;
        static void event_callback(const EventCallbackStruct& event);
        void handle_event(const EventCallbackStruct& event);

        PoolByteArray game_buffer_pba;
        unsigned char game_buffer[GAME_BUFFER_SIZE];
        unsigned int game_buffer_width;
        unsigned int game_buffer_height;

    public:
        static void _register_methods();

        GDNativeAlert();
        ~GDNativeAlert();

        void _init();

        bool cnc_start_instance(int scenario_index, int build_level, String faction);
        bool cnc_advance_instance();
        PoolByteArray cnc_get_visible_page();
        unsigned int cnc_get_visible_page_width();
        unsigned int cnc_get_visible_page_height();
        void cnc_handle_left_mouse_up(unsigned int x, unsigned int y);
        void cnc_handle_right_mouse_up(unsigned int x, unsigned int y);
        void cnc_handle_right_mouse_down(unsigned int x, unsigned int y);
        void cnc_handle_mouse_area(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
    };

}

#endif
