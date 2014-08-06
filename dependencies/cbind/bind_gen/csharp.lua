
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

local function filter_identifier( identifier )
	if identifier == "string" then
		return "_string"
	else
		return identifier
	end
end

local function ctype_to_csharp( t )
	if t.pointer_depth == 1 then
		if t.basic_type == "char" then
			return "System.String"
		else
			return "System.IntPtr"
		end
	elseif t.pointer_depth > 1 then
		return "System.IntPtr"
	elseif t.basic_type == "char" then
		return "System.SByte"
	elseif t.basic_type == "unsigned char" then
		return "System.Byte"
	elseif t.basic_type == "int" then
		return "System.Int32"
	elseif t.basic_type == "unsigned int" then
		return "System.UInt32"
	elseif t.basic_type == "long" then
		return "System.Int32"
	elseif t.basic_type == "unsigned long" then
		return "System.UInt32"
	elseif t.basic_type == "float" then
		return "System.Single"
	elseif t.basic_type == "double" then
		return "System.Double"
	elseif t.basic_type == "void" then
		return "void"
	elseif t.enum then
	    return "int"
	end
	
	assert( false, t.basic_type )
end

local function GenerateArgumentTypeList( arguments )
	local str = ""
	for i, arg in ipairs( arguments ) do
		str = str .. ctype_to_csharp( arg.type ) .. " " .. filter_identifier( arg.identifier )
		if i < #arguments then str = str .. ", " end
	end
	return str
end

local function GenerateFunction( dll_import, func )
	return "\
		[DllImport(\"" .. dll_import .. "\", CallingConvention=CallingConvention.Cdecl)]\
		public static extern " .. ctype_to_csharp( func.return_type ) .. " " .. func.name .. "(" .. GenerateArgumentTypeList( func.arguments ) .. ");"
end

function generate_binding_csharp( headers, dll_import )
	local module_name = std_to_camel( name )
	local bind_source = "\
namespace " .. module_name .. "\
{\
	using System.Runtime.InteropServices;\
	public class Imported\
	{"
	for _, header in ipairs( headers ) do
		for _, enum in ipairs( header.enums ) do
			for i = 1, #enum.entries do
				bind_source = bind_source .. "\
		public const int " .. camel_to_std( filter_enum( enum.entries[i] ) ):upper() .. " = " .. (i - 1) .. ";"
			end
		end

		for _, define in ipairs( header.defines ) do
			bind_source = bind_source .. "\
		public const int " .. camel_to_std( filter_enum( define.name ) ):upper() .. " = " .. define.value .. ";"
		end

		for _,func in ipairs( header.func_decls ) do
			bind_source = bind_source .. GenerateFunction( dll_import, func )
		end
	end

	bind_source = bind_source .. "\
	}\
}"

	return bind_source
end

