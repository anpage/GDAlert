#ifndef CNCDLL_H
#define CNCDLL_H


#include <stdlib.h>

#include <DLLInterface.h>

typedef void(__cdecl* CNC_Event_Callback_Type)(const EventCallbackStruct& event);
typedef unsigned __int64 uint64;
typedef __int64 int64;

/*
**  DLL Interface
**
**
**
**
**
*/
extern "C" __declspec(dllexport) unsigned int __cdecl CNC_Version(unsigned int version_in);
extern "C" __declspec(dllexport) void __cdecl CNC_Init(const char* command_line, CNC_Event_Callback_Type event_callback);
extern "C" __declspec(dllexport) void __cdecl CNC_Config(const CNCRulesDataStruct & rules);
extern "C" __declspec(dllexport) void __cdecl CNC_Add_Mod_Path(const char* mod_path);
extern "C" __declspec(dllexport) bool __cdecl CNC_Get_Visible_Page(unsigned char* buffer_in, unsigned int& width, unsigned int& height);
extern "C" __declspec(dllexport) bool __cdecl CNC_Get_Palette(unsigned char(&palette_in)[256][3]);
extern "C" __declspec(dllexport) bool __cdecl CNC_Start_Instance(int scenario_index, int build_level, const char* faction, const char* game_type, const char* content_directory, int sabotaged_structure, const char* override_map_name);
extern "C" __declspec(dllexport) bool __cdecl CNC_Start_Instance_Variation(int scenario_index, int scenario_variation, int scenario_direction, int build_level, const char* faction, const char* game_type, const char* content_directory, int sabotaged_structure, const char* override_map_name);
extern "C" __declspec(dllexport) bool __cdecl CNC_Start_Custom_Instance(const char* content_directory, const char* directory_path, const char* scenario_name, int build_level, bool multiplayer);
extern "C" __declspec(dllexport) bool __cdecl CNC_Advance_Instance(uint64 player_id);
extern "C" __declspec(dllexport) bool __cdecl CNC_Get_Game_State(GameStateRequestEnum state_type, uint64 player_id, unsigned char* buffer_in, unsigned int buffer_size);
extern "C" __declspec(dllexport) bool __cdecl CNC_Read_INI(int scenario_index, int scenario_variation, int scenario_direction, const char* content_directory, const char* override_map_name, char* ini_buffer, int _ini_buffer_size);
extern "C" __declspec(dllexport) void __cdecl CNC_Set_Home_Cell(int x, int y, uint64 player_id);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Game_Request(GameRequestEnum request_type);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Game_Settings_Request(int health_bar_display_mode, int resource_bar_display_mode);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Input(InputRequestEnum mouse_event, unsigned char special_key_flags, uint64 player_id, int x1, int y1, int x2, int y2);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Structure_Request(StructureRequestEnum request_type, uint64 player_id, int object_id);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Unit_Request(UnitRequestEnum request_type, uint64 player_id);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Sidebar_Request(SidebarRequestEnum request_type, uint64 player_id, int buildable_type, int buildable_id, short cell_x, short cell_y);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_SuperWeapon_Request(SuperWeaponRequestEnum request_type, uint64 player_id, int buildable_type, int buildable_id, int x1, int y1);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_ControlGroup_Request(ControlGroupRequestEnum request_type, uint64 player_id, unsigned char control_group_index);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Debug_Request(DebugRequestEnum debug_request_type, uint64 player_id, const char* object_name, int x, int y, bool unshroud, bool enemy);
extern "C" __declspec(dllexport) bool __cdecl CNC_Set_Multiplayer_Data(int scenario_index, CNCMultiplayerOptionsStruct & game_options, int num_players, CNCPlayerInfoStruct * player_list, int max_players);
extern "C" __declspec(dllexport) bool __cdecl CNC_Clear_Object_Selection(uint64 player_id);
extern "C" __declspec(dllexport) bool __cdecl CNC_Select_Object(uint64 player_id, int object_type_id, int object_to_select_id);
extern "C" __declspec(dllexport) bool __cdecl CNC_Save_Load(bool save, const char* file_path_and_name, const char* game_type);
extern "C" __declspec(dllexport) void __cdecl CNC_Set_Difficulty(int difficulty);
extern "C" __declspec(dllexport) void __cdecl CNC_Restore_Carryover_Objects(const CarryoverObjectStruct * objects);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Player_Switch_To_AI(uint64 player_id);
extern "C" __declspec(dllexport) void __cdecl CNC_Handle_Human_Team_Wins(uint64 player_id);
extern "C" __declspec(dllexport) void __cdecl CNC_Start_Mission_Timer(int time);

#endif
