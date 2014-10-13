
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

local function ctype_to_python( t )
	if t.enum then
	    return "c_int"
    else
        local result = "c_" .. t.basic_type:gsub("unsigned char", "byte"):gsub("unsigned", "u"):gsub(" ", "")

        if t.pointer_depth == 1 then result = result .. "_p" end
        assert( t.pointer_depth <= 1 )

        return result
    end
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
import sys\
if sys.platform == \"win32\":\
	try:\
		" .. name .. " = cdll." .. name .. "\
	except WindowsError:\
		" .. name .. " = cdll.lib" .. name .. "\
elif sys.platform == \"darwin\":\
	" .. name .. " = cdll.LoadLibrary(\"lib" .. name .. ".dylib\")\
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

