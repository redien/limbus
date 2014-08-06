
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

local function filter_method_name( name )
	return name:gsub( "(.+)%?$", "is_%1" ):gsub( "%!$", "" )
end

local function arguments(arguments, func)
	return "(self" .. list_as_string( arguments, func, ", ", true ) .. "):"
end

local function call_arguments(arguments, func)
	return "(" .. list_as_string( arguments, func, ", " ) .. ")"
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
	if op == "Is" then return "is" end
end

local function transform_expression( symbol )
	if symbol.is_literal then
		if symbol.token.value == "false" then return "False" end
		if symbol.token.value == "true" then return "True" end
		return symbol.token.value
	end
	if symbol.is_identifier then
		if symbol.token.value == "this" then return "self" end
		return symbol.token.value
	end
	if symbol.is_infix_op then
		return "(" .. transform_expression( symbol.left ) .. " " .. transform_operator( symbol.op ) .. " " .. transform_expression( symbol.right ) .. ")"
	end
	if symbol.is_enum then
		return symbol.namespace.token.value .. "." .. camel_to_std( filter_enum( symbol.identifier.token.value ) ):upper()
	end
	if symbol.is_member then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "self." end
		return prefix .. symbol.name.token.value
	end
	if symbol.is_constant then
		return symbol.namespace.token.value .. "." .. camel_to_std( filter_enum( symbol.name.token.value ) ):upper()
	end
	if symbol.is_method_call then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "self." end
		return prefix .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_call then
		return symbol.namespace.token.value .. "." .. symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_new then
		return symbol.type.namespace .. "." .. symbol.type.class .. call_arguments( symbol.arguments, transform_expression )
	end
	assert( false, "Unknown expression" )
end

local function transform_statement( symbol )
	if symbol.is_return then
		return line( "return " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_if then
		return line( "if " .. transform_expression( symbol.expression ) .. ":", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
	end
	if symbol.is_assert then
		local expr = transform_expression( symbol.expression )
		return line( "assert(" .. expr .. ")" )
	end
	if symbol.is_while then
		return line( "while " .. transform_expression( symbol.expression ) .. ":", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
	end
	if symbol.is_assign_member then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "self." end
		return line( prefix .. symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_method_call then
		local prefix = symbol.object.token.value .. "."
		if prefix == "this." then prefix = "self." end
		return line( prefix .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression ) )
	end
	if symbol.is_call then
		return line( symbol.namespace.token.value  .. "." .. symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression ) )
	end
	if symbol.is_assign_local then
		return line( symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) )
	end
	if symbol.is_signal_call then
		return line( "for handler in self." .. symbol.name.token.value .. ":", 1 )
			.. line( "handler" .. call_arguments( symbol.arguments, transform_expression ), -1 )
	end
end

local function transform_identifier_type_pair( pair )
	return pair.name.token.value
end

local function transform_class_statement( symbol )
	if symbol.is_method_definition then
		return line( "def " .. filter_method_name( symbol.name.token.value ) .. arguments( symbol.arguments, transform_identifier_type_pair ), 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
	end
	if symbol.is_property then
		local str, g, s = "", "None", "None"
		if symbol.get_statements then
			g = "get_" .. symbol.name.token.value
			str = str
				.. line( "def get_" .. symbol.name.token.value .. "(self):", 1 )
				.. list_as_string( symbol.get_statements, transform_statement )
				.. line( nil, -1 )
		end
		if symbol.set_statements then
			s = "set_" .. symbol.name.token.value
			str = str
				.. line( "def set_" .. symbol.name.token.value .. "(self, value):", 1 )
				.. list_as_string( symbol.set_statements, transform_statement )
				.. line( nil, -1 )
		end
		
		str = str .. line( "" )
				  .. line( symbol.name.token.value .. " = property(" .. g .. ", " .. s .. ")" )
		
		return str
	end
end

local function transform_signal( signal )
	return line( "self." .. signal.name.token.value .. " = []" )
end

local function transform_module( symbol )
	if symbol.is_class then
		local signals = list_signals( symbol.statements )
		local constructor = find_constructor( symbol.statements )
		return
			   line( "class " .. symbol.name.token.value .. "(object):", 1 )
			.. list_as_string( symbol.statements, transform_class_statement, "\n" )
			.. line( "" )
			.. line( "def __init__" .. arguments( constructor.arguments, transform_identifier_type_pair ), 1 )
			.. list_as_string( signals, transform_signal )
			.. line( "self.construct" .. call_arguments( constructor.arguments, transform_identifier_type_pair ) )
			.. line( nil, -2 )
			.. line( "" )
	end
	if symbol.is_interface then
		return ""
	end
	assert( false, "Unknown module statement" )
end

function generate_class_python( class, classes )
	return transform_module( class )
end
