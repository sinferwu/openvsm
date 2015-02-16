/**
 *
 * @file   lua_bind.c
 * @Author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @brief  Brief description of file.
 *
 * This file is part of OpenVSM.
 * OpenVSM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenVSM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenVSM.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <vsm_api.h>

static int lua_state_to_string ( lua_State* L );
static int lua_set_pin_state ( lua_State* L );
static int lua_set_pin_bool ( lua_State* L );
static int lua_get_pin_bool ( lua_State* L );
static int lua_get_pin_state ( lua_State* L );
static int lua_is_pin_active ( lua_State* L );
static int lua_is_pin_edge ( lua_State* L );
static int lua_is_pin_posedge ( lua_State* L );
static int lua_is_pin_negedge ( lua_State* L );
static int lua_is_pin_low ( lua_State* L );
static int lua_is_pin_high ( lua_State* L );
static int lua_is_pin_floating ( lua_State* L );
static int lua_toggle_pin_state ( lua_State* L );
static int lua_out_log ( lua_State* L );
static int lua_out_message ( lua_State* L );
static int lua_out_warning ( lua_State* L );
static int lua_out_error ( lua_State* L );
static int lua_set_callback ( lua_State* L );
static int lua_create_debug_popup ( lua_State* L );
static int lua_print_to_debug_popup ( lua_State* L );
static int lua_dump_to_debug_popup ( lua_State* L );
static int lua_create_memory_popup ( lua_State* L );
static int lua_create_source_popup ( lua_State* L );
static int lua_create_status_popup ( lua_State* L );
static int lua_create_var_popup ( lua_State* L );
static int lua_delete_popup ( lua_State* L );
static int lua_set_memory_popup ( lua_State* L );
static int lua_repaint_memory_popup ( lua_State* L );
static int lua_get_string_param ( lua_State* L );
static int lua_get_bool_param ( lua_State* L );
static int lua_get_num_param ( lua_State* L );
static int lua_get_hex_param ( lua_State* L );
static int lua_get_init_param ( lua_State* L );
static int lua_add_source_file ( lua_State* L );

static int lua_get_bit ( lua_State* L );
static int lua_set_bit ( lua_State* L );
static int lua_toggle_bit ( lua_State* L );
static int lua_clear_bit ( lua_State* L );

static int lua_get_systime ( lua_State* L );

static const lua_bind_var lua_var_api_list[]=
{
	{.var_name="SHI", .var_value=SHI},
	{.var_name="SLO", .var_value=SLO},
	{.var_name="FLT", .var_value=FLT},
	{.var_name="PLO", .var_value=PLO},
	{.var_name="ILO", .var_value=ILO},
	{.var_name="WLO", .var_value=WLO},
	{.var_name="WHI", .var_value=WHI},
	{.var_name="IHI", .var_value=IHI},
	{.var_name="PHI", .var_value=PHI},
	{.var_name="WUD", .var_value=WUD},
	{.var_name="SUD", .var_value=SUD},
	{.var_name="TSTATE", .var_value=TSTATE},
	{.var_name="FSTATE", .var_value=FSTATE},
	{.var_name="UNDEFINED", .var_value=UNDEFINED},
	{.var_name="MSEC", .var_value=1000000000L},
	{.var_name="NSEC", .var_value=100000000L},
	{.var_name="SEC", .var_value=1000000000000L},
	{.var_name="NOW", .var_value=0L},
	{.var_name=NULL},
};

static const lua_bind_func lua_c_api_list[] =
{
	{.lua_func_name="state_to_string", .lua_c_api=&lua_state_to_string},
	{.lua_func_name="is_pin_active", .lua_c_api=&lua_is_pin_active},
	{.lua_func_name="is_pin_edge", .lua_c_api=&lua_is_pin_edge},
	{.lua_func_name="is_pin_posedge", .lua_c_api=&lua_is_pin_posedge},
	{.lua_func_name="is_pin_negedge", .lua_c_api=&lua_is_pin_negedge},
	{.lua_func_name="set_pin_state", .lua_c_api=&lua_set_pin_state},
	{.lua_func_name="set_pin_bool", .lua_c_api=&lua_set_pin_bool},
	{.lua_func_name="get_pin_bool", .lua_c_api=&lua_get_pin_bool},
	{.lua_func_name="get_pin_state", .lua_c_api=&lua_get_pin_state},
	{.lua_func_name="is_pin_low", .lua_c_api=&lua_is_pin_low},
	{.lua_func_name="is_pin_high", .lua_c_api=&lua_is_pin_high},
	{.lua_func_name="is_pin_floating", .lua_c_api=&lua_is_pin_floating},
	{.lua_func_name="toggle_pin_state", .lua_c_api=&lua_toggle_pin_state},
	{.lua_func_name="out_log", .lua_c_api=&lua_out_log},
	{.lua_func_name="out_message", .lua_c_api=&lua_out_message},
	{.lua_func_name="out_warning", .lua_c_api=&lua_out_warning},
	{.lua_func_name="out_error", .lua_c_api=&lua_out_error},
	{.lua_func_name="set_callback", .lua_c_api=&lua_set_callback},
	{.lua_func_name="create_debug_popup", .lua_c_api=&lua_create_debug_popup},
	{.lua_func_name="create_memory_popup", .lua_c_api=&lua_create_memory_popup},
	{.lua_func_name="create_source_popup", .lua_c_api=&lua_create_source_popup},
	{.lua_func_name="create_status_popup", .lua_c_api=&lua_create_status_popup},
	{.lua_func_name="create_var_popup", .lua_c_api=&lua_create_var_popup},
	{.lua_func_name="delete_popup", .lua_c_api=&lua_delete_popup},
	{.lua_func_name="set_memory_popup", .lua_c_api=&lua_set_memory_popup},
	{.lua_func_name="repaint_memory_popup", .lua_c_api=&lua_repaint_memory_popup},
	{.lua_func_name="print_to_debug_popup", .lua_c_api=&lua_print_to_debug_popup},
	{.lua_func_name="dump_to_debug_popup", .lua_c_api=&lua_dump_to_debug_popup},
	{.lua_func_name="get_string_param", .lua_c_api=&lua_get_string_param},
	{.lua_func_name="get_num_param", .lua_c_api=&lua_get_num_param},
	{.lua_func_name="get_bool_param", .lua_c_api=&lua_get_bool_param},
	{.lua_func_name="get_init_param", .lua_c_api=&lua_get_init_param},
	{.lua_func_name="get_hex_param", .lua_c_api=&lua_get_hex_param},
	{.lua_func_name="add_source_file", .lua_c_api=&lua_add_source_file},
	{.lua_func_name="get_bit", .lua_c_api=&lua_get_bit},
	{.lua_func_name="set_bit", .lua_c_api=&lua_set_bit},
	{.lua_func_name="clear_bit", .lua_c_api=&lua_clear_bit},
	{.lua_func_name="toggle_bit", .lua_c_api=&lua_toggle_bit},
	{.lua_func_name="systime", .lua_c_api=&lua_get_systime},
	{ NULL, NULL},
};

#define STACK_CHECK(NUM) (lua_check_args(model,model->luactx, NUM, __FILE__, __PRETTY_FUNCTION__, __LINE__ - 2 ))

static void
lua_check_args (IDSIMMODEL* model, lua_State* L, int expected, const char *filename, const char *function_name, int line)
{
	lua_Number argnum = lua_gettop ( L );
	if(expected > argnum)
	{
		out_error ( model, "Too few arguments for function %s, %s:%d\nExpected %d, got %.0f", function_name, filename, line, expected, argnum );
	}
}

void
register_functions ( IDSIMMODEL* model, lua_State* L )
{
	/*  Declare functions */
	for ( int32_t i=0; lua_c_api_list[i].lua_func_name; i++ )
	{
		lua_pushcfunction ( L, lua_c_api_list[i].lua_c_api );
		lua_setglobal ( L, lua_c_api_list[i].lua_func_name );
	}
	/* Declare variables */
	for ( int32_t i=0; lua_var_api_list[i].var_name ; i++ )
	{
		lua_pushinteger ( L, lua_var_api_list[i].var_value );
		lua_setglobal ( L, lua_var_api_list[i].var_name );
	}
}

