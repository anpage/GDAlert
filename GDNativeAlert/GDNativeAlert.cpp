#include <gdnative_api_struct.gen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include <WinUser.h>

#include "CNCDll.h"

#define GAME_BUFFER_SIZE 0xFFFFFFF // 255 MB, yay overkill!

typedef struct user_data_struct {
	char data[256];
    godot_pool_byte_array pba;
    unsigned char game_buffer[GAME_BUFFER_SIZE];
    unsigned int game_buffer_width;
    unsigned int game_buffer_height;
    bool first_frame = true;
} user_data_struct;

godot_variant self;

const godot_gdnative_core_api_struct* api = NULL;
const godot_gdnative_ext_nativescript_api_struct* nativescript_api = NULL;

GDCALLINGCONV void* simple_constructor(godot_object* p_instance, void* p_method_data);
GDCALLINGCONV void simple_destructor(godot_object* p_instance, void* p_method_data, void* p_user_data);
godot_variant simple_get_data(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_register_class(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_cnc_init(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_cnc_start_instance(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_cnc_advance_instance(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_cnc_get_visible_page(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_cnc_get_visible_page_width(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);
godot_variant gdnative_alert_cnc_get_visible_page_height(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args);

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* p_options) {
	api = p_options->api_struct;

	// now find our extensions
	for (unsigned int i = 0; i < api->num_extensions; i++) {
		switch (api->extensions[i]->type) {
		case GDNATIVE_EXT_NATIVESCRIPT: {
			nativescript_api = (godot_gdnative_ext_nativescript_api_struct*)api->extensions[i];
		}; break;
		default: break;
		};
	};
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* p_options) {
	api = NULL;
	nativescript_api = NULL;
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* p_handle) {
	godot_instance_create_func create = { NULL, NULL, NULL };
	create.create_func = &simple_constructor;

	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &simple_destructor;

	nativescript_api->godot_nativescript_register_class(p_handle, "RedAlert", "Reference", create, destroy);

  godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

	godot_instance_method get_data = { NULL, NULL, NULL };
	get_data.method = &simple_get_data;

	nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "get_data", attributes, get_data);

  godot_instance_method register_class = { NULL, NULL, NULL };
  register_class.method = &gdnative_alert_register_class;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "register_class", attributes, register_class);

  godot_instance_method cnc_init = { NULL, NULL, NULL };
  cnc_init.method = &gdnative_alert_cnc_init;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "cnc_init", attributes, cnc_init);

  godot_instance_method cnc_start_instance = { NULL, NULL, NULL };
  cnc_start_instance.method = &gdnative_alert_cnc_start_instance;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "cnc_start_instance", attributes, cnc_start_instance);

  godot_instance_method cnc_advance_instance = { NULL, NULL, NULL };
  cnc_advance_instance.method = &gdnative_alert_cnc_advance_instance;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "cnc_advance_instance", attributes, cnc_advance_instance);

  godot_instance_method cnc_get_visible_page = { NULL, NULL, NULL };
  cnc_get_visible_page.method = &gdnative_alert_cnc_get_visible_page;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "cnc_get_visible_page", attributes, cnc_get_visible_page);

  godot_instance_method cnc_get_visible_page_width = { NULL, NULL, NULL };
  cnc_get_visible_page_width.method = &gdnative_alert_cnc_get_visible_page_width;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "cnc_get_visible_page_width", attributes, cnc_get_visible_page_width);

  godot_instance_method cnc_get_visible_page_height = { NULL, NULL, NULL };
  cnc_get_visible_page_height.method = &gdnative_alert_cnc_get_visible_page_height;

  nativescript_api->godot_nativescript_register_method(p_handle, "RedAlert", "cnc_get_visible_page_height", attributes, cnc_get_visible_page_height);
}

GDCALLINGCONV void* simple_constructor(godot_object* p_instance, void* p_method_data) {
	printf("RedAlert._init()\n");

	user_data_struct* user_data = (user_data_struct * )api->godot_alloc(sizeof(user_data_struct));
	strcpy(user_data->data, "World from GDNative!");

	return user_data;
}

GDCALLINGCONV void simple_destructor(godot_object* p_instance, void* p_method_data, void* p_user_data) {
	printf("RedAlert._byebye()\n");

	api->godot_free(p_user_data);
}

godot_variant simple_get_data(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args) {
	godot_string data;
	godot_variant ret;
	user_data_struct* user_data = (user_data_struct*)p_user_data;

	api->godot_string_new(&data);
	api->godot_string_parse_utf8(&data, user_data->data);
	api->godot_variant_new_string(&ret, &data);
	api->godot_string_destroy(&data);

	return ret;
}

