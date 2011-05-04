
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require 'common'

local function GenerateArgument( arg, i )
	local str = ""
	str = str .. c_var_decl( arg.type, arg.identifier ) .. " = (" .. c_type( arg.type ) .. ")"

	if arg.type.func then
		str = str .. "lua_touserdata"
	elseif arg.type.basic_type == "char"
	and arg.type.pointer_depth == 1 then
		str = str .. "luaL_checkstring"
	elseif arg.type.pointer_depth > 0 then
		str = str .. "lua_touserdata"
	elseif arg.type.basic_type:find( "int" )
		or arg.type.basic_type:find( "float" )
		or arg.type.basic_type:find( "double" )
		or arg.type.basic_type:find( "char" )
		or arg.type.basic_type:find( "short" )
		or arg.type.basic_type:find( "long" )
		or arg.type.enum
	then
		str = str .. "luaL_checknumber"
	else
		assert( false, "Unknown C type: '" .. c_type( arg.type ) .. "'" )
	end

	str = str .. "( L, " .. i .. " );\n"

	return str;
end

local function GenerateReturnValue( func )
	local str = ""

	if func.return_type.func then
		str = str .. "lua_pushlightuserdata"
	elseif  func.return_type.basic_type == "char"
	and func.return_type.pointer_depth == 1 then
		str = str .. "lua_pushstring"
	elseif func.return_type.pointer_depth > 0 then
		str = str .. "lua_pushlightuserdata"
	elseif func.return_type.basic_type:find( "int" )
		or func.return_type.basic_type:find( "float" )
		or func.return_type.basic_type:find( "double" )
		or func.return_type.basic_type:find( "char" )
		or func.return_type.basic_type:find( "short" )
		or func.return_type.basic_type:find( "long" )
		or func.return_type.enum
	then
		str = str .. "lua_pushnumber"
	else
		assert( false, "Unknown C type: '" .. c_type( func.return_type ) .. "'" )
	end

	str = str .. "( L, _lua_ret_value );\n"

	return str;
end

local function GenerateCFunctionCall( func )
	local str = ""

	str = str .. func.name .. "( "

	for i = 1, #func.arguments do
		str = str .. func.arguments[i].identifier
		if i ~= #func.arguments then
			str = str .. ", "
		end
	end

	str = str .. " );\n"

	return str
end

local function GenerateCLuaFunction( func )
	local str = ""

	str = str .. "static int lua_" .. func.name .. "( lua_State* L ) {\n"

	-- Argument declarations
	for i = 1, #func.arguments do
		str = str .. GenerateArgument( func.arguments[i], i )
	end

	-- Return statement
	if  func.return_type.basic_type == "void"
	and func.return_type.pointer_depth == 0 then
		str = str
	..	GenerateCFunctionCall( func )
	.. "return 0;\n"
	else
		str = str
	.. c_var_decl( func.return_type, "_lua_ret_value" ) .. " = " .. "(" .. c_type( func.return_type ) .. ")"
	..	GenerateCFunctionCall( func )
	..	GenerateReturnValue( func )
	.. "return 1;\n"
	end

	str = str .. "}\n"

	return str
end

local function GenerateFunctionLoader( func )
	local str = ""

	str = str .. "	lua_pushstring( L, \"" .. func.name .. "\" );\n"
			  .. "	lua_pushcfunction( L, lua_" .. func.name .. " );\n"
			  .. "	lua_settable( L, -3 );\n"

	return str
end

local function GenerateEnumLoader( enum )
	local str = ""

	for i = 1, #enum.entries do
		str = str .. "	lua_pushstring( L, \"" .. filter_enum( enum.entries[i] ) .. "\" );\n"
				  .. "	lua_pushnumber( L, " .. (i - 1) .. " );\n"
				  .. "	lua_settable( L, -3 );\n"
	end

	return str
end

local function GenerateDefineLoader( define )
	local str = ""

	str = str .. "	lua_pushstring( L, \"" .. filter_enum( define.name ) .. "\" );\n"
			  .. "	lua_pushnumber( L, " .. define.value .. " );\n"
			  .. "	lua_settable( L, -3 );\n"

	return str
end

function generate_binding_lua( headers, dependencies )
	local bind_header = "\
#ifndef " .. name:upper() .. "_LUA_H\
#define " .. name:upper() .. "_LUA_H\
#ifdef __cplusplus\
extern \"C\" {\
#endif\
#include <lua.h>\
int luaopen_" .. name .. "( lua_State* L );\
#ifdef __cplusplus\
}\
#endif\
#endif\
"

	local bind_source = "\
#include \"" .. name .. "_lua.h\"\
#include <lauxlib.h>\
"
	for _, path in ipairs( dependencies ) do
		bind_source = bind_source ..
"#include <" ..  path .. ">\n"
	end

	for _, header in ipairs( headers ) do
		for _, func in ipairs( header.func_decls ) do
			bind_source = bind_source .. GenerateCLuaFunction( func )
		end
	end

	bind_source = bind_source .. "\
int luaopen_" .. name .. "( lua_State* L ) {\
	lua_newtable( L );\
"
	for _,header in ipairs( headers ) do
		for _, func in ipairs( header.func_decls ) do
			bind_source = bind_source .. GenerateFunctionLoader( func )
		end

		for _, enum in ipairs( header.enums ) do
			bind_source = bind_source .. GenerateEnumLoader( enum )
		end

		for _, define in ipairs( header.defines ) do
			bind_source = bind_source .. GenerateDefineLoader( define )
		end
	end
	bind_source = bind_source ..
"	lua_setglobal( L, \"" .. name .. "\" );\
	return 0;\
}\
\
"

	return bind_header, bind_source
end

