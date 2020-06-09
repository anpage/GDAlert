#include "GDNativeAlert.h"

#include<iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#include <WinUser.h>
#endif

#include <ProjectSettings.hpp>

using namespace godot;

void GDNativeAlert::_register_methods() {
    register_method("cnc_start_instance", &GDNativeAlert::cnc_start_instance);
    register_method("cnc_advance_instance", &GDNativeAlert::cnc_advance_instance);
    register_method("cnc_get_visible_page", &GDNativeAlert::cnc_get_visible_page);
    register_method("cnc_get_visible_page_width", &GDNativeAlert::cnc_get_visible_page_width);
    register_method("cnc_get_visible_page_height", &GDNativeAlert::cnc_get_visible_page_height);
    register_method("cnc_handle_left_mouse_up", &GDNativeAlert::cnc_handle_left_mouse_up);
    register_method("cnc_handle_right_mouse_up", &GDNativeAlert::cnc_handle_right_mouse_up);
    register_method("cnc_handle_right_mouse_down", &GDNativeAlert::cnc_handle_right_mouse_down);
    register_method("cnc_handle_mouse_area", &GDNativeAlert::cnc_handle_mouse_area);

    register_signal<GDNativeAlert>("event", "message", GODOT_VARIANT_TYPE_STRING);
}

GDNativeAlert::GDNativeAlert() {
}

GDNativeAlert::~GDNativeAlert() {
}

void GDNativeAlert::_init() {
    game_buffer_width = 0;
    game_buffer_height = 0;
    
    String content_path = ProjectSettings::get_singleton()->globalize_path("res://RedAlert");
    String command_line = "-CD\"" + content_path.replace("/", "\\") + "\"";

    char* command_line_cstr = command_line.alloc_c_string();
    callback_instance = this;
    CNC_Init(command_line_cstr, &event_callback);
    if (command_line_cstr != nullptr) godot::api->godot_free(command_line_cstr);
#ifdef WIN32
    ShowCursor(TRUE);
#endif

    CNCRulesDataStruct rules;
    // Rules taken from original game
    // Easy
    rules.Difficulties[0].FirepowerBias = 1.000f;
    rules.Difficulties[0].GroundspeedBias = 1.200f;
    rules.Difficulties[0].AirspeedBias = 1.200f;
    rules.Difficulties[0].ArmorBias = 1.200f;
    rules.Difficulties[0].ROFBias = 0.800f;
    rules.Difficulties[0].CostBias = 0.800f;
    rules.Difficulties[0].BuildSpeedBias = 0.800f;
    rules.Difficulties[0].RepairDelay = 0.001f;
    rules.Difficulties[0].BuildDelay = 0.001f;
    rules.Difficulties[0].IsBuildSlowdown = false;
    rules.Difficulties[0].IsWallDestroyer = true;
    rules.Difficulties[0].IsContentScan = true;
    // Medium
    rules.Difficulties[1].FirepowerBias = 1.000f;
    rules.Difficulties[1].GroundspeedBias = 1.000f;
    rules.Difficulties[1].AirspeedBias = 1.000f;
    rules.Difficulties[1].ArmorBias = 1.000f;
    rules.Difficulties[1].ROFBias = 1.000f;
    rules.Difficulties[1].CostBias = 1.000f;
    rules.Difficulties[1].BuildSpeedBias = 1.000f;
    rules.Difficulties[1].RepairDelay = 0.020f;
    rules.Difficulties[1].BuildDelay = 0.030f;
    rules.Difficulties[1].IsBuildSlowdown = true;
    rules.Difficulties[1].IsWallDestroyer = true;
    rules.Difficulties[1].IsContentScan = true;
    //Hard
    rules.Difficulties[2].FirepowerBias = 1.000f;
    rules.Difficulties[2].GroundspeedBias = 0.800f;
    rules.Difficulties[2].AirspeedBias = 0.800f;
    rules.Difficulties[2].ArmorBias = 0.800f;
    rules.Difficulties[2].ROFBias = 1.200f;
    rules.Difficulties[2].CostBias = 1.000f;
    rules.Difficulties[2].BuildSpeedBias = 1.000f;
    rules.Difficulties[2].RepairDelay = 0.050f;
    rules.Difficulties[2].BuildDelay = 0.100f;
    rules.Difficulties[2].IsBuildSlowdown = true;
    rules.Difficulties[2].IsWallDestroyer = false;
    rules.Difficulties[2].IsContentScan = false;
    CNC_Config(rules);
}

GDNativeAlert* GDNativeAlert::callback_instance;

void GDNativeAlert::event_callback(const EventCallbackStruct& event) {
    if (callback_instance != NULL) {
        callback_instance->handle_event(event);
    }
}

