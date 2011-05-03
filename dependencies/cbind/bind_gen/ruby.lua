
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

local function ctype_to_ruby( t )
	local result = ":" .. t.basic_type
	if t.pointer_depth == 1 then
		if t.basic_type == "char" then
			result = ":string"
		else
			result = ":pointer"
		end
	elseif t.enum then
	    result = ":" .. filter_enum( t.basic_type )
	end
	
	result = result:gsub( "^:unsigned ", ":u" )
	
	assert( t.pointer_depth <= 1 )
	return result
end

local function GenerateArgumentTypeList( arguments )
	local str = ""
	for i=1, #arguments do
		local type = ctype_to_ruby( arguments[i].type )
		if type ~= ":void" then
			if i ~= 1 then str = str .. ", " end
			str = str .. type
		end
	end
	return str
end

local function GenerateFunction( package, func )
	local str = "\
	attach_function '" .. func.name .. "', "
	str = str .. "[" .. GenerateArgumentTypeList( func.arguments ) .. "]"
	str = str .. ", " .. ctype_to_ruby( func.return_type )
	return str
end

function generate_binding_ruby( headers )
	local bind_source = "\
require 'rubygems'\
require 'ffi'"

	local module_name = name:gsub( "^%l", string.upper )

	bind_source = bind_source .. "\
module " .. module_name .. "\
	extend FFI::Library\
	libs = Dir['./lib" .. name .. ".{dylib,so*}'] | Dir['./" .. name .. ".dll']\
	libs = libs or '" .. name .. "'\
	ffi_lib *libs"

	for _, header in ipairs( headers ) do
		for _, enum in ipairs( header.enums ) do
			bind_source = bind_source .. "\
	enum :" .. filter_enum( enum.name ) .. ", ["
			for i=1, #enum.entries do
				bind_source = bind_source .. "\
		:'" .. camel_to_std( filter_enum( enum.entries[i]:gsub( enum.name, "" ) ) ) .. "',"
			end
			bind_source = bind_source .. "\
	]"
		end

		for _, define in ipairs( header.defines ) do
			bind_source = bind_source .. "\
	" .. filter_enum( define.name ) .. " = " .. define.value
		end

		for _,func in ipairs( header.func_decls ) do
			bind_source = bind_source .. GenerateFunction( name, func )
		end
	end

	bind_source = bind_source .. "\
end"

	return bind_source
end

