
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require "common"

local type_to_string_tbl = {
	ptr = "System.IntPtr",
}

local function type_to_s( type )
	if type.basic_type then
		local translated = type_to_string_tbl[type.basic_type]
		if translated then return translated end
		return type.basic_type
	else
		return type.namespace .. "." .. type.class
	end
end

local function filter_method_name( name )
	return std_to_camel( name:gsub( "(.+)%?$", "is_%1" ):gsub( "%!$", "" ) )
end

local function arguments(arguments, func)
	return "(" .. list_as_string( arguments, func, ", " ) .. ")"
end
call_arguments = arguments

local function transform_operator( op )
	if op == "Plus" then return "+" end
	if op == "Minus" then return "-" end
	if op == "Equal" then return "==" end
	if op == "NotEqual" then return "!=" end
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
		return std_to_camel( symbol.namespace.token.value ) .. ".Imported." .. camel_to_std( filter_enum( symbol.identifier.token.value ) ):upper()
	end
	if symbol.is_member then
		return symbol.object.token.value .. "." .. symbol.name.token.value
	end
	if symbol.is_constant then
		return std_to_camel( symbol.namespace.token.value ) .. ".Imported." .. camel_to_std( filter_enum( symbol.name.token.value ) ):upper()
	end
	if symbol.is_method_call then
		return symbol.object.token.value .. "." .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_call then
		return std_to_camel( symbol.namespace.token.value ) .. ".Imported." .. symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_new then
		return "new " .. symbol.type.namespace .. "." .. symbol.type.class .. call_arguments( symbol.arguments, transform_expression )
	end
	assert( false, "Unknown expression" )
end

local function transform_identifier_type_pair( pair )
	return type_to_s( pair.type ) .. " " .. pair.name.token.value
end

local function transform_statement( symbol )
	if symbol.is_return then
		return line( "return " .. transform_expression( symbol.expression ) .. ";" )
	end
	if symbol.is_if then
		return line( "if (" .. transform_expression( symbol.expression ) .. ")" )
			.. line( "{", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "}" )
	end
	if symbol.is_while then
		return line( "while (" .. transform_expression( symbol.expression ) .. ")" )
			.. line( "{", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "}" )
	end
	if symbol.is_local_declaration then
		local str = ""
		for i, item in ipairs( symbol.identifier_type_pairs ) do
			str = str .. line( transform_identifier_type_pair( item ) .. ";" )
		end
		return str
	end
	if symbol.is_assign_member then
		return line( symbol.object.token.value .. "." .. symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) .. ";" )
	end
	if symbol.is_method_call then
		return line( symbol.object.token.value .. "." .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression ) .. ";" )
	end
	if symbol.is_call then
		return line( std_to_camel( symbol.namespace.token.value ) .. ".Imported." .. symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression ) .. ";" )
	end
	if symbol.is_assign_local then
		return line( symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) .. ";" )
	end
	if symbol.is_signal_call then
		return line( "if (" .. std_to_camel( symbol.name.token.value ) .. " != null) " .. std_to_camel( symbol.name.token.value ) .. "(this" .. list_as_string( symbol.arguments, transform_expression, ", ", true ) .. ");" )
	end
end

local function transform_class_statement( symbol, class_name )
	if symbol.is_method_definition then
		local args = arguments( symbol.arguments, transform_identifier_type_pair )

		local str
		if symbol.name.token.value == "construct" then
			str = line( "public " .. class_name .. args )
		else
			if symbol.name.token.value == "destruct" then
				args = "(bool unused)"
			end
			str = line( "public " .. type_to_s( symbol.return_type ) .. " " .. filter_method_name( symbol.name.token.value ) .. args )
		end
		
		return str
			.. line( "{", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "}" )
	end
	if symbol.is_define_member then
		local str = ""
		for _, pair in ipairs( symbol.identifier_type_pairs ) do
			str = str .. line( transform_identifier_type_pair( pair ) .. ";" )
		end
		return str
	end
	if symbol.is_property then
		local str =
			   line( "public " .. type_to_s( symbol.type ) .. " " .. std_to_camel( symbol.name.token.value ) )
			.. line( "{", 1 )

		
		if symbol.get_statements then
			str = str
				.. line( "get" )
				.. line( "{", 1 )
				.. list_as_string( symbol.get_statements, transform_statement )
				.. line( nil, -1 )
				.. line( "}" )
		end
		if symbol.set_statements then
			str = str
				.. line( "set" )
				.. line( "{", 1 )
				.. list_as_string( symbol.set_statements, transform_statement )
				.. line( nil, -1 )
				.. line( "}" )
		end
		
		return str
			.. line( nil, -1 )
			.. line( "}" )
	end
end

local function transform_interface_statement( symbol )
	if symbol.is_method_declaration then
		return line( type_to_s( symbol.return_type ) .. " " .. filter_method_name( symbol.name.token.value ) .. arguments( symbol.arguments, transform_identifier_type_pair ) .. ";" )
	end
end

local function transform_module( symbol )
	if symbol.is_class then
		local interface = ""
		if symbol.interface then
			interface = ", " .. symbol.interface.namespace.token.value .. "." .. symbol.interface.name.token.value
		end
		
		local signals = list_signals( symbol.statements )

		local str =
			   line( "namespace " .. symbol.namespace.token.value )
			.. line( "{", 1 )

		for _, signal in ipairs( signals ) do
			str = str
			.. line( "public delegate void " .. std_to_camel( signal.name.token.value ) .. "Delegate(" .. symbol.name.token.value .. " sender" .. list_as_string( signal.arguments, transform_identifier_type_pair, ", ", true ) .. ");" )
		end
		
		str = str
			.. line( "public class " .. symbol.name.token.value .. " : System.IDisposable" .. interface )
			.. line( "{", 1 )

		for _, signal in ipairs( signals ) do
			str = str
			.. line( "public event " .. std_to_camel( signal.name.token.value ) .. "Delegate " .. std_to_camel( signal.name.token.value ) .. ";" )
		end
		
		str = str
			.. list_as_string( symbol.statements, transform_class_statement, "\n", false, symbol.name.token.value )
			.. line( "" )
			.. line( "public void Dispose()" )
			.. line( "{", 1 )
			.. line( "Destruct();" )
			.. line( "System.GC.SuppressFinalize(this);" )
			.. line( nil, -1 )
			.. line( "}" )
			.. line( "" )
			.. line( "bool disposed = false;" )
			.. line( "public void Destruct()" )
			.. line( "{", 1 )
			.. line( "if (!this.disposed)", 1 )
			.. line( "Destruct(true);", -1 )
			.. line( "disposed = true;", -1 )
			.. line( "}" )
			.. line( "" )
			.. line( "~" .. symbol.name.token.value .. "()" )
			.. line( "{", 1 )
			.. line( "Destruct();", -1 )
			.. line( "}", -1 )
			.. line( "}", -1 )
			.. line( "}" )
			.. line( "" )
		
		return str
	end
	if symbol.is_interface then
		return line( "namespace " .. symbol.namespace.token.value )
			.. line( "{", 1 )
			.. line( "public interface " .. symbol.name.token.value )
			.. line( "{", 1 )
			.. list_as_string( symbol.statements, transform_interface_statement )
			.. line( nil, -1 )
			.. line( "}", -1 )
			.. line( "}" )
			.. line( "" )
	end
	assert( false, "Unknown module statement" )
end

function generate_class_csharp( class, classes )
	return transform_module( class )
end