void GDNativeAlert::handle_event(const EventCallbackStruct& event) {
    String message;

    switch (event.EventType)
    {
        case (CALLBACK_EVENT_INVALID):
        {
            message = "Unhandled event: CALLBACK_EVENT_INVALID";
        }
        break;
        case (CALLBACK_EVENT_SOUND_EFFECT):
        {
            String effect(event.SoundEffect.SoundEffectName);
            message = "Sound effect: " + effect;
        }
        break;
        case (CALLBACK_EVENT_SPEECH):
        {
            String speech(event.Speech.SpeechName);
            message = "Speech: " + speech;
        }
        break;
        case (CALLBACK_EVENT_GAME_OVER):
            message = "Unhandled event: CALLBACK_EVENT_GAME_OVER";
        break;
        case (CALLBACK_EVENT_DEBUG_PRINT):
        {
            String debug(event.DebugPrint.PrintString);
            message = "DEBUG PRINT: " + debug;
        }
        break;
        case (CALLBACK_EVENT_MOVIE):
        {
            String movie(event.Movie.MovieName);
            message = "Play movie: " + movie;
        }
        break;
        case (CALLBACK_EVENT_MESSAGE):
        {
            String msg(event.Message.Message);
            message = "Incoming message: " + msg;
        }
        break;
        case (CALLBACK_EVENT_UPDATE_MAP_CELL):
            message = "Unhandled event: CALLBACK_EVENT_UPDATE_MAP_CELL";
        break;
        case (CALLBACK_EVENT_ACHIEVEMENT):
        {
            String type(event.Achievement.AchievementType);
            String reason(event.Achievement.AchievementReason);
            message = String("Achievement unlocked: ") + type + String(", ") + reason;
        }
        break;
        case (CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS):
            message = "Unhandled event: CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS";
        break;
        case (CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING):
            message = "Unhandled event: CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING";
        break;
        case (CALLBACK_EVENT_BRIEFING_SCREEN):
            message = "Unhandled event: CALLBACK_EVENT_BRIEFING_SCREEN";
        break;
        case (CALLBACK_EVENT_CENTER_CAMERA):
            message = "Unhandled event: CALLBACK_EVENT_CENTER_CAMERA";
        break;
        case (CALLBACK_EVENT_PING):
            message = "Unhandled event: CALLBACK_EVENT_PING";
        break;
        default:
        {
            String type(event.EventType);
            message = String("Unknown event type: ") + type;
        }
        break;
    }

    emit_signal("event", message);
}

bool GDNativeAlert::cnc_start_instance(int scenario_index, int build_level, String faction) {
    char* faction_cstr = faction.alloc_c_string();
    return CNC_Start_Instance(scenario_index, build_level, faction_cstr, "GAME_NORMAL", "", NULL, NULL);
    if (faction_cstr != nullptr) godot::api->godot_free(faction_cstr);
}

bool GDNativeAlert::cnc_advance_instance() {
    return CNC_Advance_Instance(0);
}

PoolByteArray GDNativeAlert::cnc_get_visible_page() {
    // Get palette
    unsigned char palette[256][3];
    CNC_Get_Palette(palette);

    // Get visible page
    unsigned int width = 0;
    unsigned int height = 0;
    CNC_Get_Visible_Page(game_buffer, width, height);
    if (width != game_buffer_width || height != game_buffer_height)
    {
        game_buffer_width = width;
        game_buffer_height = height;
        game_buffer_pba.resize(width * height * 4);
    }

    // Get pointer to byte array data
    PoolByteArray::Write pba_write = game_buffer_pba.write();
    unsigned char* pba_data = pba_write.ptr();

    // Convert visible page to FORMAT_RGBA8
    for (unsigned int pixel = 0; pixel < width * height; pixel++)
    {
        unsigned int pixel_out = pixel * 4;
        for (unsigned int component = 0; component <= 2; component++)
        {
            // R, G, and B
            pba_data[pixel_out + component] = palette[game_buffer[pixel]][component] << 2;
        }
        // A
        pba_data[pixel_out + 3] = 0xFF;
    }

    return game_buffer_pba;
}

unsigned int GDNativeAlert::cnc_get_visible_page_width() {
    return game_buffer_width;
}

unsigned int GDNativeAlert::cnc_get_visible_page_height() {
    return game_buffer_height;
}

void GDNativeAlert::cnc_handle_left_mouse_up(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_LEFT_CLICK, NULL, 0, x, y, NULL, NULL);
}

void GDNativeAlert::cnc_handle_right_mouse_up(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_RIGHT_CLICK, NULL, 0, x, y, NULL, NULL);
}

void GDNativeAlert::cnc_handle_right_mouse_down(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_RIGHT_DOWN, NULL, 0, x, y, NULL, NULL);
}

void GDNativeAlert::cnc_handle_mouse_area(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_AREA, NULL, 0, x1, y1, x2, y2);
}
