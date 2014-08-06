
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

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
