#include "GDNativeAlert.h"

#include <iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#include <WinUser.h>
#endif

#include <ProjectSettings.hpp>
#include <ConfigFile.hpp>
#include <AudioStreamPlayer.hpp>
#include <Image.hpp>

#include "mixfile.h"
#include "gamefile.h"
#include "ini.h"


using namespace godot;

void GDNativeAlert::_register_methods() {
    register_method("start_instance", &GDNativeAlert::start_instance);
    register_method("advance_instance", &GDNativeAlert::advance_instance);
    register_method("get_palette", &GDNativeAlert::get_palette);
    register_method("get_visible_page", &GDNativeAlert::get_visible_page);
    register_method("handle_left_mouse_up", &GDNativeAlert::handle_left_mouse_up);
    register_method("handle_right_mouse_up", &GDNativeAlert::handle_right_mouse_up);
    register_method("handle_right_mouse_down", &GDNativeAlert::handle_right_mouse_down);
    register_method("handle_mouse_area", &GDNativeAlert::handle_mouse_area);
    register_method("handle_mouse_motion", &GDNativeAlert::handle_mouse_motion);
    register_method("get_cursor_name", &GDNativeAlert::get_cursor_name);
    register_method("get_game_objects", &GDNativeAlert::get_game_objects);

    register_signal<GDNativeAlert>("event", "message", GODOT_VARIANT_TYPE_STRING);
    register_signal<GDNativeAlert>("play_sound", "name", GODOT_VARIANT_TYPE_STRING, "x", GODOT_VARIANT_TYPE_INT, "y", GODOT_VARIANT_TYPE_INT);
    register_signal<GDNativeAlert>("play_speech", "name", GODOT_VARIANT_TYPE_STRING);
}

GDNativeAlert::GDNativeAlert() {
    game_image = Image::_new();
}

GDNativeAlert::~GDNativeAlert() {
    godot::api->godot_free(game_image);
}

void GDNativeAlert::_init() {
    String content_path = ProjectSettings::get_singleton()->globalize_path("res://RedAlert");
    String command_line = "-CD\"" + content_path.replace("/", "\\") + "\"";

    char* command_line_cstr = command_line.alloc_c_string();
    callback_instance = this;
    CNC_Init(command_line_cstr, &event_callback);
    if (command_line_cstr != nullptr) godot::api->godot_free(command_line_cstr);
#ifdef WIN32
    ShowCursor(TRUE);
#endif

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

void GDNativeAlert::play_sound(String name, bool is_speech, int x = 0, int y = 0) {
    if (is_speech || x < 1 || y < 1)
    {
        emit_signal("play_speech", name);
    }
    else
    {
        emit_signal("play_sound", name, x, y);
    }
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
            CNCMapDataStruct* state = new CNCMapDataStruct;
            CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)state, sizeof(CNCMapDataStruct));
            play_sound(event.SoundEffect.SoundEffectName, false, event.SoundEffect.PixelX - (state->OriginalMapCellX * 24.5), event.SoundEffect.PixelY - (state->OriginalMapCellY * 24.5));
            delete state;
        }
        break;
        case (CALLBACK_EVENT_SPEECH):
        {
            play_sound(event.Speech.SpeechName, true);
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

    if (message.length()) {
        emit_signal("event", message);
    }
}

bool GDNativeAlert::start_instance(int scenario_index, int build_level, String faction) {
    game_buffer_pba.resize(3072 * 3072);

    char* faction_cstr = faction.alloc_c_string();
    return CNC_Start_Instance(scenario_index, build_level, faction_cstr, "GAME_NORMAL", "", NULL, NULL);
    if (faction_cstr != nullptr) godot::api->godot_free(faction_cstr);
}

bool GDNativeAlert::advance_instance(uint64 player_id) {
    return CNC_Advance_Instance(player_id);
}

PoolByteArray GDNativeAlert::get_palette() {
    unsigned char palette[256][3];
    CNC_Get_Palette((unsigned char(&)[256][3])palette);
    PoolByteArray palette_buffer;
    palette_buffer.resize(256 * 3);
    {
        PoolByteArray::Write palette_pba_write = palette_buffer.write();
        unsigned char* palette_pba_data = palette_pba_write.ptr();
        memcpy(palette_pba_data, palette, 256 * 3);
    }
    return palette_buffer;
}

