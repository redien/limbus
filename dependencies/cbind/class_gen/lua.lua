
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

require "common"

local function arguments(arguments, func)
	return "( this" .. list_as_string( arguments, func, ", ", true ) .. " )"
end

local function call_arguments(arguments, func)
	if #arguments > 0 then
		return "( " .. list_as_string( arguments, func, ", " ) .. " )"
	else
		return "()"
	end
end

local function filter_method_name( name )
	return std_to_lcamel( name:gsub( "(.+)%?$", "is_%1" ):gsub( "%!$", "" ) )
end

local function transform_operator( op )
	if op == "Plus" then return "+" end
	if op == "Minus" then return "-" end
	if op == "Equal" then return "==" end
	if op == "NotEqual" then return "~=" end
	if op == "LessThan" then return "<" end
	if op == "GreaterThan" then return ">" end
	if op == "LessThanEqual" then return "<=" end
	if op == "GreaterThanEqual" then return ">=" end
	if op == "Is" then return "==" end
end

local function transform_expression( symbol )
	if symbol.is_literal then
		return symbol.token.value
	end
	if symbol.is_identifier then
		return symbol.token.value
	end
	if symbol.is_infix_op then
		return "(" .. transform_expression( symbol.left ) .. " " .. transform_operator( symbol.op ) .. " " .. transform_expression( symbol.right ) .. ")"
	end
	if symbol.is_enum then
		return symbol.namespace.token.value .. "." .. filter_enum( symbol.identifier.token.value )
	end
	if symbol.is_member then
		return symbol.object.token.value .. "." .. symbol.name.token.value
	end
	if symbol.is_constant then
		return symbol.namespace.token.value .. "." .. filter_enum( symbol.name.token.value )
	end
	if symbol.is_method_call then
		return symbol.object.token.value .. ":" .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_call then
		return symbol.namespace.token.value .. "." .. symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_new then
		return camel_to_std( symbol.type.namespace ) .. "." .. symbol.type.class .. call_arguments( symbol.arguments, transform_expression )
	end
	assert( false, "Unknown expression" )
end

local function transform_statement( symbol, raw_set )
	if symbol.is_return then
		return line( "return " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_if then
		return line( "if " .. transform_expression( symbol.expression ) .. " then", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "end" )
	end
	if symbol.is_assert then
		return line( "assert( " .. transform_expression( symbol.expression ) .. " )" )
	end
	if symbol.is_local_declaration then
		local str = ""
		for _, item in ipairs( symbol.identifier_type_pairs ) do
			str = str .. line( "local " .. item.name.token.value )
		end
		return str
	end
	if symbol.is_while then
		return line( "while " .. transform_expression( symbol.expression ) .. " do", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "end" )
	end
	if symbol.is_assign_member then
		if raw_set then
			return line( "rawset( " .. symbol.object.token.value .. ", \"" .. symbol.name.token.value .. "\", " .. transform_expression( symbol.expression ) .. " )" )
		else
			return line( symbol.object.token.value .. "." .. symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) )
		end
	end
	if symbol.is_method_call then
		return line( symbol.object.token.value .. ":" .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression ) )
	end
	if symbol.is_call then
		return line( symbol.namespace.token.value  .. "." .. symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression ) )
	end
	if symbol.is_assign_local then
		return line( symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_signal_call then
		return line( "for i = 1, #__" .. symbol.name.token.value .. " do", 1 )
			.. line( "__" .. symbol.name.token.value .. "[i]" .. call_arguments( symbol.arguments, transform_expression ), -1 )
			.. line( "end" )
	end
end

local function transform_identifier_type_pair( pair )
	return pair.name.token.value
end

local function transform_class_statement( symbol )
	if symbol.is_method_definition then
		return line( "function this." .. filter_method_name( symbol.name.token.value ) .. arguments( symbol.arguments, transform_identifier_type_pair ), 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "end" )
	end
	if symbol.is_signal then
		return line( "function this." .. std_to_lcamel( symbol.name.token.value ) .. "( this, func )", 1 )
			.. line( "__" .. symbol.name.token.value .. "[#__" .. symbol.name.token.value .. " + 1] = func", -1 )
			.. line( "end" )
	end
end

local function transform_signal( signal )
	return line( "local __" .. signal.name.token.value .. " = {}" )
end

local function transform_module( symbol )
	if symbol.is_class then
		local namespace = camel_to_std( symbol.namespace.token.value )
		local signals = list_signals( symbol.statements )
		local properties = list_properties( symbol.statements )

		local str =
			   line( namespace .. " = " .. namespace .. " or {}" )
			.. line( "function " .. namespace .. "." .. symbol.name.token.value .. "( ... )", 1 )
			.. line( "local this = {}" )
			.. line( "" )
			.. list_as_string( signals, transform_signal )
			.. line( "" )
			.. list_as_string( symbol.statements, transform_class_statement, "\n" )
			.. line( "" )
			.. line( "local mt = {}" )
			.. line( "setmetatable( this, mt )" )
			.. line( "" )
			.. line( "function mt.__index( this, __key )", 1 )

		for _, property in ipairs( properties ) do
			if property.get_statements then
				str = str
					.. line( "if __key == \"" .. property.name.token.value .. "\" then", 1 )
					.. list_as_string( property.get_statements, transform_statement )
					.. line( nil, -1 )
					.. line( "end" )
			end
		end
		
		str = str
			.. line( "assert( false, \"Member '\" .. __key .. \"' does not exist.\" )" )
			.. line( nil, -1 )
			.. line( "end" )
			.. line( "" )
			.. line( "function mt.__newindex( this, __key, value )", 1 )
			
		for _, property in ipairs( properties ) do
			if property.set_statements then
				str = str
					.. line( "if __key == \"" .. property.name.token.value .. "\" then", 1 )
					.. list_as_string( property.set_statements, transform_statement, "", false, true )
					.. line( "return" )
					.. line( nil, -1 )
					.. line( "end" )
			end
		end

		str = str
			.. line( "rawset( this, __key, value )" )
			.. line( nil, -1 )
			.. line( "end" )
			.. line( "" )
			.. line( "this.construct( this, ... )" )
			.. line( "return this" )
			.. line( nil, -1 )
			.. line( "end" )
			.. line( "" )
		
		return str
	end
	if symbol.is_interface then
		return ""
	end
	assert( false, "Unknown module statement" )
end

function generate_class_lua( class, classes )
	return transform_module( class )
end
