#ifndef GDNATIVEALERT_H
#define GDNATIVEALERT_H

#include <Godot.hpp>
#include <PoolArrays.hpp>
#include <ImageTexture.hpp>
#include <String.hpp>
#include <AudioStreamSample.hpp>

#include "CNCDll.h"

namespace godot {

    class GDNativeAlert : public ImageTexture {
        GODOT_CLASS(GDNativeAlert, ImageTexture);

    private:
        static GDNativeAlert* callback_instance;
        static void event_callback(const EventCallbackStruct& event);
        void handle_event(const EventCallbackStruct& event);

        static int distance(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);

        Image* game_image;
        PoolByteArray game_buffer_pba;
        PoolByteArray game_palette_pba;
        PoolByteArray speech_buffer;

    public:
        static void _register_methods();

        GDNativeAlert();
        ~GDNativeAlert();

        void _init();

        void play_sound(String name, bool isSpeech, int x, int y);
        bool start_instance(int scenario_index, int build_level, String faction);
        bool advance_instance(uint64 player_id = 0);
        PoolByteArray get_palette();
        bool get_visible_page();
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
