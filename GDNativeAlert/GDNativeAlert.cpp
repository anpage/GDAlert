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

#include "mixfile.h"
#include "pk.h"
#include "gamefile.h"
#include "ini.h"
#include "audio.h"
#include "adpcm.h"

using namespace godot;

void GDNativeAlert::_register_methods() {
    register_method("start_instance", &GDNativeAlert::start_instance);
    register_method("advance_instance", &GDNativeAlert::advance_instance);
    register_method("get_palette", &GDNativeAlert::get_palette);
    register_method("get_visible_page", &GDNativeAlert::get_visible_page);
    register_method("get_visible_page_width", &GDNativeAlert::get_visible_page_width);
    register_method("get_visible_page_height", &GDNativeAlert::get_visible_page_height);
    register_method("handle_left_mouse_up", &GDNativeAlert::handle_left_mouse_up);
    register_method("handle_right_mouse_up", &GDNativeAlert::handle_right_mouse_up);
    register_method("handle_right_mouse_down", &GDNativeAlert::handle_right_mouse_down);
    register_method("handle_mouse_area", &GDNativeAlert::handle_mouse_area);
    register_method("get_score_sample", &GDNativeAlert::get_score_sample);
    register_method("handle_mouse_motion", &GDNativeAlert::handle_mouse_motion);
    register_method("get_cursor_name", &GDNativeAlert::get_cursor_name);
    register_method("get_game_objects", &GDNativeAlert::get_game_objects);

    register_signal<GDNativeAlert>("event", "message", GODOT_VARIANT_TYPE_STRING);
    register_signal<GDNativeAlert>("play_sound", "sample", GODOT_VARIANT_TYPE_OBJECT, "x", GODOT_VARIANT_TYPE_INT, "y", GODOT_VARIANT_TYPE_INT);
    register_signal<GDNativeAlert>("play_speech", "sample", GODOT_VARIANT_TYPE_OBJECT);
}

GDNativeAlert::GDNativeAlert() {
}

GDNativeAlert::~GDNativeAlert() {
}

