
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

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
