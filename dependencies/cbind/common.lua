
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

function camel_to_std( str )
	return str:gsub( "(%l)(%d+)", function( a, b ) return a .. '_' .. b end )
              :gsub( "^%u+%d*", string.lower )
			  :gsub( "%u+%d*", function( s ) return '_' .. string.lower( s ) end )
end

function std_to_lcamel( name )
	return name:gsub( "_(%l)", string.upper )
end

function std_to_camel( name )
	return std_to_lcamel( name ):gsub( "^%l", string.upper )
end

local indentation = 0
function line( line, increment )
	if line == nil and increment then
		indentation = indentation + increment
		return ""
	end
	local str = "\n" .. string.rep( "	", indentation ) .. line
	if increment then indentation = indentation + increment end
	return str
end

function filter_enum( name )
    if not enum_filters then return name end

    for _, filter in ipairs( enum_filters ) do
        local tmp = name:match( filter )
        if tmp then name = tmp end
    end

    return name
end

function filter_function( name )
    if not function_filters then return name end

    for _, filter in ipairs( function_filters ) do
        local tmp = name:match( filter )
        if tmp then name = tmp end
    end

    return name
end

function list_as_string( list, func, separator, prefix, ... )
	if #list == 0 then return "" end

	if not separator then separator = "" end
	if not func then func = function(a) return a end end

	local str = ""
	if prefix then str = separator end
	for i, item in ipairs( list ) do
		local value = func( item, ... )
		if value then
			str = str .. value
			if i < #list then str = str .. separator end
		end
	end
	return str
end

function list_class_statements( class_statements, type )
	local statements = {}
	for _, statement in ipairs( class_statements ) do
		if statement[type] then
			statements[#statements + 1] = statement
		end
	end
	return statements
end

function list_signals( class_statements )
	return list_class_statements( class_statements, "is_signal" )
end

function list_properties( class_statements )
	return list_class_statements( class_statements, "is_property" )
end

function find_constructor( class_statements )
	for _, statement in ipairs( class_statements ) do
		if statement.is_method_definition and statement.name.token.value == "construct" then
			return statement
		end
	end
end