bool GDNativeAlert::get_visible_page() {
    unsigned int width = 0;
    unsigned int height = 0;

    {
        PoolByteArray::Write pba_write = game_buffer_pba.write();
        unsigned char* pba_data = pba_write.ptr();
        if (!CNC_Get_Visible_Page(pba_data, width, height)) {
            return false;
        }
    }

    if (width != get_width() || height != get_height())
    {
        game_buffer_pba.resize(width * height);
    }

    Image* image = Image::_new();
    image->create_from_data(width, height, false, Image::FORMAT_L8, game_buffer_pba);
    set_data(image);
    set_size_override(Vector2(width, height));

    return true;
}

void GDNativeAlert::handle_left_mouse_up(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_LEFT_CLICK, NULL, 0, x, y, NULL, NULL);
}

void GDNativeAlert::handle_right_mouse_up(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_RIGHT_CLICK, NULL, 0, x, y, NULL, NULL);
}

void GDNativeAlert::handle_right_mouse_down(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_RIGHT_DOWN, NULL, 0, x, y, NULL, NULL);
}

void GDNativeAlert::handle_mouse_area(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_AREA, NULL, 0, x1, y1, x2, y2);
}

void GDNativeAlert::handle_mouse_motion(unsigned int x, unsigned int y) {
    CNC_Handle_Input(INPUT_REQUEST_MOUSE_MOVE, NULL, 0, x, y, NULL, NULL);
}

String GDNativeAlert::get_cursor_name(real_t x, real_t y) {
    unsigned int buff_size = sizeof(CNCPlayerInfoStruct) + 33;
    uint8_t* state = new uint8_t[buff_size];
    CNC_Get_Game_State(GAME_STATE_PLAYER_INFO, 0, (unsigned char*)state, buff_size);
    CNCPlayerInfoStruct* player_state = (CNCPlayerInfoStruct*)state;

    unsigned char* layer_state = new unsigned char[0x100000];
    bool got_layers = CNC_Get_Game_State(GAME_STATE_LAYERS, 0, layer_state, 0x10000);
    CNCObjectListStruct* layers = (CNCObjectListStruct*)layer_state;

    CNCMapDataStruct* map_state = new CNCMapDataStruct;
    CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)map_state, sizeof(CNCMapDataStruct));

    CNCObjectStruct* nearest_object = get_nearest_object(layers, (map_state->OriginalMapCellX * 24) + x, (map_state->OriginalMapCellY * 24) + y);
    if (nearest_object != nullptr)
    {
        DllActionTypeEnum action = nearest_object->ActionWithSelected[player_state->House];

        switch (action)
        {
        case DAT_ATTACK:
            delete[] state;
            delete[] layer_state;
            delete map_state;
            return "MOUSE_CAN_ATTACK";
        case DAT_ATTACK_OUT_OF_RANGE:
            delete[] state;
            delete[] layer_state;
            delete map_state;
            return "MOUSE_STAY_ATTACK";
        case DAT_SABOTAGE:
            delete[] state;
            delete[] layer_state;
            delete map_state;
            return "MOUSE_DEMOLITIONS";
        case DAT_ENTER:
            delete[] state;
            delete[] layer_state;
            delete map_state;
            return "MOUSE_ENTER";
        case DAT_SELECT:
            delete[] state;
            delete[] layer_state;
            delete map_state;
            return "MOUSE_CAN_SELECT";
        }
    }

    if (player_state->SelectedID == -1)
    {
        String mouse = "MOUSE_NORMAL";

        if (nearest_object != nullptr)
        {
            mouse = "MOUSE_CAN_SELECT";
        }

        delete[] state;
        delete[] layer_state;
        delete map_state;
        return mouse;
    }

    unsigned int action_count = player_state->ActionWithSelectedCount;

    DllActionTypeEnum* actions = new DllActionTypeEnum[action_count];
    actions = player_state->ActionWithSelected;

    int map_cell_x = (x / 24);
    int map_cell_y = (y / 24);

    DllActionTypeEnum action = actions[map_cell_y * map_state->OriginalMapCellWidth + map_cell_x];

    delete[] state;
    delete[] layer_state;
    delete map_state;

    switch (action)
    {
    case DAT_MOVE:
        return "MOUSE_CAN_MOVE";
    case DAT_NOMOVE:
        return "MOUSE_NO_MOVE";
    default:
        return "MOUSE_NORMAL";
    }
}

static float lepton_to_pixel(int map_cell, unsigned short in) {
    int map_lepton = map_cell * 256;
    in -= map_lepton;
    return ((float)in / 256.0) * 24.0;
}

