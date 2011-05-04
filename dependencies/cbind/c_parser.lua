
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

function c_var_decl( type, ident )
	local basic
	if type.enum then basic = "int"
	else basic = type.basic_type end
	
	local str = basic
	.. string.rep( "*", type.pointer_depth )
	.. " "
	
	if type.func then
		str = str .. "(*" .. ident .. ")("
		for i = 1, #type.arguments do
			if i > 1 then str = str .. ", " end
			str = str .. c_type(type.arguments[i].type)
		end
		str = str .. ")"
	else
		str = str .. ident
	end
	
	for i = 1, type.array_dimensions do
		str = str .. "[" .. type.array_lengths[i] .. "]"
	end
	
	return str
end

local test_type = {basic_type = "unsigned char",
				   pointer_depth = 3,
				   array_dimensions = 2,
				   array_lengths = {}}
test_type.array_lengths[1] = 32
test_type.array_lengths[2] = 435

local test_var_decl = c_var_decl( test_type, "array" )
assert( test_var_decl == "unsigned char*** array[32][435]" )


function c_type( type )
	if type.enum then
		return "int"
	elseif type.func then
		local str = type.basic_type ..
			   string.rep( "*", type.pointer_depth ) ..
			   "(*)("
		for i = 1, #type.arguments do
			if i > 1 then str = str .. ", " end
			str = str .. c_type(type.arguments[i].type)
		end
		str = str .. ")"
		return str
	else
		return type.basic_type ..
			   string.rep( "*", type.pointer_depth )
	end
end

local function trim( str )
	return str:gsub( "^%s*(.-)%s*$", "%1" )
end

function filter_c_basic_type( basic_type )
	-- Remove macro stuff...
	return trim( basic_type:gsub(
		"([%w_]+)",
		function( str )
			if str == "unsigned"
			or str == "signed"
			or str == "const"
			or str == "int"
			or str == "short"
			or str == "long"
			or str == "float"
			or str == "double"
			or str == "char"
			or str == "void"
			or str == "enum"
			then
				return nil
			else
				return ""
			end
		end))
end