void cnc_event_callback(const EventCallbackStruct& event)
{
  godot_variant arg;

  char msg[256];

  switch (event.EventType)
  {
  case (CALLBACK_EVENT_INVALID):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_INVALID\n");
    break;
  case (CALLBACK_EVENT_SOUND_EFFECT):
    sprintf(msg, "Sound effect: %s\n", event.SoundEffect.SoundEffectName);
    break;
  case (CALLBACK_EVENT_SPEECH):
    sprintf(msg, "Speech: %s\n", event.Speech.SpeechName);
    break;
  case (CALLBACK_EVENT_GAME_OVER):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_GAME_OVER\n");
    break;
  case (CALLBACK_EVENT_DEBUG_PRINT):
    sprintf(msg, "DEBUG PRINT: %s\n", event.DebugPrint.PrintString);
    break;
  case (CALLBACK_EVENT_MOVIE):
    sprintf(msg, "Play movie: %s\n", event.Movie.MovieName);
    break;
  case (CALLBACK_EVENT_MESSAGE):
    sprintf(msg, "Incoming message: %s\n", event.Message.Message);
    break;
  case (CALLBACK_EVENT_UPDATE_MAP_CELL):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_UPDATE_MAP_CELL\n");
    break;
  case (CALLBACK_EVENT_ACHIEVEMENT):
    sprintf(msg, "Achievement unlocked: %s, %s\n", event.Achievement.AchievementType, event.Achievement.AchievementReason);
    break;
  case (CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS\n");
    break;
  case (CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING\n");
    break;
  case (CALLBACK_EVENT_BRIEFING_SCREEN):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_BRIEFING_SCREEN\n");
    break;
  case (CALLBACK_EVENT_CENTER_CAMERA):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_CENTER_CAMERA\n");
    break;
  case (CALLBACK_EVENT_PING):
    sprintf(msg, "Unhandled event: CALLBACK_EVENT_PING\n");
    break;
  default:
    sprintf(msg, "Unknown event type: %i\n", event.EventType);
    break;
  }

  auto gdmsg = api->godot_string_chars_to_utf8(msg);
  api->godot_variant_new_string(&arg, &gdmsg);
  const godot_variant* args[] = { &arg };

  auto cb_name = api->godot_string_chars_to_utf8("debug_message");
  godot_variant_call_error err;
  api->godot_variant_call(&self, &cb_name, args, sizeof args / sizeof * args, &err);
}

godot_variant gdnative_alert_register_class(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
  godot_variant ret;
  api->godot_variant_new_int(&ret, 0);
  //Cache object that can be used to call our method in GDScript
  api->godot_variant_new_copy(&self, p_args[0]);
  return ret;
}

godot_variant gdnative_alert_cnc_init(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
  godot_variant ret;
  api->godot_variant_new_int(&ret, 0);

  char buff[256];
  _getcwd(buff, 256);
  printf(buff);

  godot_string cmdline = api->godot_variant_as_string(p_args[0]);
  godot_char_string char_cmdline = api->godot_string_ascii(&cmdline);
  const char* c_cmdline = api->godot_char_string_get_data(&char_cmdline);
  CNC_Init(c_cmdline, &cnc_event_callback);
  ShowCursor(TRUE);

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

  api->godot_free(&cmdline);
  api->godot_free(&char_cmdline);
  return ret;
}

godot_variant gdnative_alert_cnc_start_instance(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
    godot_variant ret;
    api->godot_variant_new_int(&ret, 0);
    CNC_Start_Instance(1, 10, "ALLY", "GAME_NORMAL", "", NULL, NULL);
    return ret;
}

godot_variant gdnative_alert_cnc_advance_instance(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
    godot_variant ret;
    api->godot_variant_new_int(&ret, 0);
    CNC_Advance_Instance(0);
    return ret;
}

godot_variant gdnative_alert_cnc_get_visible_page(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
    godot_variant ret;
    user_data_struct* user_data = (user_data_struct*)p_user_data;
    if (user_data->first_frame == true)
    {
        api->godot_pool_byte_array_new(&user_data->pba);
        user_data->first_frame = false;
    }

    // Get palette
    unsigned char palette[256][3];
    CNC_Get_Palette(palette);

    // Get visible page
    unsigned int width = 0;
    unsigned int height = 0;
    CNC_Get_Visible_Page(user_data->game_buffer, width, height);
    if (width != user_data->game_buffer_width || height != user_data->game_buffer_height)
    {
        user_data->game_buffer_width = width;
        user_data->game_buffer_height = height;
        api->godot_pool_byte_array_resize(&user_data->pba, width*height*4);
    }

    // Get pointer to byte array data
    godot_pool_byte_array_write_access* pba_write_access = api->godot_pool_byte_array_write(&user_data->pba);
    unsigned char* pba_data = api->godot_pool_byte_array_write_access_ptr(pba_write_access);

    // Convert visible page to FORMAT_RGBA8
    for (unsigned int pixel = 0; pixel < width * height; pixel++)
    {
        unsigned int pixel_out = pixel * 4;
        for (unsigned int component = 0; component <= 2; component++)
        {

            // R, G, and B
            pba_data[pixel_out + component] = palette[user_data->game_buffer[pixel]][component] << 2;
        }
        // A
        pba_data[pixel_out + 3] = 0xFF;
    }

    // Return byte array as variant
    api->godot_variant_new_pool_byte_array(&ret, &user_data->pba);
    return ret;
}

godot_variant gdnative_alert_cnc_get_visible_page_width(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
    godot_variant ret;
    user_data_struct* user_data = (user_data_struct*)p_user_data;
    api->godot_variant_new_int(&ret, user_data->game_buffer_width);
    return ret;
}

godot_variant gdnative_alert_cnc_get_visible_page_height(godot_object* p_instance, void* p_method_data, void* p_user_data, int p_num_args, godot_variant** p_args)
{
    godot_variant ret;
    user_data_struct* user_data = (user_data_struct*)p_user_data;
    api->godot_variant_new_int(&ret, user_data->game_buffer_height);
    return ret;
}
