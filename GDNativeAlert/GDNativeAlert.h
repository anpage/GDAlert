#ifndef GDNATIVEALERT_H
#define GDNATIVEALERT_H

#include <Godot.hpp>
#include <PoolArrays.hpp>
#include <Node.hpp>
#include <String.hpp>
#include <AudioStreamSample.hpp>

#include "CNCDll.h"

namespace godot {

    class GDNativeAlert : public Node {
        GODOT_CLASS(GDNativeAlert, Node);

    private:
        static GDNativeAlert* callback_instance;
        static void event_callback(const EventCallbackStruct& event);
        void handle_event(const EventCallbackStruct& event);

        static AudioStreamSample* decode_aud(String name);

        static int distance(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);

        PoolByteArray game_buffer_pba;
        PoolByteArray game_palette_pba;
        PoolByteArray speech_buffer;
        unsigned int game_buffer_width;
        unsigned int game_buffer_height;

    public:
        static void _register_methods();

        GDNativeAlert();
        ~GDNativeAlert();

        void _init();

        AudioStreamSample* get_score_sample(String name);
        void play_sound(String name, bool isSpeech, int x, int y);

        bool start_instance(int scenario_index, int build_level, String faction);
        bool advance_instance();
        PoolByteArray get_palette();
        PoolByteArray get_visible_page();
        unsigned int get_visible_page_width();
        unsigned int get_visible_page_height();
        void handle_left_mouse_up(unsigned int x, unsigned int y);
        void handle_right_mouse_up(unsigned int x, unsigned int y);
        void handle_right_mouse_down(unsigned int x, unsigned int y);
        void handle_mouse_area(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
        void handle_mouse_motion(unsigned int x, unsigned int y);
        String get_cursor_name(real_t x, real_t y);
        Array get_game_objects();
        CNCObjectStruct* get_nearest_object(CNCObjectListStruct* layers, real_t x, real_t y);
    };

}

#endif
