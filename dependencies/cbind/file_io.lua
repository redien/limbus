
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

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

