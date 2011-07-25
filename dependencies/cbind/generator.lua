
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require "common"
require "file_io"
require "c_parser"
require "cdef_parser"
require "dll_helper"

local function print_dbg( str )
	if verbose then
		print( str )
	end
end

local do_config = function( config_path, ... )
	local chunk = loadfile( config_path )
	chunk( ... )
end
do_config( ... )

if c_source_path:sub( -1 ) ~= "/" then c_source_path = c_source_path .. "/" end
if cdef_source_path:sub( -1 ) ~= "/" then cdef_source_path = cdef_source_path .. "/" end
if output_path:sub( -1 ) ~= "/" then output_path = output_path .. "/" end

if not cpp_include_output_path then cpp_include_output_path = output_path .. "cpp/"
elseif cpp_include_output_path:sub( -1 ) ~= "/" then cpp_include_output_path = cpp_include_output_path .. "/" end
if not cpp_include_prefix then cpp_include_prefix = "" end

local parsed_c_files, parsed_cdef_files_syntax = {}, {}

local c_source_file_contents = {}
local contents_by_file = {}
for i, c_file in ipairs( c_source_files ) do
	c_source_file_contents[i] = read_file( c_source_path .. c_file, true )
	contents_by_file[c_file] = c_source_file_contents[i]
	print_dbg( "Parsing C header '" .. c_source_path .. c_file .. "'" )
end

-- Parse C headers
for _, contents in ipairs( c_source_file_contents ) do
	local typedefs = {}
	ParseCHeaderTypedefs( contents, typedefs )

	local includes = ParseCHeaderIncludes( contents )
	for _, include in ipairs( includes ) do
		ParseCHeaderTypedefs( contents_by_file[include], typedefs )
	end

	parsed_c_files[#parsed_c_files + 1] = ParseCHeader( contents, typedefs )
end


local class_names, class_dependancies = {}, {}
-- Parse cdef files
for _, cdef_file in ipairs( cdef_source_files ) do
	local contents = read_file( cdef_source_path .. cdef_file, true )
	print_dbg( "Parsing cdef file '" .. cdef_source_path .. cdef_file .. "'" )
	parsed_cdef_files_syntax[#parsed_cdef_files_syntax + 1],
	class_dependancies[#class_dependancies + 1] = cdef_parser:parse( contents )
	class_names[#class_names + 1] = cdef_file:match( "^([%w_]+)" )
end

for _, language in ipairs( languages ) do
	if language == "lua" then
		print_dbg( "Generating Lua binding..." )
		require "bind_gen.lua"
		local header, source = generate_binding_lua( parsed_c_files, c_source_files )
		write_file( output_path .. "lua/" .. name .. "_lua.h", header )
		write_file( output_path .. "lua/" .. name .. "_lua.c", source )
		generate_dll_def( name, output_path .. "lua/" .. name .. ".def", output_path .. "lua/", {name .. "_lua.h"} )

		require "class_gen.lua"
		for i = 1, #parsed_cdef_files_syntax do
			local class = generate_class_lua( parsed_cdef_files_syntax[i] )
			if class ~= "" then
				write_file( output_path .. "lua/" .. std_to_camel( class_names[i] ) .. ".lua", class )
			end
		end

	elseif language == "ruby" then
		print_dbg( "Generating Ruby binding..." )
		require "bind_gen.ruby"
		local source = generate_binding_ruby( parsed_c_files )
		write_file( output_path .. "ruby/" .. name .. ".rb", source )

		require "class_gen.ruby"
		for i = 1, #parsed_cdef_files_syntax do
			local class = generate_class_ruby( parsed_cdef_files_syntax[i] )
			if class ~= "" then
				write_file( output_path .. "ruby/" .. class_names[i] .. ".rb", class )
			end
		end

	elseif language == "python" then
		print_dbg( "Generating Python binding..." )
		require "bind_gen.python"
		local source = generate_binding_python( parsed_c_files )
		write_file( output_path .. "python/" .. name .. ".py", source )

		require "class_gen.python"
		for i = 1, #parsed_cdef_files_syntax do
			local class = generate_class_python( parsed_cdef_files_syntax[i] )
			if class ~= "" then
				write_file( output_path .. "python/" .. class_names[i] .. ".py", class )
			end
		end

	elseif language == "csharp" then
		print_dbg( "Generating C# binding..." )
		require "bind_gen.csharp"
		local source = generate_binding_csharp( parsed_c_files, name )
		write_file( output_path .. "csharp/" .. name .. ".cs", source )

		require "class_gen.csharp"
		for i = 1, #parsed_cdef_files_syntax do
			local class = generate_class_csharp( parsed_cdef_files_syntax[i] )
			write_file( output_path .. "csharp/" .. std_to_camel( class_names[i] ) .. ".cs", class )
		end

	elseif language == "go" then
		print_dbg( "Generating Go! binding..." )
		require "bind_gen.go_syscall"
		local source, makefile = generate_binding_go( parsed_c_files )
		write_file( output_path .. "go/" .. name .. ".go", source )
		write_file( output_path .. "go/Makefile", makefile )

	elseif language == "cpp" then
		print_dbg( "Generating C++ binding..." )
		require "class_gen.cpp"
		for i = 1, #parsed_cdef_files_syntax do
			local header, source = generate_class_cpp( parsed_cdef_files_syntax[i], class_dependancies[i], c_source_files, cpp_include_prefix )
			write_file( cpp_include_output_path .. std_to_camel( class_names[i] ) .. ".hpp", header )
			if source ~= "" then
				write_file( output_path .. "cpp/" .. std_to_camel( class_names[i] ) .. ".cpp", source )
			end
		end
	end
end