Array GDNativeAlert::get_game_objects() {
    unsigned char* layer_state = new unsigned char[0x200000];
    bool got_layers = CNC_Get_Game_State(GAME_STATE_LAYERS, 0, layer_state, 0x20000);
    CNCObjectListStruct* layers = (CNCObjectListStruct*)layer_state;

    CNCMapDataStruct* map_state = new CNCMapDataStruct;
    CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)map_state, sizeof(CNCMapDataStruct));

    Array objects;

    for (int i = 0; i < layers->Count; i++) {
        CNCObjectStruct* object = layers->Objects + i;

        Dictionary object_dict;

        object_dict["PositionX"] = object->PositionX;
        object_dict["PositionY"] = object->PositionY;

        object_dict["CenterCoordX"] = lepton_to_pixel(map_state->OriginalMapCellX, object->CenterCoordX);
        object_dict["CenterCoordY"] = lepton_to_pixel(map_state->OriginalMapCellY, object->CenterCoordY);

        object_dict["DimensionX"] = object->DimensionX;
        object_dict["DimensionY"] = object->DimensionY;

        real_t top_left_x = object->CenterCoordX - (((object->DimensionX / 24) * 256) / 2);
        real_t top_left_y = object->CenterCoordY - (((object->DimensionY / 24) * 256) / 2);

        int top_left_cell_x = top_left_x / 256;
        int top_left_cell_y = top_left_y / 256;

        Array occupy_list;

        for (int i = 0; i < object->OccupyListLength; i++) {
            short cell_offset = object->OccupyList[i];
            int cell_offset_x = cell_offset % 128;
            int cell_offset_y = cell_offset / 128;
            int cell_new_x = (top_left_cell_x + cell_offset_x) - map_state->OriginalMapCellX;
            int cell_new_y = (top_left_cell_y + cell_offset_y) - map_state->OriginalMapCellY;
            occupy_list.push_front(Vector2(cell_new_x * 24, cell_new_y * 24));
        }

        object_dict["OccupyList"] = occupy_list;

        objects.push_front(object_dict);
    }

    delete[] layer_state;
    delete map_state;

    return objects;
}

int GDNativeAlert::distance(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    int	diff1, diff2;

    diff1 = y1 - y2;
    if (diff1 < 0) diff1 = -diff1;
    diff2 = x1 - x2;
    if (diff2 < 0) diff2 = -diff2;
    if (diff1 > diff2) {
        return(diff1 + ((unsigned)diff2 / 2));
    }
    return(diff2 + ((unsigned)diff1 / 2));
}

CNCObjectStruct* GDNativeAlert::get_nearest_object(CNCObjectListStruct* layers, real_t x, real_t y) {
    int lepton_position_x = (x / 24) * 256;
    int lepton_position_y = (y / 24) * 256;

    CNCObjectStruct* nearest_object = nullptr;
    int nearest_object_distance;
    for (int i = 0; i < layers->Count; i++) {
        CNCObjectStruct* object = layers->Objects + i;
        if (!object->IsSelectable) continue;

        if (object->Type == BUILDING || object->Type == BUILDING_TYPE) {
            int top_left_x = object->CenterCoordX - (((object->DimensionX / 24) * 256) / 2);
            int top_left_y = object->CenterCoordY - (((object->DimensionY / 24) * 256) / 2);

            int top_left_cell_x = top_left_x / 256;
            int top_left_cell_y = top_left_y / 256;

            for (int i = 0; i < object->OccupyListLength; i++) {
                short cell_offset = object->OccupyList[i];
                int cell_offset_x = cell_offset % 128;
                int cell_offset_y = cell_offset / 128;
                int cell_new_x = (top_left_cell_x + cell_offset_x);
                int cell_new_y = (top_left_cell_y + cell_offset_y);

                int cell_x = (cell_new_x * 256) + 128;
                int cell_y = (cell_new_y * 256) + 128;
                int distance = GDNativeAlert::distance(cell_x, cell_y, lepton_position_x, lepton_position_y);
                if (distance < 192) {
                    if (nearest_object != nullptr) {
                        if (distance < nearest_object_distance) {
                            nearest_object = object;
                            nearest_object_distance = distance;
                        }
                    }
                    else {
                        nearest_object = object;
                        nearest_object_distance = distance;
                    }
                }
            }
        }
        else {
            int distance = GDNativeAlert::distance(object->CenterCoordX, object->CenterCoordY, lepton_position_x, lepton_position_y);
            if (distance < 192) {
                if (nearest_object != nullptr) {
                    if (distance < nearest_object_distance) {
                        nearest_object = object;
                        nearest_object_distance = distance;
                    }
                }
                else {
                    nearest_object = object;
                    nearest_object_distance = distance;
                }
            }
        }
    }

    if (nearest_object == nullptr) {
        return nullptr;
    }

    return nearest_object;
}