function parse_c_arguments( arguments )
	local result = {}

	local filtered_captures = {}
	local filtered_arguments = arguments:sub(2, -2):gsub("%b()", function(capture)
		if capture:sub(2, 2) == "*" then
			return capture
		else
			filtered_captures[#filtered_captures + 1] = capture
			return #filtered_captures
		end
	end)

	local function handle_argument( basic_type, stars, ident, brackets )
		local star_count
		_,star_count = stars:gsub( "(*)", function() end )

		local array_dimensions = 0
		local array_lengths = {}
		local func_arguments = nil
		
		if brackets:find( "%[" ) then
			for len in brackets:gmatch( "%d+" ) do
				array_lengths[#array_lengths + 1] = tonumber( len )
				array_dimensions = array_dimensions + 1
			end
		else
			if tonumber(brackets) then
				func_arguments = parse_c_arguments(filtered_captures[tonumber(brackets)])
			end
		end

		-- Remove unnecessary space
		basic_type = basic_type:gsub( "%s+", " " )

		-- Check for const-ness
		local const = false
		basic_type = basic_type:gsub( "const ", function( s )
			const = true
			return ""
		end)

		-- Check for enums
		local enum = false
		basic_type = basic_type:gsub( "enum ", function( s )
			enum = true
			return ""
		end)
		
		result[#result + 1] = {
			identifier = ident,
			type = {
				pointer_depth = star_count,
				basic_type = basic_type,
				array_dimensions = array_dimensions,
				array_lengths = array_lengths,
				const = const,
				enum = enum,
				func = func_arguments ~= nil,
				arguments = func_arguments,
			},
		}
	end
	
	filtered_arguments:gsub("([%w_][%s%w_]+[%w_])([%*%s]+)%(?%*?([%w_]+)%)?([%[%]%d%s]*)(%d*)", handle_argument)

	return result
end

local args = parse_c_arguments( "( const char* _str,\
								   unsigned int i,\
								   void* *ptrptr,\
								   short *  ** * array [463] [2   ],\
								   int (*myfunction) ( int f, void* d),\
								   long   int  ** array2[  3]  [32],\
								   enum MyEnum test )" )
assert( #args == 7 )
assert( args[1] )
assert( args[1].type.basic_type == "char" )
assert( args[1].type.pointer_depth == 1 )
assert( args[1].type.const == true )
assert( args[1].type.enum == false )
assert( args[1].type.func == false )
assert( args[1].type.array_dimensions == 0 )
assert( #args[1].type.array_lengths == 0 )
assert( args[1].identifier == "_str" )
assert( args[2] )
assert( args[2].type.basic_type == "unsigned int" )
assert( args[2].type.pointer_depth == 0 )
assert( args[2].type.const == false )
assert( args[2].type.enum == false )
assert( args[2].type.func == false )
assert( args[2].type.array_dimensions == 0 )
assert( #args[2].type.array_lengths == 0 )
assert( args[2].identifier == "i" )
assert( args[3] )
assert( args[3].type.basic_type == "void" )
assert( args[3].type.pointer_depth == 2 )
assert( args[3].type.const == false )
assert( args[3].type.enum == false )
assert( args[3].type.func == false )
assert( args[3].type.array_dimensions == 0 )
assert( #args[3].type.array_lengths == 0 )
assert( args[3].identifier == "ptrptr" )
assert( args[4] )
assert( args[4].type.basic_type == "short" )
assert( args[4].type.pointer_depth == 4 )
assert( args[4].type.const == false )
assert( args[4].type.enum == false )
assert( args[4].type.func == false )
assert( args[4].type.array_dimensions == 2 )
assert( #args[4].type.array_lengths == 2 )
assert( args[4].type.array_lengths[1] == 463 )
assert( args[4].type.array_lengths[2] == 2 )
assert( args[4].identifier == "array" )

assert( args[5] )
assert( args[5].type.basic_type == "int" )
assert( args[5].type.pointer_depth == 0 )
assert( args[5].type.const == false )
assert( args[5].type.enum == false )
assert( args[5].type.func == true )
assert( args[5].type.array_dimensions == 0 )
assert( #args[5].type.array_lengths == 0 )
assert( args[5].identifier == "myfunction" )

local args5 = args[5].type.arguments
assert( #args5 == 2 )
assert( args5[1] )
assert( args5[1].type.basic_type == "int" )
assert( args5[1].type.pointer_depth == 0 )
assert( args5[1].type.const == false )
assert( args5[1].type.enum == false )
assert( args5[1].type.func == false )
assert( args5[1].type.array_dimensions == 0 )
assert( #args5[1].type.array_lengths == 0 )
assert( args5[1].identifier == "f" )
assert( args5[2] )
assert( args5[2].type.basic_type == "void" )
assert( args5[2].type.pointer_depth == 1 )
assert( args5[2].type.const == false )
assert( args5[2].type.enum == false )
assert( args5[2].type.func == false )
assert( args5[2].type.array_dimensions == 0 )
assert( #args5[2].type.array_lengths == 0 )
assert( args5[2].identifier == "d" )

assert( args[6] )
assert( args[6].type.basic_type == "long int" )
assert( args[6].type.pointer_depth == 2 )
assert( args[6].type.const == false )
assert( args[6].type.enum == false )
assert( args[6].type.func == false )
assert( args[6].type.array_dimensions == 2 )
assert( #args[6].type.array_lengths == 2 )
assert( args[6].type.array_lengths[1] == 3 )
assert( args[6].type.array_lengths[2] == 32 )
assert( args[6].identifier == "array2" )
assert( args[7] )
assert( args[7].type.basic_type == "MyEnum" )
assert( args[7].type.pointer_depth == 0 )
assert( args[7].type.const == false )
assert( args[7].type.enum == true )
assert( args[7].type.func == false )
assert( args[7].type.array_dimensions == 0 )
assert( #args[7].type.array_lengths == 0 )
assert( args[7].identifier == "test" )

args = parse_c_arguments( "( void )" )
assert( #args == 0 )

args = parse_c_arguments( "()" )
assert( #args == 0 )


function parse_return_type( return_type )
	result = {}

	return_type, result.pointer_depth = return_type:gsub( "*", "" )

	return_type = return_type:gsub( "^%s+", "" )
	return_type = return_type:gsub( "%s+$", "" )

	result.basic_type = return_type:gsub( "%s+", " " )

	result.const = false
	-- Check for const-ness
	result.basic_type = result.basic_type:gsub( "const ", function( s )
		result.const = true
		return ""
	end)

	result.enum = false
	-- Check for enums
	result.basic_type = result.basic_type:gsub( "enum ", function( s )
		result.enum = true
		return ""
	end)

	result.array_dimensions = 0
	result.array_lengths = {}

	return result
end


local return_type = parse_return_type( " void* " )
assert( return_type.basic_type == "void" )
assert( return_type.pointer_depth == 1 )
assert( return_type.const == false )
assert( return_type.enum == false )
assert( return_type.array_dimensions == 0 )
assert( #return_type.array_lengths == 0 )

return_type = parse_return_type( "unsigned		  \nchar* * * " )
assert( return_type.basic_type == "unsigned char" )
assert( return_type.pointer_depth == 3 )
assert( return_type.const == false )
assert( return_type.enum == false )
assert( return_type.array_dimensions == 0 )
assert( #return_type.array_lengths == 0 )

return_type = parse_return_type( "const char*" )
assert( return_type.basic_type == "char" )
assert( return_type.pointer_depth == 1 )
assert( return_type.const == true )
assert( return_type.enum == false )
assert( return_type.array_dimensions == 0 )
assert( #return_type.array_lengths == 0 )

return_type = parse_return_type( "enum _TestEnum" )
assert( return_type.basic_type == "_TestEnum" )
assert( return_type.pointer_depth == 0 )
assert( return_type.const == false )
assert( return_type.enum == true )
assert( return_type.array_dimensions == 0 )
assert( #return_type.array_lengths == 0 )



function FunctionDeclaration( return_type, name, arguments )
	return {
		return_type = return_type,
		name = name,
		arguments = arguments,
	}
end

table.add = function( t, v )
	t[#t + 1] = v
end


local typedef_pattern = "typedef%s+([%w_]+%**)%s+([%w_]+);"
assert( string.find( "typedef a b;", typedef_pattern ) )
assert( string.find( "typedef a* b;", typedef_pattern ) )
assert( string.find( "typedef abc*** b;", typedef_pattern ) )

local typedef_enum_pattern = "typedef%s+(enum [%w_]+%**)%s+([%w_]+);"
assert( string.find( "typedef enum a b;", typedef_enum_pattern ) )
assert( string.find( "typedef enum a* b;", typedef_enum_pattern ) )
assert( string.find( "typedef enum abc*** b;", typedef_enum_pattern ) )

function ParseCHeaderIncludes( header )
	local include_pattern = "%#include%s+%<([^%>]+)%>"
	-- Parse includes
	local includes = {}
	for include in header:gmatch( include_pattern ) do
		includes[#includes + 1] = include
	end
	
	return includes
end

function ParseCHeaderTypedefs( header, typedefs )
	-- Handle typedef's
	for type, name in header:gmatch( typedef_pattern ) do
		typedefs[#typedefs + 1] = {type = type, name = name}
	end
	for type, name in header:gmatch( typedef_enum_pattern ) do
		typedefs[#typedefs + 1] = {type = type, name = name}
	end
end

function ParseCHeader( header, typedefs )
	-- Remove comments
    header = header:gsub( "%/%*.-%*%/", "" )

	local function_decl_pattern = "([%w%*_ ]+)%s([%w_]+)%s*(%b())%s*;"

	local enum_pattern = "enum%s*([%w_]*)%s*(%b{})%s*;"
	local enum_entry_pattern = "([%w_]+)"

	local define_pattern = "#define%s+([%w_]+)%s+(%d[%w]*)%s*\n"

	local doc = {
		func_decls = {},
		enums = {},
		defines = {},
	}

	for _,typedef in ipairs( typedefs ) do
		header = header:gsub( "([^%w_])" .. typedef.name .. "([^%w_])", "%1" .. typedef.type .. "%2" )
	end

	-- Parse function declarations
	for return_type, name, arguments in header:gmatch( function_decl_pattern ) do
		doc.func_decls[#doc.func_decls + 1] =
			FunctionDeclaration( parse_return_type( return_type ),
								 name,
								 parse_c_arguments( arguments ) )
	end

	-- Parse enums
	for identifier, block in header:gmatch( enum_pattern ) do
		local enum_entries = {}
		
		for entry in block:gmatch( enum_entry_pattern ) do
			enum_entries[#enum_entries + 1] = entry
		end

		doc.enums[#doc.enums + 1] = {entries = enum_entries, name = identifier}
	end

	-- Parse defines
	for name, value in header:gmatch( define_pattern ) do
		doc.defines[#doc.defines + 1] = {name = name, value = value}
	end

	return doc
end