void GDNativeAlert::_init() {
    game_palette_pba.resize(256 * 3);

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
    GameMixFile* local_mix = new GameMixFile("LOCAL.MIX", &fastKey);
    GameMixFile* speech_mix = new GameMixFile("SPEECH.MIX", &fastKey);
    GameMixFile::Cache("SPEECH.MIX");

    GameMixFile* main_mix = new GameMixFile("RedAlert\\MAIN.MIX", &fastKey);
    GameMixFile* sounds_mix = new GameMixFile("SOUNDS.MIX", &fastKey);
    GameMixFile::Cache("SOUNDS.MIX");
    GameMixFile* allies_mix = new GameMixFile("ALLIES.MIX", &fastKey);
    GameMixFile::Cache("ALLIES.MIX");
    GameMixFile* russian_mix = new GameMixFile("RUSSIAN.MIX", &fastKey);
    GameMixFile::Cache("RUSSIAN.MIX");

    GameMixFile* scores_mix = new GameMixFile("SCORES.MIX", &fastKey);
    GameMixFile::Cache("SCORES.MIX");

    GameMixFile* hires_mix = new GameMixFile("HIRES.MIX", &fastKey);
    GameMixFile::Cache("HIRES.MIX");

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

AudioStreamSample* GDNativeAlert::decode_aud(String name) {
    char* filename;
    if (name.find(".") == -1) {
        filename = (name + ".AUD").alloc_c_string();
    } else {
        filename = name.alloc_c_string();
    }

    unsigned char* aud_data = (unsigned char*)GameMixFile::Retrieve(filename);
    if (filename != nullptr) godot::api->godot_free(filename);

    if (aud_data == nullptr) return nullptr;

    ADPCMStreamType stream_info;

    // Read in the file's header.
    AUDHeaderStruct raw_header;
    memcpy(&raw_header, aud_data, sizeof(raw_header));
    aud_data += sizeof(raw_header);

    // Compression is ADPCM so we need to init its stream info.
    if (raw_header.m_Compression == COMP_ADPCM) {
        stream_info.m_Channels = (raw_header.m_Flags & 1) + 1;
        stream_info.m_BitsPerSample = raw_header.m_Flags & 2 ? 16 : 8;
        stream_info.m_CompSize = raw_header.m_Size;
        stream_info.m_UnCompSize = raw_header.m_Size * (stream_info.m_BitsPerSample / 4);
        ADPCM_Stream_Init(&stream_info);
    }
    else
    {
        return nullptr;
    }

    unsigned char* dechunk_pba_data = new unsigned char[stream_info.m_CompSize];
    unsigned char* pba_data = new unsigned char[stream_info.m_UnCompSize];

    void* aud_data_buf = (void*)aud_data;
    int out_size = Sample_Copy(&stream_info, &aud_data_buf, &stream_info.m_CompSize, pba_data, stream_info.m_UnCompSize, dechunk_pba_data);

    PoolByteArray sound_buffer;
    sound_buffer.resize(out_size);
    PoolByteArray::Write sound_buffer_write = sound_buffer.write();
    unsigned char* sound_buffer_data = sound_buffer_write.ptr();

    memcpy(sound_buffer_data, pba_data, out_size);

    delete[] dechunk_pba_data;
    delete[] pba_data;

    if (out_size)
    {
        AudioStreamSample* sample = AudioStreamSample::_new();
        if (stream_info.m_BitsPerSample == 16)
        {
            sample->set_format(AudioStreamSample::FORMAT_16_BITS);
        }
        if (stream_info.m_Channels == 2)
        {
            sample->set_stereo(true);
        }
        sample->set_mix_rate(raw_header.m_Rate);
        sample->set_data(sound_buffer);

        return sample;
    }

    return nullptr;
}

AudioStreamSample* GDNativeAlert::get_score_sample(String name) {
    return decode_aud(name);
}

void GDNativeAlert::play_sound(String name, bool is_speech, int x = 0, int y = 0) {
    AudioStreamSample* sample = decode_aud(name);
    if (is_speech || x < 1 || y < 1)
    {
        emit_signal("play_speech", sample);
    }
    else
    {
        emit_signal("play_sound", sample, x, y);
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
    game_buffer_width = 3072;
    game_buffer_height = 3072;

    char* faction_cstr = faction.alloc_c_string();
    return CNC_Start_Instance(scenario_index, build_level, faction_cstr, "GAME_NORMAL", "", NULL, NULL);
    if (faction_cstr != nullptr) godot::api->godot_free(faction_cstr);
}

bool GDNativeAlert::advance_instance() {
    return CNC_Advance_Instance(0);
}

PoolByteArray GDNativeAlert::get_palette() {
    unsigned char palette[256][3];
    CNC_Get_Palette((unsigned char(&)[256][3])palette);
    // Get palette
    {
        PoolByteArray::Write palette_pba_write = game_palette_pba.write();
        unsigned char* palette_pba_data = palette_pba_write.ptr();
        memcpy(palette_pba_data, palette, 256 * 3);
    }
    return game_palette_pba;
}

PoolByteArray GDNativeAlert::get_visible_page() {
    // Get visible page
    unsigned int width = 0;
    unsigned int height = 0;

    // Get pointer to byte array data
    {
        PoolByteArray::Write pba_write = game_buffer_pba.write();
        unsigned char* pba_data = pba_write.ptr();
        CNC_Get_Visible_Page(pba_data, width, height);
    }

    if (width != game_buffer_width || height != game_buffer_height)
    {
        game_buffer_width = width;
        game_buffer_height = height;
        game_buffer_pba.resize(width * height);
    }

    return game_buffer_pba;
}

unsigned int GDNativeAlert::get_visible_page_width() {
    return game_buffer_width;
}

unsigned int GDNativeAlert::get_visible_page_height() {
    return game_buffer_height;
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
    int map_lepton = map_cell * 128;
    in -= map_lepton;
    return ((float)in / 128.0) * 24.0;
}

Array GDNativeAlert::get_game_objects() {
    unsigned char* layer_state = new unsigned char[0x100000];
    bool got_layers = CNC_Get_Game_State(GAME_STATE_LAYERS, 0, layer_state, 0x10000);
    CNCObjectListStruct* layers = (CNCObjectListStruct*)layer_state;

    CNCMapDataStruct* map_state = new CNCMapDataStruct;
    CNC_Get_Game_State(GAME_STATE_STATIC_MAP, 0, (unsigned char*)map_state, sizeof(CNCMapDataStruct));

    Array objects;

    for (int i = 0; i < layers->Count; i++) {
        CNCObjectStruct* object = layers->Objects + i;

        Dictionary object_dict;

        object_dict["PositionX"] = object->PositionX;
        object_dict["PositionY"] = object->PositionY;

        object_dict["CenterCoordX"] = lepton_to_pixel(map_state->OriginalMapCellX, object->CenterCoordX / 2);
        object_dict["CenterCoordY"] = lepton_to_pixel(map_state->OriginalMapCellY, object->CenterCoordY / 2);

        object_dict["DimensionX"] = object->DimensionX;
        object_dict["DimensionY"] = object->DimensionY;

        objects.push_front(object_dict);
    }

    delete[] layer_state;
    delete map_state;

    return objects;
}

CNCObjectStruct* GDNativeAlert::get_nearest_object(CNCObjectListStruct* layers, real_t x, real_t y) {
    Vector2 lepton_position = Vector2((x / 24) * 256, (y / 24) * 256);

    CNCObjectStruct* nearest_object = nullptr;
    for (int i = 0; i < layers->Count; i++) {
        CNCObjectStruct* object = layers->Objects + i;
        if (!object->IsSelectable) continue;

        Vector2 object_position = Vector2(object->CenterCoordX, object->CenterCoordY);
        real_t distance = object_position.distance_to(lepton_position);
        if (distance < 192.0) nearest_object = object;
    }

    if (nearest_object == nullptr) {
        return nullptr;
    }

    return nearest_object;
}