bool
load_device_script ( IDSIMMODEL* model, const char* device_name )
{
	char spath[512] = {0};
	if ( 0 == GetEnvironmentVariable ( "LUAVSM", spath, sizeof spath ) )
	{
		out_error ( model, "LUAVSM environment variable is not set" );
		return false;
	}
	char *script=NULL;
	asprintf ( &script, "%s%s%s", spath, '\\' == spath[strlen(spath)-1]? "":"\\", device_name );

	int32_t lua_err = luaL_loadfile ( model->luactx, script );
	free(script);
	if ( 0 != lua_err )
	{
		const char* mess = NULL;
		switch ( lua_err )
		{
			case LUA_ERRSYNTAX:
				mess = lua_tostring(model->luactx, -1);
				out_error ( model, "Syntax error in Lua script\n%s", mess );
				return false;
			case LUA_ERRMEM:
				out_error ( model, "Not enough memory to load script" );
				return false;
			case LUA_ERRFILE:
				out_error ( model, "Error loading script file" );
				return false;
			default:
				out_error ( model, "Unknown error, shouldn't happen" );
				assert ( 0 );
		}
	}
	/* Primer run, if not run it - nothing works, need for parse */
	if ( 0 != lua_pcall ( model->luactx, 0, 0, 0 ) )
	{
		out_error ( model, "Failed to load the script" );
		return false;
	}
	return true;
}

