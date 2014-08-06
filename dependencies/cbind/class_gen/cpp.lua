
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

local type_to_string_tbl = {
	ptr = "void*",
	uint = "unsigned int",
	string = "const char*",
}

local function type_to_s( type )
	if type.basic_type then
		local translated = type_to_string_tbl[type.basic_type]
		if translated then return translated end
		return type.basic_type
	else
		return type.namespace .. "::" .. type.class .. "*"
	end
end

local function filter_method_name( name )
	return std_to_lcamel( name:gsub( "(.+)%?$", "is_%1" ):gsub( "%!$", "" ) )
end

local function arguments(arguments, func)
	if #arguments > 0 then
		return "( " .. list_as_string( arguments, func, ", " ) .. " )"
	else
		return "()"
	end
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
		return symbol.identifier.token.value
	end
	if symbol.is_member then
		return symbol.object.token.value .. "->" .. symbol.name.token.value
	end
	if symbol.is_constant then
		return symbol.name.token.value
	end
	if symbol.is_method_call then
		return symbol.object.token.value .. "->" .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_call then
		return symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression )
	end
	if symbol.is_new then
		return "new " .. symbol.type.namespace .. "::" .. symbol.type.class .. call_arguments( symbol.arguments, transform_expression )
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
	if symbol.is_assert then
		local expr = transform_expression( symbol.expression )
		return line( "assert( " .. expr .. " );" )
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
		return line( symbol.object.token.value .. "->" .. symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) .. ";" )
	end
	if symbol.is_method_call then
		return line( symbol.object.token.value .. "->" .. filter_method_name( symbol.name.token.value ) .. call_arguments( symbol.arguments, transform_expression ) .. ";" )
	end
	if symbol.is_call then
		return line( symbol.name.token.value .. call_arguments( symbol.arguments, transform_expression ) .. ";" )
	end
	if symbol.is_assign_local then
		return line( symbol.name.token.value .. " = " .. transform_expression( symbol.expression ) .. ";" )
	end
	if symbol.is_signal_call then
		return line( "{", 1 )
			.. line( "size_t size = m_handlers.size();" )
			.. line( "for (size_t i = 0; i < size; ++i)" )
			.. line( "{", 1 )
			.. line( "m_handlers[i]->" .. std_to_lcamel( symbol.name.token.value ) .. "( *this" .. list_as_string( symbol.arguments, transform_expression, ", ", true ) .. " );" )
			.. line( nil, -1 )
			.. line( "}", -1 )
			.. line( "}" )
	end
	if symbol.is_delete then
		if symbol.member then
			return line( "delete " .. symbol.object.token.value .. "->" .. symbol.member.token.value .. ";" )
		else
			return line( "delete " .. symbol.object.token.value .. ";" )
		end
	end
end

local function method_declaration( symbol, class_name, is_source )
	local args = arguments( symbol.arguments, transform_identifier_type_pair )

	local str = ""
	if is_source then str = class_name .. "::" end
	
	if symbol.name.token.value == "construct" then
		str = str .. class_name .. args
	elseif symbol.name.token.value == "destruct" then
		str = str .. "~" .. class_name .. "()"
	else
		str = type_to_s( symbol.return_type ) .. " " .. str .. filter_method_name( symbol.name.token.value ) .. args
	end
	
	return str
end

local function transform_class_statement( symbol, class_name )
	if symbol.is_method_definition then
		return line( method_declaration( symbol, class_name, true ) )
			.. line( "{", 1 )
			.. list_as_string( symbol.statements, transform_statement )
			.. line( nil, -1 )
			.. line( "}" )
	end
	if symbol.is_property then
		local str = ""
		
		if symbol.get_statements then
			str = str
				.. line( type_to_s( symbol.type ) .. " " .. class_name .. "::get" .. std_to_camel( symbol.name.token.value ) .. "()" )
				.. line( "{", 1 )
				.. list_as_string( symbol.get_statements, transform_statement )
				.. line( nil, -1 )
				.. line( "}" )
		end
		if symbol.set_statements then
			str = str
				.. line( "void " .. class_name .. "::set" .. std_to_camel( symbol.name.token.value ) .. "( " .. type_to_s( symbol.type ) .. " value )" )
				.. line( "{", 1 )
				.. list_as_string( symbol.set_statements, transform_statement )
				.. line( nil, -1 )
				.. line( "}" )
		end
		
		return str
	end
end

local function transform_interface_statement( symbol )
	if symbol.is_method_declaration then
		return line( "virtual " .. type_to_s( symbol.return_type ) .. " " .. filter_method_name( symbol.name.token.value ) .. arguments( symbol.arguments, transform_identifier_type_pair ) .. " = 0;" )
	end
end

