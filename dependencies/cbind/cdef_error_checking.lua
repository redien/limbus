
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

local function each( list, func )
	for _, item in ipairs( list ) do
		func( item )
	end
end

local symbol_exists

local function add_identifier( symbol, name )
	if symbol_exists[name] then
		error( "Identifier has already been defined: '" .. name .. "'" )
	end
	symbol_exists[name] = true
end

local function class_statement( symbol )
	if symbol.is_define_member then
		for i = 1, #symbol.identifier_type_pairs do
			add_identifier(symbol, symbol.identifier_type_pairs[i].name.token.value)
		end
	end
	if symbol.is_property then
			add_identifier(symbol, symbol.name.token.value)
	end
end

function cdef_error_checking( symbol )
	symbol_exists = {}
	if symbol.is_class then
		each( symbol.statements, class_statement )
	end
end
