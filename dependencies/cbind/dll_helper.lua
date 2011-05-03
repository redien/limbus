
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require "file_io"
require "c_parser"

function generate_dll_def( name, destination, c_source_path, c_source_files )
	local def =
	"LIBRARY   " .. name .. "\
	EXPORTS\
	"

	local i = 1

	for _,v in ipairs( c_source_files ) do
		local name = v:match( "([^%/%\\]+).h$" )
		assert( name )
		local file = read_file( c_source_path .. v, true )
		local typedefs = {}
		ParseCHeaderTypedefs( file, typedefs )
		local header = ParseCHeader( file, typedefs )

		for _,func in ipairs( header.func_decls ) do
			def = def .. "   " .. func.name .. "   @" .. i .. "\n"
			i = i + 1
		end
	end

	write_file( destination, def )
end