local function transform_module( symbol, referenced_classes, c_source_files )
	if symbol.is_class then
		local interface = ""
		if symbol.interface then
			interface = " : public " .. symbol.interface.namespace.token.value .. "::" .. symbol.interface.name.token.value
		end
		
		local signals = list_signals( symbol.statements )
		local member_declarations = list_class_statements( symbol.statements, "is_define_member" )
		local method_declarations = list_class_statements( symbol.statements, "is_method_definition" )
		local properties = list_properties( symbol.statements )

		local header =
			   line( "#ifndef " .. symbol.namespace.token.value:upper() .. "_" .. symbol.name.token.value:upper() .. "_HPP" )
			.. line( "#define " .. symbol.namespace.token.value:upper() .. "_" .. symbol.name.token.value:upper() .. "_HPP" )
			.. line( "" )
		
		if #signals > 0 then
			header = header
				.. line( "#include <vector>" )
				.. line( "" )
		end
		
		if #c_source_files > 0 then
			header = header
				.. line( "extern \"C\"" )
				.. line( "{", 1 )

			for _, file in ipairs( c_source_files ) do
				header = header
					.. line( "#include <" .. file .. ">" )
			end

			header = header
				.. line( nil, -1 )
				.. line( "}" )
		end
		
		for _, reference in pairs(referenced_classes) do
			header = header
				.. line( "#include <" .. camel_to_std(reference.namespace) .. "/" .. reference.class .. ".hpp>" )
		end

		header = header
			.. line( "" )
		
		header = header
			.. line( "namespace " .. symbol.namespace.token.value )
			.. line( "{", 1 )

		header = header
			.. line( "class " .. symbol.name.token.value .. interface )
			.. line( "{" )
			.. line( "public:", 1 )

		if #signals > 0 then
			header = header
				.. line( "struct EventHandler" )
				.. line( "{", 1 )

			for _, signal in ipairs( signals ) do
				header = header
				.. line( "virtual void " .. std_to_lcamel( signal.name.token.value ) .. "( " .. symbol.name.token.value .. "& sender" .. list_as_string( signal.arguments, transform_identifier_type_pair, ", ", true ) .. " ) {}" )
			end

			header = header
				.. line( nil, -1 )
				.. line( "};" )
				.. line( "" )
				.. line( "void addEventHandler( EventHandler* handler );" )
				.. line( "" )
		end
		
		for _, method in ipairs( method_declarations ) do
			header = header
				.. line( method_declaration( method, symbol.name.token.value ) .. ";" )
		end

		header = header
			.. line( "" )
		
		for _, property in ipairs( properties ) do
			if property.get_statements then
				header = header
					.. line( type_to_s( property.type ) .. " get" .. std_to_camel( property.name.token.value ) .. "();" )
			end
			if property.set_statements then
				header = header
					.. line( "void set" .. std_to_camel( property.name.token.value ) .. "( " .. type_to_s( property.type ) .. " value );" )
			end
		end

		header = header
			.. line( "", -1 )
			.. line( "private:", 1 )
			.. line( symbol.name.token.value .. "( const " .. symbol.name.token.value .. "& other );" )
			.. line( symbol.name.token.value .. "& operator = ( const " .. symbol.name.token.value .. "& other );" )
		
		for _, member in ipairs( member_declarations ) do
			for _, pair in ipairs( member.identifier_type_pairs ) do
				header = header .. line( transform_identifier_type_pair( pair ) .. ";" )
			end
		end

		if #signals > 0 then
			header = header
				.. line( "std::vector< EventHandler* > m_handlers;" )
		end

		header = header
			.. line( nil, -1 )
			.. line( "};", -1 )
			.. line( "}" )
			.. line( "" )
			.. line( "#endif" )
			.. line( "" )


		local source =
			   line( "#include <cassert>" )
			.. line( "#include <" .. camel_to_std(symbol.namespace.token.value) .. "/" .. symbol.name.token.value .. ".hpp>" )
		
		local class_prefix = symbol.name.token.value .. "::"
		
		source = source
			.. line( "" )
			.. line( "namespace " .. symbol.namespace.token.value )
			.. line( "{", 1 )
			.. line( class_prefix .. symbol.name.token.value .. "( const " .. symbol.namespace.token.value .. "::" .. symbol.name.token.value .. "& other ) {}" )
			.. line( symbol.name.token.value .. "& " .. class_prefix .. "operator = ( const " .. symbol.name.token.value .. "& other ) { return *this; }" )
			.. line( "" )
			
		if #signals > 0 then
			source = source
				.. line( "void " .. class_prefix .. "addEventHandler( " .. symbol.name.token.value .. "::EventHandler* handler )" )
				.. line( "{", 1 )
				.. line( "m_handlers.push_back( handler );", -1 )
				.. line( "}" )
				.. line( "" )
		end

		source = source
			.. list_as_string( symbol.statements, transform_class_statement, "\n", false, symbol.name.token.value )
			.. line( nil, -1 )
			.. line( "}" )
			.. line( "" )
			
		return header, source
	end
	if symbol.is_interface then
		local header =
			   line( "#ifndef " .. symbol.namespace.token.value:upper() .. "_" .. symbol.name.token.value:upper() .. "_HPP" )
			.. line( "#define " .. symbol.namespace.token.value:upper() .. "_" .. symbol.name.token.value:upper() .. "_HPP" )
			.. line( "" )

		for _, reference in pairs(referenced_classes) do
			header = header
				.. line( "#include <" .. camel_to_std(reference.namespace) .. "/" .. reference.class .. ".hpp>" )
		end
		
		header = header
			.. line( "" )
			.. line( "namespace " .. symbol.namespace.token.value )
			.. line( "{", 1 )
			.. line( "class " .. symbol.name.token.value )
			.. line( "{" )
			.. line( "public:", 1 )
			.. list_as_string( symbol.statements, transform_interface_statement )
			.. line( nil, -1 )
			.. line( "};", -1 )
			.. line( "}" )
			.. line( "" )
			.. line( "#endif" )
			.. line( "" )
		return header, ""
	end
	assert( false, "Unknown module statement" )
end

function generate_class_cpp( class, referenced_classes, c_source_files )
	return transform_module( class, referenced_classes, c_source_files )
end
