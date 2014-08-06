
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

local function ctype_to_go( t )
	if t.replaced_typedef then
		return "C." .. t.replaced_typedef
	elseif t.pointer_depth == 1 then
		if t.basic_type == "char" then
			return "C.CString"
		else
			return "unsafe.Pointer"
		end
	elseif t.pointer_depth > 1 then
		return "unsafe.Pointer"
	elseif t.basic_type == "char" then
		return "C.char"
	elseif t.basic_type == "unsigned char" then
		return "C.uchar"
	elseif t.basic_type == "int" then
		return "C.int"
	elseif t.basic_type == "unsigned int" then
		return "C.uint"
	elseif t.basic_type == "long" then
		return "C.long"
	elseif t.basic_type == "unsigned long" then
		return "C.ulong"
	elseif t.basic_type == "float" then
		return "C.float"
	elseif t.basic_type == "double" then
		return "C.double"
	elseif t.enum then
	    return "C.int"
	end

	assert( false, t.basic_type )
end

local function ctype_to_go_native( t )
	if t.pointer_depth == 1 then
		if t.basic_type == "char" then
			return "string"
		else
			return "uintptr"
		end
	elseif t.pointer_depth > 1 then
		return "uintptr"
	elseif t.basic_type == "char" then
		return "int8"
	elseif t.basic_type == "unsigned char" then
		return "uint8"
	elseif t.basic_type == "int" then
		return "int"
	elseif t.basic_type == "unsigned int" then
		return "uint"
	elseif t.basic_type == "long" then
		return "int"
	elseif t.basic_type == "unsigned long" then
		return "uint"
	elseif t.basic_type == "float" then
		return "float32"
	elseif t.basic_type == "double" then
		return "float64"
	elseif t.enum then
	    return "int"
	end
	
	assert( false, t.basic_type )
end

local function ctype_to_go_native_cast( t )
	if t.pointer_depth == 1 then
		if t.basic_type == "char" then
			return "C.GoString"
		end
	end
	
	return ctype_to_go_native( t )
end

local function GenerateArgumentTypeList( arguments )
	local str = ""
	for i, arg in ipairs( arguments ) do
		str = str .. filter_identifier( arg.identifier ) .. " " .. ctype_to_go_native( arg.type )
		if i < #arguments then str = str .. ", " end
	end
	return str
end

local function argument_call( arg )
	return ctype_to_go( arg.type ) .. "(" .. filter_identifier( arg.identifier ) .. ")"
end

local function is_void( t )
	return t.basic_type == "void" and t.pointer_depth == 0
end

local function GenerateFunction( func )
	str = "\
func " .. std_to_camel( filter_function( func.name ) ) .. "(" .. GenerateArgumentTypeList( func.arguments ) .. ") "
	if not is_void( func.return_type ) then
		str = str .. ctype_to_go_native( func.return_type )
	end
	str = str .. " {\
	"
	if not is_void( func.return_type ) then
		str = str .. "return " .. ctype_to_go_native_cast( func.return_type ) .. "("
	end
	
	str = str .. "C." .. func.name .. "(" .. list_as_string( func.arguments, argument_call, ", " ) .. ")"
	
	if not is_void( func.return_type ) then
		str = str .. ")"
	end
	
	str = str .. "\
}"

	return str
end

function generate_binding_go( headers )
	local module_name = std_to_camel( name )
	local bind_source = "\
package " .. name

	for i = 1, #c_source_files do
		bind_source = bind_source .. "\
// #include <" .. c_source_files[i] .. ">"
	end

	bind_source = bind_source .. "\
import \"C\"\
import \"unsafe\"\
"

	for _, header in ipairs( headers ) do
		bind_source = bind_source .. "\
const ("
		for _, enum in ipairs( header.enums ) do
			for i = 1, #enum.entries do
				bind_source = bind_source .. "\
	" .. camel_to_std( filter_enum( enum.entries[i] ) ):upper() .. " = " .. (i - 1)
			end
		end

		for _, define in ipairs( header.defines ) do
			bind_source = bind_source .. "\
	" .. camel_to_std( filter_enum( define.name ) ):upper() .. " = " .. define.value
		end
		bind_source = bind_source .. "\
)"

		for _,func in ipairs( header.func_decls ) do
			bind_source = bind_source .. GenerateFunction( func )
		end
	end
	
	local libs = ""
	for _, lib in pairs(c_shared_libraries) do
		libs = libs .. "-l" .. lib .. " "
	end

	local makefile = "\
include $(GOROOT)/src/Make.inc\
\
TARG=" .. name .. "\
CGOFILES:=" .. name .. ".go\
CLEANFILES+=" .. name .. "\
CGO_LDFLAGS:=-L../../lib -l" .. name .. " " .. libs .. "\
CGO_CFLAGS:=-I../../include\
\
include $(GOROOT)/src/Make.pkg\
\
%: install %.go\
	$(GC) $*.go\
	$(LD) -o $@ $*.$O\
"

	return bind_source, makefile
end
