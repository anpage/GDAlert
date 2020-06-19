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

Vector2 GDNativeAlert::coord_to_pixel(unsigned short x, unsigned short y) {
    int map_lepton_x = static_map_state_cache->OriginalMapCellX * LEPTONS_PER_CELL;
    int map_lepton_y = static_map_state_cache->OriginalMapCellY * LEPTONS_PER_CELL;
    x -= map_lepton_x;
    y -= map_lepton_y;
    return Vector2(((float)x / 256.0) * 24.0, ((float)y / 256.0) * 24.0);
}

Vector2 GDNativeAlert::pixel_to_coord(int x, int y) {
    int map_lepton_x = static_map_state_cache->OriginalMapCellX * LEPTONS_PER_CELL;
    int map_lepton_y = static_map_state_cache->OriginalMapCellY * LEPTONS_PER_CELL;
    unsigned short lepton_x = (((x * LEPTONS_PER_CELL) + (PIXELS_PER_CELL / 2) - ((x < 0) ? (PIXELS_PER_CELL - 1) : 0)) / PIXELS_PER_CELL);
    unsigned short lepton_y = (((y * LEPTONS_PER_CELL) + (PIXELS_PER_CELL / 2) - ((y < 0) ? (PIXELS_PER_CELL - 1) : 0)) / PIXELS_PER_CELL);
    return Vector2(lepton_x + map_lepton_x, lepton_y + map_lepton_y);
}

