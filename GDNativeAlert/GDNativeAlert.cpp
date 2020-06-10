#include "GDNativeAlert.h"

#include <iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#include <WinUser.h>
#endif

#include <ProjectSettings.hpp>
#include <ConfigFile.hpp>

#include "mixfile.h"
#include "pk.h"
#include "gamefile.h"
#include "ini.h"

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

    PKey fastKey = PKey();
    uint8_t buffer[512];

    // From PK.H
    Int<MAX_UNIT_PRECISION> exp(65537L);
    MPMath::DER_Encode(exp, buffer, MAX_UNIT_PRECISION);
    fastKey.Decode_Exponent(buffer);

    // From CONST.CPP, base64 encoded
    // "AihRvNoIbTn85FZRYNZRcT+i6KpU+maCsEqr3Q5q+LDB5tH7Tz2qQ38V"
    const char mod[] = { '\x02', '\x28', '\x51', '\xbc', '\xda', '\x08', '\x6d', '\x39', '\xfc', '\xe4', '\x56', '\x51', '\x60', '\xd6', '\x51', '\x71',
                         '\x3f', '\xa2', '\xe8', '\xaa', '\x54', '\xfa', '\x66', '\x82', '\xb0', '\x4a', '\xab', '\xdd', '\x0e', '\x6a', '\xf8', '\xb0',
                         '\xc1', '\xe6', '\xd1', '\xfb', '\x4f', '\x3d', '\xaa', '\x43', '\x7f', '\x15' };
    fastKey.Decode_Modulus(mod);

    GameMixFile* redalert_mix = new GameMixFile("RedAlert\\REDALERT.MIX", &fastKey);
    GameMixFile::Cache("REDALERT.MIX");
    GameMixFile* local_mix = new GameMixFile("LOCAL.MIX", &fastKey);
    GameMixFile::Cache("LOCAL.MIX");

    INIClass RuleINI;
    GameFileClass ini_file = GameFileClass("RULES.INI");
    RuleINI.Load(ini_file);

    CNCRulesDataStruct rules;
    rules.Difficulties[0].FirepowerBias = RuleINI.Get_Float("Easy", "Firepower");
    rules.Difficulties[0].GroundspeedBias = RuleINI.Get_Float("Easy", "Groundspeed");
    rules.Difficulties[0].AirspeedBias = RuleINI.Get_Float("Easy", "Airspeed");
    rules.Difficulties[0].ArmorBias = RuleINI.Get_Float("Easy", "Armor");
    rules.Difficulties[0].ROFBias = RuleINI.Get_Float("Easy", "ROF");
    rules.Difficulties[0].CostBias = RuleINI.Get_Float("Easy", "Cost");
    rules.Difficulties[0].BuildSpeedBias = RuleINI.Get_Float("Easy", "BuildTime");
    rules.Difficulties[0].RepairDelay = RuleINI.Get_Float("Easy", "RepairDelay");
    rules.Difficulties[0].BuildDelay = RuleINI.Get_Float("Easy", "BuildDelay");
    rules.Difficulties[0].IsBuildSlowdown = RuleINI.Get_Bool("Easy", "BuildSlowdown");
    rules.Difficulties[0].IsWallDestroyer = RuleINI.Get_Bool("Easy", "DestroyWalls");
    rules.Difficulties[0].IsContentScan = RuleINI.Get_Bool("Easy", "ContentScan");

    rules.Difficulties[1].FirepowerBias = RuleINI.Get_Float("Normal", "Firepower");
    rules.Difficulties[1].GroundspeedBias = RuleINI.Get_Float("Normal", "Groundspeed");
    rules.Difficulties[1].AirspeedBias = RuleINI.Get_Float("Normal", "Airspeed");
    rules.Difficulties[1].ArmorBias = RuleINI.Get_Float("Normal", "Armor");
    rules.Difficulties[1].ROFBias = RuleINI.Get_Float("Normal", "ROF");
    rules.Difficulties[1].CostBias = RuleINI.Get_Float("Normal", "Cost");
    rules.Difficulties[1].BuildSpeedBias = RuleINI.Get_Float("Normal", "BuildTime");
    rules.Difficulties[1].RepairDelay = RuleINI.Get_Float("Normal", "RepairDelay");
    rules.Difficulties[1].BuildDelay = RuleINI.Get_Float("Normal", "BuildDelay");
    rules.Difficulties[1].IsBuildSlowdown = RuleINI.Get_Bool("Normal", "BuildSlowdown");
    rules.Difficulties[1].IsWallDestroyer = RuleINI.Get_Bool("Normal", "DestroyWalls");
    rules.Difficulties[1].IsContentScan = RuleINI.Get_Bool("Normal", "ContentScan");

    rules.Difficulties[2].FirepowerBias = RuleINI.Get_Float("Difficult", "Firepower");
    rules.Difficulties[2].GroundspeedBias = RuleINI.Get_Float("Difficult", "Groundspeed");
    rules.Difficulties[2].AirspeedBias = RuleINI.Get_Float("Difficult", "Airspeed");
    rules.Difficulties[2].ArmorBias = RuleINI.Get_Float("Difficult", "Armor");
    rules.Difficulties[2].ROFBias = RuleINI.Get_Float("Difficult", "ROF");
    rules.Difficulties[2].CostBias = RuleINI.Get_Float("Difficult", "Cost");
    rules.Difficulties[2].BuildSpeedBias = RuleINI.Get_Float("Difficult", "BuildTime");
    rules.Difficulties[2].RepairDelay = RuleINI.Get_Float("Difficult", "RepairDelay");
    rules.Difficulties[2].BuildDelay = RuleINI.Get_Float("Difficult", "BuildDelay");
    rules.Difficulties[2].IsBuildSlowdown = RuleINI.Get_Bool("Difficult", "BuildSlowdown");
    rules.Difficulties[2].IsWallDestroyer = RuleINI.Get_Bool("Difficult", "DestroyWalls");
    rules.Difficulties[2].IsContentScan = RuleINI.Get_Bool("Difficult", "ContentScan");
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
