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
        static const int GAME_STATE_BUFFER_SIZE = 0x200000;

        static const int LEPTONS_PER_CELL = 256;
        static const int PIXELS_PER_CELL = 24;

        uint64 player_id = 0;

        CNCPlayerInfoStruct* player_state_cache;
        CNCObjectListStruct* game_state_cache;
        CNCMapDataStruct* static_map_state_cache;
        CNCDynamicMapStruct* dynamic_map_state_cache;
        CNCShroudStruct* shroud_state_cache;
        CNCSidebarStruct* sidebar_state_cache;

        unsigned char palette_cache[256][3];

        static GDNativeAlert* callback_instance;
        static void event_callback(const EventCallbackStruct& event);
        void handle_event(const EventCallbackStruct& event);


        Vector2 coord_to_pixel(unsigned short x, unsigned short y);
        Vector2 pixel_to_coord(int x, int y);
        static int coord_distance(Vector2 coord_1, Vector2 coord_2);
        Array get_building_occupied_cells(CNCObjectStruct* building);
        bool get_cell_is_shrouded(int x, int y);

        bool cache_game_state();
        CNCObjectStruct* get_nearest_object(int x, int y);

        Image* game_image;
        PoolByteArray game_buffer_pba;
        PoolByteArray game_palette_pba;
        PoolByteArray speech_buffer;

    public:
        static void _register_methods();

        GDNativeAlert();
        ~GDNativeAlert();

        void _init();

        bool start_instance(int scenario_index, int build_level, String faction);
        bool advance_instance();
        PoolByteArray get_palette();
        bool get_visible_page();
        void handle_left_mouse_up(unsigned int x, unsigned int y);
        void handle_right_mouse_up(unsigned int x, unsigned int y);
        void handle_right_mouse_down(unsigned int x, unsigned int y);
        void handle_mouse_area(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
        void handle_mouse_motion(unsigned int x, unsigned int y);
        String get_cursor_name(int x, int y);
        Array get_game_objects();
        Dictionary get_sidebar_state();
    };

}

#endif
