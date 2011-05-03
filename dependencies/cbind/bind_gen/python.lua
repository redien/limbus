
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

local function ctype_to_python( t )
	local result = "c_" .. t.basic_type

    if t.basic_type:find( "unsigned char" ) then result = "c_ubyte" end
    if t.basic_type:find( "unsigned int" ) then result = "c_uint" end

	if t.pointer_depth == 1 then result = result .. "_p" end
	assert( t.pointer_depth <= 1 )
	return result
end

local function GenerateArgumentTypeList( arguments )
	local str = ""
	for i=1, #arguments do
		local type = ctype_to_python( arguments[i].type )
		if type ~= "c_void" then
			if i ~= 1 then str = str .. ", " end
			str = str .. type
		end
	end
	return str
end

local function GenerateFunction( package, func )
	local str = ""
	str = str .. "\
" .. func.name .. " = " .. package .. "." .. func.name
	
	local return_type = ctype_to_python( func.return_type )
	if return_type ~= "c_void" then
		str = str .. "\
" .. func.name .. ".restype = " .. return_type
	end

	str = str .. "\
" .. func.name .. ".argtypes = [" .. GenerateArgumentTypeList( func.arguments ) .. "]"
	return str
end

function generate_binding_python( headers )
	local bind_source = "\
from ctypes import *\
import platform\
if platform.system() == \"Windows\":\
	try:\
		" .. name .. " = cdll." .. name .. "\
	except WindowsError:\
		" .. name .. " = cdll.lib" .. name .. "\
else:\
	" .. name .. " = cdll.LoadLibrary(\"lib" .. name .. ".so\")"

	for _, header in ipairs( headers ) do
		for _,func in ipairs( header.func_decls ) do
			bind_source = bind_source .. GenerateFunction( name, func )
		end

		for _, enum in ipairs( header.enums ) do
			for i=1, #enum.entries do
				bind_source = bind_source .. "\
" .. camel_to_std( filter_enum( enum.entries[i] ) ):upper() .. " = " .. (i - 1)
			end
		end

		for _, define in ipairs( header.defines ) do
			bind_source = bind_source .. "\
" .. camel_to_std( filter_enum( define.name ) ):upper() .. " = " .. define.value
		end
	end
	
	return bind_source
end