static int
lua_get_string_param ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	lua_pushstring ( L, get_string_param ( model, ( char* ) lua_tostring ( L, -1 ) ) );
	return 1;
}

static int
lua_get_bool_param ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	lua_pushboolean ( L, get_bool_param ( model, ( char* ) lua_tostring ( L, -1 ) ) );
	return 1;
}

static int
lua_get_num_param ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	lua_pushnumber ( L, get_num_param ( model, ( char* ) lua_tostring ( L, -1 ) ) );
	return 1;
}

static int
lua_get_hex_param ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	lua_pushinteger ( L, get_hex_param ( model, ( char* ) lua_tostring ( L, -1 ) ) );
	return 1;
}

static int
lua_get_init_param ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	lua_pushinteger ( L, get_init_param ( model, ( char* ) lua_tostring ( L, -1 ) ) );
	return 1;
}

static int
lua_delete_popup ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int id = lua_tonumber ( L, -1 );
	delete_popup ( model, id );
	return 0;
}

static int
lua_create_debug_popup ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	lua_pushlightuserdata ( L, create_debug_popup ( model, text, ++popup_id ) );
	lua_pushinteger ( L, popup_id );
	return 2;
}

/**
* Prints a text string to debug popup
* @param L Lua state
* @return a pointer to popup and its ID
*/
static int
lua_print_to_debug_popup ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -3 );
	STACK_CHECK(3);
	/**
	* Popup pointer and text of the message are in the stack
	*/
	print_to_debug_popup ( lua_touserdata ( L, -2 ), lua_tostring ( L, -1 ) );
	return 0;
}

/**
* Prints a text string to debug popup
* @param L Lua state
* @return a pointer to popup and its ID
*/
static int
lua_dump_to_debug_popup ( lua_State* L )
{

	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -5 );
	STACK_CHECK(5);
	lua_Number offset = luaL_checknumber ( L,-1 );
	lua_Number size = luaL_checknumber ( L,-2 );
	const char* buf = luaL_checkstring ( L,-3 );
	dump_to_debug_popup ( lua_touserdata ( L, -4 ), ( BYTE* ) buf, offset, size );
	return 0;
}

static int
lua_create_source_popup ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	lua_pushlightuserdata ( L, create_source_popup ( model, text, ++popup_id ) );
	lua_pushinteger ( L, popup_id );
	return 2;
}
static int
lua_create_status_popup ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	lua_pushlightuserdata ( L, create_status_popup ( model, text, ++popup_id ) );
	lua_pushinteger ( L, popup_id );
	return 2;
}
static int
lua_create_var_popup ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	lua_pushlightuserdata ( L, create_var_popup ( model, text, ++popup_id ) );
	lua_pushinteger ( L, popup_id );
	return 2;
}

static int
lua_create_memory_popup ( lua_State* L )
{
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	lua_pushlightuserdata ( L, create_memory_popup ( model, text, ++popup_id ) );
	lua_pushinteger ( L, popup_id );
	return 2;
}

static int
lua_set_memory_popup ( lua_State* L )
{

	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -4 );
	STACK_CHECK(4);
	lua_Number size = luaL_checknumber ( L,-1 );
	const char* buf = luaL_checkstring ( L,-2 );

	set_memory_popup ( lua_touserdata ( L, -3 ), 0, ( void* ) buf, size );

	return 0;
}

static int
lua_add_source_file ( lua_State* L )
{

	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -4 );
	STACK_CHECK(4);

	if ( false == add_source_file ( lua_touserdata ( L, -3 ), ( char* ) lua_tostring ( L, -2 ), lua_tonumber ( L, -1 ) ) )
	{
		out_log ( model, "Fail to load source file" );
	}

	return 0;
}

static int
lua_repaint_memory_popup ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	if ( 0 == lua_isuserdata ( L, -1 ) )
	{
		out_error ( model, "Bad argument" );
		return 0;
	}

	repaint_memory_popup ( lua_touserdata ( L, -1 ) );
	return 0;
}