int GDNativeAlert::coord_distance(Vector2 coord_1, Vector2 coord_2)
{
    int	diff1, diff2;

    diff1 = coord_1.y - coord_2.y;
    if (diff1 < 0) diff1 = -diff1;
    diff2 = coord_1.x - coord_2.x;
    if (diff2 < 0) diff2 = -diff2;
    if (diff1 > diff2) {
        return(diff1 + ((unsigned)diff2 / 2));
    }
    return(diff2 + ((unsigned)diff1 / 2));
}

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

    register_signal<GDNativeAlert>("event_not_handled", "message", GODOT_VARIANT_TYPE_STRING);
    register_signal<GDNativeAlert>("sound_played", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("speech_played", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("game_ended", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("debug_printed", "message", GODOT_VARIANT_TYPE_STRING);
    register_signal<GDNativeAlert>("movie_played", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("message_received", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("map_cell_updated", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("achievement_triggered", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("carryover_objects_stored", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("special_weapon_targeted", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("briefing_screen_triggered", "event", GODOT_VARIANT_TYPE_DICTIONARY);
    register_signal<GDNativeAlert>("camera_centered", "position", GODOT_VARIANT_TYPE_VECTOR2);
    register_signal<GDNativeAlert>("pinged", "position", GODOT_VARIANT_TYPE_VECTOR2);
}

GDNativeAlert::GDNativeAlert() {
    game_image = Image::_new();

    player_state_cache = (CNCPlayerInfoStruct*)new unsigned char[sizeof(CNCPlayerInfoStruct) + 33];
    game_state_cache = (CNCObjectListStruct*)new unsigned char[GAME_STATE_BUFFER_SIZE];
    static_map_state_cache = new CNCMapDataStruct;
}

GDNativeAlert::~GDNativeAlert() {
    godot::api->godot_free(game_image);

    delete[] player_state_cache;
    delete[] game_state_cache;
    delete static_map_state_cache;
}

void GDNativeAlert::_init() {
    String content_path = ProjectSettings::get_singleton()->globalize_path("res://RedAlert");
    String command_line = "-CD\"" + content_path + "\"";

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
            message = "CALLBACK_EVENT_INVALID";
        }
        break;
        case (CALLBACK_EVENT_SOUND_EFFECT):
        {
            int x = event.SoundEffect.PixelX;
            int y = event.SoundEffect.PixelY;

            if (x > 0 && y > 0) {
                CNCMapDataStruct* state = new CNCMapDataStruct;
                CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)state, sizeof(CNCMapDataStruct));
                x = event.SoundEffect.PixelX - (state->OriginalMapCellX * 24);
                y = event.SoundEffect.PixelY - (state->OriginalMapCellY * 24);
                delete state;
            }

            Dictionary event_sound_effect;
            event_sound_effect["sfx_index"] = event.SoundEffect.SFXIndex;
            event_sound_effect["variation"] = event.SoundEffect.Variation;
            event_sound_effect["position"] = Vector2(x, y);
            event_sound_effect["player_id"] = event.SoundEffect.PlayerID;
            event_sound_effect["sound_effect_name"] = event.SoundEffect.SoundEffectName;
            event_sound_effect["sound_effect_priority"] = event.SoundEffect.SoundEffectPriority;
            event_sound_effect["sound_effect_context"] = event.SoundEffect.SoundEffectContext;

            emit_signal("sound_played", event_sound_effect);
        }
        break;
        case (CALLBACK_EVENT_SPEECH):
        {
            Dictionary event_speech;
            event_speech["player_id"] = event.Speech.PlayerID;
            event_speech["speech_index"] = event.Speech.SpeechIndex;
            event_speech["speech_name"] = event.Speech.SpeechName;

            emit_signal("speech_played", event_speech);
        }
        break;
        case (CALLBACK_EVENT_GAME_OVER):
        {
            Dictionary event_game_over;
            event_game_over["multiplayer"] = event.GameOver.Multiplayer;
            event_game_over["player_wins"] = event.GameOver.PlayerWins;
            event_game_over["movie_name"] = event.GameOver.MovieName;
            event_game_over["movie_name_2"] = event.GameOver.MovieName2;
            event_game_over["movie_name_3"] = event.GameOver.MovieName3;
            event_game_over["movie_name_4"] = event.GameOver.MovieName4;
            event_game_over["after_score_movie_name"] = event.GameOver.AfterScoreMovieName;
            event_game_over["score"] = event.GameOver.Score;
            event_game_over["leadership"] = event.GameOver.Leadership;
            event_game_over["efficiency"] = event.GameOver.Efficiency;
            event_game_over["category_total"] = event.GameOver.CategoryTotal;
            event_game_over["nod_killed"] = event.GameOver.NODKilled;
            event_game_over["gdi_killed"] = event.GameOver.GDIKilled;
            event_game_over["civilians_killed"] = event.GameOver.CiviliansKilled;
            event_game_over["nod_buildings_destroyed"] = event.GameOver.NODBuildingsDestroyed;
            event_game_over["gdi_buildings_destroyed"] = event.GameOver.GDIBuildingsDestroyed;
            event_game_over["civilian_buildings_destroyed"] = event.GameOver.CiviliansBuildingsDestroyed;
            event_game_over["remaining_credits"] = event.GameOver.RemainingCredits;
            event_game_over["sabotaged_structure_type"] = event.GameOver.SabotagedStructureType;
            event_game_over["timer_remaining"] = event.GameOver.TimerRemaining;

            emit_signal("game_ended", event_game_over);
        }
        break;
        case (CALLBACK_EVENT_DEBUG_PRINT):
        {
            String debug(event.DebugPrint.PrintString);
            message = "DEBUG PRINT: " + debug;
        }
        break;
        case (CALLBACK_EVENT_MOVIE):
        {
            Dictionary event_movie;
            event_movie["movie_name"] = event.Movie.MovieName;
            event_movie["theme"] = event.Movie.Theme;
            event_movie["immediate"] = event.Movie.Immediate;

            emit_signal("movie_played", event_movie);
        }
        break;
        case (CALLBACK_EVENT_MESSAGE):
        {
            Dictionary event_message;
            event_message["message"] = event.Message.Message;
            event_message["timeout_seconds"] = event.Message.TimeoutSeconds;
            event_message["message_type"] = event.Message.MessageType;
            event_message["message_param"] = event.Message.MessageParam1;

            emit_signal("message_received", event_message);
        }
        break;
        case (CALLBACK_EVENT_UPDATE_MAP_CELL):
        {
            Dictionary event_update_map_cell;
            event_update_map_cell["cell_position"] = Vector2(event.UpdateMapCell.CellX, event.UpdateMapCell.CellY);
            event_update_map_cell["template_type_name"] = event.UpdateMapCell.TemplateTypeName;

            emit_signal("map_cell_updated", event_update_map_cell);
        }
        break;
        case (CALLBACK_EVENT_ACHIEVEMENT):
        {
            Dictionary event_achievement;
            event_achievement["achievement_type"] = event.Achievement.AchievementType;
            event_achievement["achievement_reason"] = event.Achievement.AchievementReason;

            emit_signal("achievement_triggered", event_achievement);
        }
        break;
        case (CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS):
            message = "Unhandled event: CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS";
        break;
        case (CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING):
        {
            Dictionary event_special_weapon_targeting;
            event_special_weapon_targeting["type"] = event.SpecialWeaponTargetting.Type;
            event_special_weapon_targeting["id"] = event.SpecialWeaponTargetting.ID;
            event_special_weapon_targeting["name"] = event.SpecialWeaponTargetting.Name;
            event_special_weapon_targeting["weapon_type"] = event.SpecialWeaponTargetting.WeaponType;

            emit_signal("special_weapon_targeted", event_special_weapon_targeting);
        }
        break;
        case (CALLBACK_EVENT_BRIEFING_SCREEN):
            message = "Unhandled event: CALLBACK_EVENT_BRIEFING_SCREEN";
        break;
        case (CALLBACK_EVENT_CENTER_CAMERA):
        {
            Vector2 pixel_position = coord_to_pixel(event.CenterCamera.CoordX, event.CenterCamera.CoordY);
            emit_signal("camera_centered", pixel_position);
        }
        break;
        case (CALLBACK_EVENT_PING):
        {
            Vector2 pixel_position = coord_to_pixel(event.Ping.CoordX, event.Ping.CoordY);
            emit_signal("pinged", pixel_position);
        }
        break;
        default:
        {
            message = event.EventType;
        }
        break;
    }

    if (message.length()) {
        emit_signal("event_not_handled", message);
    }
}

bool GDNativeAlert::start_instance(int scenario_index, int build_level, String faction) {
    game_buffer_pba.resize(3072 * 3072);

    char* faction_cstr = faction.alloc_c_string();
    bool instance_started = CNC_Start_Instance(scenario_index, build_level, faction_cstr, "GAME_NORMAL", "", NULL, NULL);
    if (faction_cstr != nullptr) godot::api->godot_free(faction_cstr);

    // Cache game states
    if (instance_started) {
        bool cached_game_state = CNC_Get_Game_State(GAME_STATE_LAYERS, 0, (unsigned char*)game_state_cache, GAME_STATE_BUFFER_SIZE);
        if (cached_game_state == false)
            return false;

        bool cached_static_map_state = CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)static_map_state_cache, sizeof(CNCMapDataStruct));
        if (cached_static_map_state == false)
            return false;

        bool cached_player_state = CNC_Get_Game_State(GAME_STATE_PLAYER_INFO, 0, (unsigned char*)player_state_cache, sizeof(CNCPlayerInfoStruct) + 33);
        if (cached_player_state == false)
            return false;

        bool cached_palette = CNC_Get_Palette((unsigned char(&)[256][3])palette_cache);
        if (cached_palette == false)
            return false;

        return true;
    }

    return false;
}

bool GDNativeAlert::advance_instance(uint64 player_id) {
    bool instance_advanced = CNC_Advance_Instance(player_id);

    // Cache game states
    if (instance_advanced) {
        bool cached_game_state = CNC_Get_Game_State(GAME_STATE_LAYERS, 0, (unsigned char*)game_state_cache, GAME_STATE_BUFFER_SIZE);
        if (cached_game_state == false)
            return false;

        bool cached_static_map_state = CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)static_map_state_cache, sizeof(CNCMapDataStruct));
        if (cached_static_map_state == false)
            return false;

        bool cached_player_state = CNC_Get_Game_State(GAME_STATE_PLAYER_INFO, 0, (unsigned char*)player_state_cache, sizeof(CNCPlayerInfoStruct) + 33);
        if (cached_player_state == false)
            return false;

        bool cached_palette = CNC_Get_Palette((unsigned char(&)[256][3])palette_cache);
        if (cached_palette == false)
            return false;

        return true;
    }

    return false;
}

