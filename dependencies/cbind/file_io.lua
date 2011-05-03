
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

function write_file( filename, string )
	local file = io.open( filename, "wb" )
	assert( file, "Could not open file for writing: " .. filename )

	file:write( string )

	file:close()
end

function read_file( filename, text )
	local file
	if text then
		file = io.open( filename, "r" )
	else
		file = io.open( filename, "rb" )
	end
	assert( file, "Could not open file for reading: " .. filename )

	local string = file:read( "*a" )

	file:close()

	return string
end