static int
lua_set_pin_state ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -3 );
	STACK_CHECK(3);
	int32_t pin_num = lua_tonumber ( L, -2 );
	int32_t pin_state = lua_tonumber ( L, -1 );
	set_pin_state ( model, model->device_pins[pin_num], pin_state );
	return 0;
}

static int
lua_get_pin_state ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );

	if ( TRUE == is_pin_high (model->device_pins[pin_num].pin ) )
	{
		lua_pushnumber ( L, SHI );
		return 1;
	}
	else if ( TRUE == is_pin_low (model->device_pins[pin_num].pin ) )
	{
		lua_pushnumber ( L, SLO );
		return 1;
	}
	else if ( TRUE == is_pin_floating (model->device_pins[pin_num].pin ) )
	{
		lua_pushnumber ( L, FLT );
		return 1;
	}
	else
	{
		lua_pushnumber ( L, UNDEFINED );
		return 1;
	}
}

static int
lua_set_pin_bool ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -3 );
	STACK_CHECK(3);
	int32_t pin_num = lua_tonumber ( L, -2 );
	bool pin_level = lua_toboolean ( L, -1 );
	set_pin_bool (model, model->device_pins[pin_num], pin_level );
	return 0;
}

static int
lua_get_pin_bool ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	bool state = get_pin_bool (model->device_pins[pin_num]);
	if ( -1 == state )
	{
		lua_pushnil(L);
		return 1;
	}
	lua_pushboolean ( L,  state);
	return 1;
}

static int
lua_state_to_string ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t state = lua_tonumber ( L, -1 );

	lua_pushstring ( L,  state_to_string(state));
	return 1;
}

static int
lua_is_pin_low ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_low (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_is_pin_high ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_high (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_is_pin_edge ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_edge (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_is_pin_posedge ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_posedge (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_is_pin_negedge ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_negedge (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_is_pin_active ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_active (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_toggle_pin_state ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	toggle_pin_state (model, model->device_pins[pin_num] );
	return 0;
}

static int
lua_is_pin_floating ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	int32_t pin_num = lua_tonumber ( L, -1 );
	lua_pushboolean ( L, is_pin_floating (model->device_pins[pin_num].pin ) );
	return 1;
}

static int
lua_out_log ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	out_log ( model, text );
	return 0;
}

static int
lua_out_message ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	out_message ( model, text );
	return 0;
}

static int
lua_out_warning ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	out_warning ( model, text );
	return 0;
}

static int
lua_out_error ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -2 );
	STACK_CHECK(2);
	const char* text = lua_tostring ( L, -1 );
	out_error ( model, text );
	return 0;
}

static int
lua_set_callback ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -3 );
	STACK_CHECK(3);
	//TODO: Add check integer type
	lua_Number picotime = lua_tonumber ( L, -2 );
	lua_Number eventid = lua_tonumber ( L, -1 );

	set_callback ( model, picotime, eventid );
	return 0;
}

static int
lua_get_systime ( lua_State* L )
{
	ABSTIME curtime = 0;
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -1 );
	systime ( model, &curtime );
	lua_pushnumber ( L, curtime );
	return 1;
}

static int
lua_get_bit ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -3 );
	STACK_CHECK(3);
	//TODO: Add check integer type
	size_t byte = lua_tointeger ( L, -2 );
	size_t bit = lua_tointeger ( L, -1 );
	lua_pushboolean ( L, ( byte >> bit & 0x01 ) );
	return 1;
}

static int
lua_clear_bit ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	IDSIMMODEL* model = (IDSIMMODEL*) lua_touserdata ( L, -3 );
	STACK_CHECK(3);
	//TODO: Add check integer type
	size_t byte = lua_tointeger ( L, -2 );
	size_t bit = lua_tointeger ( L, -1 );
	byte &= ~ ( 1 << bit );
	lua_pushnumber ( L, byte );
	return 1;
}

static int
lua_set_bit ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	//TODO: Add check integer type
	size_t byte = lua_tointeger ( L, -2 );
	size_t bit = lua_tointeger ( L, -1 );
	byte |= ( 1 << bit );
	lua_pushnumber ( L, byte );
	return 1;
}

static int
lua_toggle_bit ( lua_State* L )
{
	lua_Number argnum = lua_gettop ( L );
	//TODO: Add check integer type
	size_t byte = lua_tointeger ( L, -2 );
	size_t bit = lua_tointeger ( L, -1 );
	byte ^= 1 << bit;
	lua_pushnumber ( L, byte );
	return 1;
}