PoolByteArray GDNativeAlert::get_palette() {
    PoolByteArray palette_buffer;
    palette_buffer.resize(256 * 3);
    {
        PoolByteArray::Write palette_pba_write = palette_buffer.write();
        unsigned char* palette_pba_data = palette_pba_write.ptr();
        memcpy(palette_pba_data, palette_cache, 256 * 3);
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

Array GDNativeAlert::get_game_objects() {
    Array objects;

    for (int i = 0; i < game_state_cache->Count; i++) {
        CNCObjectStruct* object = game_state_cache->Objects + i;

        // We only care about selectable objects for now
        if (!object->IsSelectable)
            continue;

        Dictionary object_dict;

        object_dict["position"] = coord_to_pixel(object->CenterCoordX, object->CenterCoordY);
        object_dict["cell_position"] = Vector2(object->CellX - static_map_state_cache->OriginalMapCellX,
                                               object->CellY - static_map_state_cache->OriginalMapCellY);
        object_dict["size"] = Vector2(object->DimensionX, object->DimensionY);

        int top_left_x = object->CenterCoordX - (((object->DimensionX / PIXELS_PER_CELL) * LEPTONS_PER_CELL) / 2);
        int top_left_y = object->CenterCoordY - (((object->DimensionY / PIXELS_PER_CELL) * LEPTONS_PER_CELL) / 2);

        int top_left_cell_x = top_left_x / LEPTONS_PER_CELL;
        int top_left_cell_y = top_left_y / LEPTONS_PER_CELL;

        Array occupy_list;

        for (int i = 0; i < object->OccupyListLength; i++) {
            short cell_offset = object->OccupyList[i];
            int cell_offset_x = cell_offset % MAP_MAX_CELL_WIDTH;
            int cell_offset_y = cell_offset / MAP_MAX_CELL_WIDTH;
            int cell_new_x = (top_left_cell_x + cell_offset_x) - static_map_state_cache->OriginalMapCellX;
            int cell_new_y = (top_left_cell_y + cell_offset_y) - static_map_state_cache->OriginalMapCellY;
            occupy_list.push_front(Vector2(cell_new_x * PIXELS_PER_CELL, cell_new_y * PIXELS_PER_CELL));
        }

        object_dict["occupied_cells"] = occupy_list;

        if (object->Type == BUILDING || object->Type == BUILDING_TYPE) {
            object_dict["type"] = "BUILDING";
        }
        else {
            object_dict["type"] = "UNIT";
        }

        objects.push_front(object_dict);
    }

    return objects;
}

String GDNativeAlert::get_cursor_name(int x, int y) {
    CNCObjectStruct* nearest_object = get_nearest_object(x, y);

    if (player_state_cache->SelectedID == -1)
    {
        // Player doesn't have objects selected
        if (nearest_object != nullptr && nearest_object->IsSelectable)
        {
            return "MOUSE_CAN_SELECT";
        }

        return "MOUSE_NORMAL";
    }
    else {
        if (nearest_object != nullptr)
        {
            DllActionTypeEnum action = nearest_object->ActionWithSelected[player_state_cache->House];

            switch (action)
            {
            case DAT_ATTACK:
                return "MOUSE_CAN_ATTACK";
            case DAT_ATTACK_OUT_OF_RANGE:
                return "MOUSE_STAY_ATTACK";
            case DAT_SABOTAGE:
                return "MOUSE_DEMOLITIONS";
            case DAT_ENTER:
                return "MOUSE_ENTER";
            case DAT_SELECT:
                return "MOUSE_CAN_SELECT";
            case DAT_NOMOVE:
                return "MOUSE_NO_MOVE";
            }
        }

        unsigned int action_count = player_state_cache->ActionWithSelectedCount;

        DllActionTypeEnum* actions = new DllActionTypeEnum[action_count];
        actions = player_state_cache->ActionWithSelected;

        int map_cell_x = (x / 24);
        int map_cell_y = (y / 24);

        DllActionTypeEnum action = actions[map_cell_y * static_map_state_cache->OriginalMapCellWidth + map_cell_x];

        if (action == DAT_NOMOVE) return "MOUSE_NO_MOVE";

        return "MOUSE_CAN_MOVE";
    }

    return "MOUSE_NORMAL";
}

CNCObjectStruct* GDNativeAlert::get_nearest_object(int x, int y) {
    Vector2 coord = pixel_to_coord(x, y);
    unsigned short lepton_position_x = coord.x;
    unsigned short lepton_position_y = coord.y;

    CNCObjectStruct* nearest_object = nullptr;
    int nearest_object_distance;
    for (int i = 0; i < game_state_cache->Count; i++) {
        CNCObjectStruct* object = game_state_cache->Objects + i;

        if (object->Type == BUILDING || object->Type == BUILDING_TYPE) {
            unsigned short top_left_x = object->CenterCoordX - (((object->DimensionX / 24) * 256) / 2);
            unsigned short top_left_y = object->CenterCoordY - (((object->DimensionY / 24) * 256) / 2);

            unsigned short top_left_cell_x = top_left_x / 256;
            unsigned short top_left_cell_y = top_left_y / 256;

            for (int i = 0; i < object->OccupyListLength; i++) {
                short cell_offset = object->OccupyList[i];
                int cell_offset_x = cell_offset % 128;
                int cell_offset_y = cell_offset / 128;
                int cell_new_x = (top_left_cell_x + cell_offset_x);
                int cell_new_y = (top_left_cell_y + cell_offset_y);

                unsigned short cell_x = (cell_new_x * 256) + 128;
                unsigned short cell_y = (cell_new_y * 256) + 128;
                int distance = GDNativeAlert::coord_distance(Vector2(cell_x, cell_y), coord);
                if (distance <= 0xC0) {
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
            int distance = GDNativeAlert::coord_distance(Vector2(object->CenterCoordX, object->CenterCoordY), coord);
            if (distance <= 0xC0) {
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
