
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require "common"

local signals, constructor

local function arguments(arguments, func)
	if #arguments > 0 then
		return "(" .. list_as_string( arguments, func, ", " ) .. ")"
	else
		return ""
	end
end

local function transform_operator( op )
	if op == "Plus" then return "+" end
	if op == "Minus" then return "-" end
	if op == "Equal" then return "==" end
	if op == "NotEqual" then return "!=" end
	if op == "LessThan" then return "<" end
	if op == "GreaterThan" then return ">" end
	if op == "LessThanEqual" then return "<=" end
	if op == "GreaterThanEqual" then return ">=" end
	if op == "Is" then end
end

local function transform_expression( symbol )
	if symbol.is_literal then
		return symbol.token.value
	end
	if symbol.is_identifier then
		if symbol.token.value == "this" then return "self" end
		return symbol.token.value
	end
	if symbol.is_infix_op then
		if symbol.op == "Is" then
			return "(" .. transform_expression( symbol.left ) .. ").equal?(" .. transform_expression( symbol.right ) .. ")"
		end
		return "(" .. transform_expression( symbol.left ) .. " " .. transform_operator( symbol.op ) .. " " .. transform_expression( symbol.right ) .. ")"
	end
	if symbol.is_enum then
		return ":" .. camel_to_std( filter_enum( symbol.identifier.token.value:gsub( symbol.enum.token.value, "" ) ) )
	end
	if symbol.is_member then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "@" end
		return prefix .. symbol.name.token.value
	end
	if symbol.is_constant then
		return symbol.namespace.token.value:gsub( "^%l", string.upper ) .. "::" .. filter_enum( symbol.name.token.value )
	end
	if symbol.is_method_call then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "" end
		return prefix .. symbol.name.token.value .. arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_call then
		return symbol.namespace.token.value:gsub( "^%l", string.upper ) .. "." .. symbol.name.token.value .. arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_new then
		return symbol.type.namespace .. "::" .. symbol.type.class .. ".new" .. arguments( symbol.arguments, transform_expression )
	end
	assert( false, "Unknown expression" )
end

local function transform_statement( symbol )
	if symbol.is_return then
		return line( "return " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_if then
		return line( "if " .. transform_expression( symbol.expression ), 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "end" )
	end
	if symbol.is_assert then
		local expr = transform_expression( symbol.expression )
		return line( "raise 'Assertion failed: " .. expr .. "' unless " .. expr )
	end
	if symbol.is_while then
		return line( "while " .. transform_expression( symbol.expression ), 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "end" )
	end
	if symbol.is_assign_member then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "@" end
		return line( prefix .. symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_method_call then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "" end
		return line( prefix .. symbol.name.token.value .. arguments( symbol.arguments, transform_expression ) )
	end
	if symbol.is_call then
		return line( symbol.namespace.token.value:gsub( "^%l", string.upper )  .. "." .. symbol.name.token.value .. arguments( symbol.arguments, transform_expression ) )
	end
	if symbol.is_assign_local then
		return line( symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_signal_call then
		return line( "@" .. symbol.name.token.value .. ".each { |handler| handler.call(" .. list_as_string( symbol.arguments, transform_expression, ", " ) .. ") }" )
	end
end

local function transform_identifier_type_pair( pair )
	return pair.name.token.value
end

local function transform_class_statement( symbol )
	if symbol.is_method_definition then
		if symbol.name.token.value == "construct" then
			constructor = {symbol.arguments, symbol.statements}
		end

		return line( "def " .. symbol.name.token.value .. arguments( symbol.arguments, transform_identifier_type_pair ), 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "end" )
	end
	if symbol.is_property then
		local str = ""
		if symbol.get_statements then
			str = str
				.. line( "def " .. symbol.name.token.value, 1 )
				.. list_as_string( symbol.get_statements, transform_statement )
				.. line( nil, -1 )
				.. line( "end" )
		end
		if symbol.set_statements then
			if symbol.get_statements then str = str .. line( "" ) end
			str = str
				.. line( "def " .. symbol.name.token.value .. "=(value)", 1 )
				.. list_as_string( symbol.set_statements, transform_statement )
				.. line( nil, -1 )
				.. line( "end" )
		end
		return str
	end
	if symbol.is_signal then
		signals[#signals + 1] = {symbol.name, symbol.arguments}
		return line( "def " .. symbol.name.token.value, 1 )
			.. line( "@" .. symbol.name.token.value .. ".push Proc.new", -1 )
			.. line( "end" )
	end
end

local function transform_signal( signal )
	return line( "@" .. signal[1].token.value .. " = []" )
end

local function transform_module( symbol )
	if symbol.is_class then
		return
			   line( "module " .. symbol.namespace.token.value, 1 )
			.. line( "class " .. symbol.name.token.value, 1 )
			.. list_as_string( symbol.statements, transform_class_statement, "\n" )
			.. line( "" )
			.. line( "def initialize" .. arguments( constructor[1], transform_identifier_type_pair ), 1 )
			.. list_as_string( signals, transform_signal )
			.. line( "construct" .. arguments( constructor[1], transform_identifier_type_pair ) )
			.. line( nil, -1 )
			.. line( "end" )
			.. line( nil, -1 )
			.. line( "end" )
			.. line( nil, -1 )
			.. line( "end" )
			.. line( "" )
	end
	if symbol.is_interface then
		return ""
	end
	assert( false, "Unknown module statement" )
end

function generate_class_ruby( class, classes )
	signals = {}
	constructor = nil
	return transform_module( class )
end
