
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

require "lparse.parser"
require "lparse.tokenizer"
require "lparse.expressions"
require "lparse.scope"

cdef_parser = parser:new()

local token_ids = {}
local tokenizer = tokenizer.new( token_ids )
tokenizer:set_keyword_terminal_class( "[^%w_]" )

tokenizer:skip_token_type( "%s+" )
tokenizer:skip_token_type( "%#[^\n]*\n" )
tokenizer:add_token_type( "T_BEGIN_PARENT", "(", "%(" )
tokenizer:add_token_type( "T_END_PARENT", ")", "%)" )
tokenizer:add_token_type( "T_DOT", ".", "%." )
tokenizer:add_token_type( "T_COMMA", ",", "%," )
tokenizer:add_token_type( "T_DOUBLE_COLON", "::", "%:%:" )
tokenizer:add_token_type( "T_COLON", ":", "%:" )
tokenizer:add_token_type( "T_EQUAL_OP", "==", "%=%=" )
tokenizer:add_token_type( "T_LESS_THAN_OP", "<", "%<" )
tokenizer:add_token_type( "T_GREATER_THAN_OP", ">", "%>" )
tokenizer:add_token_type( "T_LESS_THAN_EQUAL_OP", "<=", "%<%=" )
tokenizer:add_token_type( "T_GREATER_THAN_EQUAL_OP", ">=", "%>%=" )
tokenizer:add_token_type( "T_NOT_EQUAL_OP", "!=", "%!%=" )
tokenizer:add_token_type( "T_PLUS_OP", "+", "%+" )
tokenizer:add_token_type( "T_MINUS_OP", "-", "%-" )
tokenizer:add_token_type( "T_ASSIGN", "=", "%=" )
tokenizer:add_keyword( "T_IS_OP",   "is" )
tokenizer:add_keyword( "T_CLASS",   "class" )
tokenizer:add_keyword( "T_INTERFACE",   "interface" )
tokenizer:add_keyword( "T_REQUIRE",   "require" )
tokenizer:add_keyword( "T_DEF",   "def" )
tokenizer:add_keyword( "T_VAR",   "var" )
tokenizer:add_keyword( "T_NEW",   "new" )
tokenizer:add_keyword( "T_DELETE",   "delete" )
tokenizer:add_keyword( "T_SIGNAL",   "signal" )
tokenizer:add_keyword( "T_PROPERTY",   "property" )
tokenizer:add_keyword( "T_GET",   "get" )
tokenizer:add_keyword( "T_SET",   "set" )
tokenizer:add_keyword( "T_IF",   "if" )
tokenizer:add_keyword( "T_THEN", "then" )
tokenizer:add_keyword( "T_END",  "end" )
tokenizer:add_keyword( "T_WHILE",   "while" )
tokenizer:add_keyword( "T_DO",   "do" )
tokenizer:add_keyword( "T_RETURN",   "return" )
tokenizer:add_keyword( "T_TRUE", "true" )
tokenizer:add_keyword( "T_FALSE", "false" )
tokenizer:add_keyword( "T_ASSERT", "assert" )
tokenizer:add_token_type( "T_NUMBER", "<number>", "%d*%.?%d+" )
tokenizer:add_token_type( "T_IDENTIFIER", "<identifier>", "[%a_][%w_]*[%?%!]?" )

-- Make a hashtable of all operator ids
local is_operator = {}
local key = next( token_ids )
while key do
	if key:sub( -2 ) == "OP" then
		is_operator[token_ids[key]] = true
	end
	key = next( token_ids, key )
end

local function position_of_token( tokenizer, token )
	return "at line " .. tokenizer:line_of_token( token )
		.. ", column " .. tokenizer:column_of_token( token )
end

function cdef_parser:token_to_symbol( token )
	if token.id == token_ids.T_BEGIN_PARENT or is_operator[token.id] then
		local symbol = self.defined_symbols[token.id]
		if not symbol then
			error( "Unknown operator '"
				.. tokenizer:token_type_as_string( token.id ) .. "' "
				.. position_of_token( tokenizer, token ) )
		else
			symbol = self:clone_symbol( symbol )
			symbol.token = token
			symbol.is_operator = true
			return symbol
		end
	elseif token.id == token_ids.T_NUMBER or
           token.id == token_ids.T_TRUE or
           token.id == token_ids.T_FALSE then
		local symbol = {
			is_literal = true,
			token = token,
			as_prefix = function( self ) return self end,
			{},
		}

		if token.id == token_ids.T_NUMBER then
			symbol.type = {basic_type = "int"}
		end
		if token.id == token_ids.T_TRUE or token.id == token_ids.T_FALSE then
			symbol.type = {basic_type = "bool"}
		end
		return symbol
	elseif token.id == token_ids.T_IDENTIFIER then
		return {
			is_identifier = true,
			token = token,
			as_prefix = function( self )
				local symbol = cdef_parser:find_symbol_in_scope( token.value )
				if not symbol then
					error( "Symbol '" .. token.value .. "' not defined " .. position_of_token( cdef_parser.tokenizer, token ) )
				end
				self.type = symbol.type
				return self
			end,
			{},
		}
	else
		return {token = token}
	end

	assert( false )
end

add_scope_to( cdef_parser )
add_expression_parser_to( cdef_parser )

local function default_infix( self, left, right )
	return {is_infix_op = true, op = self.operator_name, left = left, right = right}
end

cdef_parser:define_infix( token_ids.T_PLUS_OP,
                          60, default_infix ).operator_name = "Plus"
cdef_parser:define_infix( token_ids.T_MINUS_OP,
                          60, default_infix ).operator_name = "Minus"

cdef_parser:define_infix( token_ids.T_IS_OP,
                          50, default_infix ).operator_name = "Is"
cdef_parser:define_infix( token_ids.T_EQUAL_OP,
                          50, default_infix ).operator_name = "Equal"
cdef_parser:define_infix( token_ids.T_NOT_EQUAL_OP,
                          50, default_infix ).operator_name = "NotEqual"
cdef_parser:define_infix( token_ids.T_LESS_THAN_OP,
                          50, default_infix ).operator_name = "LessThan"
cdef_parser:define_infix( token_ids.T_GREATER_THAN_OP,
                          50, default_infix ).operator_name = "GreaterThan"
cdef_parser:define_infix( token_ids.T_LESS_THAN_EQUAL_OP,
                          50, default_infix ).operator_name = "LessThanEqual"
cdef_parser:define_infix( token_ids.T_GREATER_THAN_EQUAL_OP,
                          50, default_infix ).operator_name = "GreaterThanEqual"

cdef_parser:define_prefix( token_ids.T_BEGIN_PARENT, 0, function( self, expression )
	cdef_parser:advance( token_ids.T_END_PARENT )
	return expression
end)

local function add_reference(type)
	local found = false
	for _, reference in pairs(cdef_parser.referenced_classes) do
		if reference.namespace == type.namespace and reference.class == type.class then
			found = true
			break
		end
	end
	if not found then
		cdef_parser.referenced_classes[#cdef_parser.referenced_classes + 1] = type
	end
end

local function parse_type()
	local namespace = cdef_parser:advance( token_ids.T_IDENTIFIER )
	if cdef_parser:next_symbol_is( token_ids.T_DOUBLE_COLON ) then
		cdef_parser:advance()
		local class = cdef_parser:advance( token_ids.T_IDENTIFIER )
		
		local type = {namespace = namespace.token.value, class = class.token.value}
		add_reference(type)
		return type
	else
		return {basic_type = namespace.token.value}
	end
end

-- function/method call and member variables
cdef_parser:define_special_expression( function()
	cdef_parser:push_state()

	if cdef_parser:next_symbol_is( token_ids.T_NEW ) then
		cdef_parser:advance()
		local type = parse_type()
		cdef_parser:advance( token_ids.T_BEGIN_PARENT )
		local arguments = {}
		if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
			arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, cdef_parser.parse_expression )
		end
		cdef_parser:advance( token_ids.T_END_PARENT )
		cdef_parser:pop_state()
		return {
			is_new = true,
			type = type,
			arguments = arguments
		}

	elseif cdef_parser:next_symbol_is( token_ids.T_IDENTIFIER ) then
		local namespace = cdef_parser:advance()
		if cdef_parser:next_symbol_is( token_ids.T_DOUBLE_COLON ) or
		   cdef_parser:next_symbol_is( token_ids.T_DOT ) then
			local separator = cdef_parser.next_symbol.token.id
			cdef_parser:advance()

			local name = cdef_parser:advance( token_ids.T_IDENTIFIER )

			if cdef_parser:next_symbol_is( token_ids.T_DOUBLE_COLON ) then
				cdef_parser:advance()
				local last_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
				cdef_parser:pop_state()
				return {
					is_enum = true,
					namespace = namespace,
					enum = name,
					identifier = last_name,
					type = {basic_type = "int"},
				}

			elseif cdef_parser:next_symbol_is( token_ids.T_BEGIN_PARENT ) then
				cdef_parser:advance()
				local arguments = {}
				if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
					arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, cdef_parser.parse_expression )
				end
				cdef_parser:advance( token_ids.T_END_PARENT )
				cdef_parser:pop_state()
				if separator == token_ids.T_DOUBLE_COLON then
					return {
						is_call = true,
						namespace = namespace,
						name = name,
						arguments = arguments,
						type = {basic_type = "n/a"},
					}
				else
					local symbol = cdef_parser:find_symbol_in_scope( namespace.token.value )
					if not symbol then
						error( "Symbol '" .. namespace.token.value .. "' not defined " .. position_of_token( cdef_parser.tokenizer, namespace.token ) )
					end
					
					local method
					if namespace.token.value == "this" then
						method = cdef_parser:find_symbol_in_scope( name.token.value )[1]
					end

					return {
						is_method_call = true,
						object = namespace,
						name = name,
						arguments = arguments,
						type = symbol.type,
						method = method,
					}
				end

			elseif separator == token_ids.T_DOT then
				cdef_parser:pop_state()

				local object_symbol = cdef_parser:find_symbol_in_scope( namespace.token.value )
				if not object_symbol then
					error( "Symbol '" .. namespace.token.value .. "' not defined " .. position_of_token( cdef_parser.tokenizer, namespace.token ) )
				end
				
				-- TODO: This assumes that it's not possible to get properties of other objects, i.e. name == "this"
				local symbol = cdef_parser:find_symbol_in_scope( name.token.value )
				if not symbol then
					error( "Symbol '" .. name.token.value .. "' not defined " .. position_of_token( cdef_parser.tokenizer, name.token ) )
				end

				return {
					is_member = true,
					object = namespace,
					name = name,
					object_type = object_symbol.type,
					type = symbol.type,
				}

			elseif separator == token_ids.T_DOUBLE_COLON then
				cdef_parser:pop_state()
				return {
					is_constant = true,
					namespace = namespace,
					name = name,
					type = {basic_type = "int"},
				}
			end
		end
	end

	cdef_parser:revert_state()
end)

local function parse_identifier_type_pair()
	cdef_parser:expect( token_ids.T_IDENTIFIER )
	local name = cdef_parser:advance()
	cdef_parser:advance( token_ids.T_COLON )
	local type = parse_type()
	return {name = name, type = type}
end

local function parse_block()
	local statements = {}
	while not cdef_parser:next_symbol_is( token_ids.T_END ) and
		  not cdef_parser:next_symbol_is( token_ids.T_GET ) and
		  not cdef_parser:next_symbol_is( token_ids.T_SET ) do

		if cdef_parser:next_symbol_is( token_ids.T_RETURN ) then
			cdef_parser:advance()
			local expression = cdef_parser:parse_expression( 0 )
			statements[#statements + 1] = {
				is_return = true,
				expression = expression
			}

		elseif cdef_parser:next_symbol_is( token_ids.T_DELETE ) then
			cdef_parser:advance()
			local object = cdef_parser:advance( token_ids.T_IDENTIFIER )
			if cdef_parser:next_symbol_is( token_ids.T_DOT ) then
				cdef_parser:advance()
				local member = cdef_parser:advance( token_ids.T_IDENTIFIER )
				statements[#statements + 1] = {
					is_delete = true,
					object = object,
					member = member,
				}
			else
				statements[#statements + 1] = {
					is_delete = true,
					object = object,
				}
			end

		elseif cdef_parser:next_symbol_is( token_ids.T_IF ) then
			cdef_parser:advance()
			local expression = cdef_parser:parse_expression( 0 )
			cdef_parser:advance( token_ids.T_THEN )
			local scope = cdef_parser:push_scope()
			local block = parse_block()
			cdef_parser:pop_scope()
			cdef_parser:advance( token_ids.T_END )
			statements[#statements + 1] = {
				is_if = true,
				expression = expression,
				statements = block,
				scope = scope
			}

		elseif cdef_parser:next_symbol_is( token_ids.T_ASSERT ) then
			cdef_parser:advance()
			cdef_parser:advance( token_ids.T_BEGIN_PARENT )
			local expression = cdef_parser:parse_expression( 0 )
			cdef_parser:advance( token_ids.T_END_PARENT )
			statements[#statements + 1] = {
				is_assert = true,
				expression = expression
			}

		elseif cdef_parser:next_symbol_is( token_ids.T_VAR ) then
			cdef_parser:advance()
			local item_list = cdef_parser:parse_separated( {token_ids.T_COMMA}, parse_identifier_type_pair )
			local symbol = {
				is_local_declaration = true,
				identifier_type_pairs = item_list
			}
			for _, item in ipairs( item_list ) do
				cdef_parser:define_symbol_in_scope( item, item.name.token.value )
			end
			statements[#statements + 1] = symbol

		elseif cdef_parser:next_symbol_is( token_ids.T_WHILE ) then
			cdef_parser:advance()
			local expression = cdef_parser:parse_expression( 0 )
			cdef_parser:advance( token_ids.T_DO )
			local scope = cdef_parser:push_scope()
			local block = parse_block()
			cdef_parser:pop_scope()
			cdef_parser:advance( token_ids.T_END )
			statements[#statements + 1] = {
				is_while = true,
				expression = expression,
				statements = block,
				scope = scope
			}

		elseif cdef_parser:next_symbol_is( token_ids.T_IDENTIFIER ) then
			local first_name = cdef_parser:advance()

			if cdef_parser:next_symbol_is( token_ids.T_DOT ) then
				cdef_parser:advance()
				local second_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
				if cdef_parser:next_symbol_is( token_ids.T_ASSIGN ) then
					cdef_parser:advance()
					local expression = cdef_parser:parse_expression()
					statements[#statements + 1] = {
						is_assign_member = true,
						object = first_name,
						name = second_name,
						expression = expression
					}

				elseif cdef_parser:next_symbol_is( token_ids.T_BEGIN_PARENT ) then
					cdef_parser:advance()
					local arguments = {}
					if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
						arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, cdef_parser.parse_expression )
					end
					cdef_parser:advance( token_ids.T_END_PARENT )

					local symbol = cdef_parser:find_symbol_in_scope( first_name.token.value )
					if not symbol then
						error( "Symbol '" .. first_name.token.value .. "' not defined " .. position_of_token( cdef_parser.tokenizer, first_name.token ) )
					end

					local method
					if first_name.token.value == "this" then
						method = cdef_parser:find_symbol_in_scope( second_name.token.value )
						if not method then
							error( "Method '" .. second_name.token.value .. "' not defined "  .. position_of_token( cdef_parser.tokenizer, first_name.token ) )
						end
						method = method[1]
					end

					statements[#statements + 1] = {
						is_method_call = true,
						object = first_name,
						name = second_name,
						arguments = arguments,
						type = symbol.type,
						method = method,
					}
				else
					error( "Expected statement "
						.. position_of_token( cdef_parser.tokenizer,
											  first_name.token ) )
				end

			elseif cdef_parser:next_symbol_is( token_ids.T_DOUBLE_COLON ) then
				cdef_parser:advance()
				local second_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
				cdef_parser:advance( token_ids.T_BEGIN_PARENT )
				local arguments = {}
				if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
					arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, cdef_parser.parse_expression )
				end
				cdef_parser:advance( token_ids.T_END_PARENT )
				statements[#statements + 1] = {
					is_call = true,
					namespace = first_name,
					name = second_name,
					arguments = arguments
				}

			elseif cdef_parser:next_symbol_is( token_ids.T_ASSIGN ) then
				cdef_parser:advance()
				local expression = cdef_parser:parse_expression()
				statements[#statements + 1] = {
					is_assign_local = true,
					name = first_name,
					expression = expression
				}

			elseif cdef_parser:next_symbol_is( token_ids.T_BEGIN_PARENT ) then
				cdef_parser:advance()
				local arguments = {}
				if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
					arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, cdef_parser.parse_expression )
				end
				cdef_parser:advance( token_ids.T_END_PARENT )
				statements[#statements + 1] = {
					is_signal_call = true,
					name = first_name,
					arguments = arguments
				}
			else
				error( "Expected statement "
					.. position_of_token( cdef_parser.tokenizer,
										  first_name.token ) )
			end
		else
			error( "Expected statement "
				.. position_of_token( cdef_parser.tokenizer,
									  cdef_parser.next_symbol.token ) )
		end
	end
	return statements
end

local function parse_method_definition()
	cdef_parser:advance()
	local method_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
	cdef_parser:advance( token_ids.T_BEGIN_PARENT )
	local arguments = {}
	if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
		arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, parse_identifier_type_pair )
	end
	cdef_parser:advance( token_ids.T_END_PARENT )
	cdef_parser:advance( token_ids.T_COLON )
	local return_type = parse_type()
	
	local scope = cdef_parser:push_scope()
	
	for _, argument in ipairs( arguments ) do
		cdef_parser:define_symbol_in_scope( argument, argument.name.token.value )
	end
	
	local statements = parse_block()
	cdef_parser:pop_scope()

	cdef_parser:advance( token_ids.T_END )
	
	local symbol = {
		is_method_definition = true,
		scope = scope,
		name = method_name,
		arguments = arguments,
		return_type = return_type,
		statements = statements
	}
	cdef_parser:define_symbol_in_scope( symbol, method_name.token.value )
	return symbol
end

local function parse_member_definition()
	cdef_parser:advance()
	local identifier_type_pairs = cdef_parser:parse_separated( {token_ids.T_COMMA}, parse_identifier_type_pair )

	local symbol = {
		is_define_member = true,
		identifier_type_pairs = identifier_type_pairs
	}
	for _, identifier in ipairs( identifier_type_pairs ) do
		identifier.is_member = true
		cdef_parser:define_symbol_in_scope( identifier, identifier.name.token.value )
	end
	return symbol
end

local function parse_signal_definition()
	cdef_parser:advance()
	local signal_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
	cdef_parser:advance( token_ids.T_BEGIN_PARENT )
	local arguments = {}
	if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
		arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, parse_identifier_type_pair )
	end
	cdef_parser:advance( token_ids.T_END_PARENT )
	return {
		is_signal = true,
		name = signal_name,
		arguments = arguments
	}
end

local function parse_property_definition()
	cdef_parser:advance()

	local property_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
	cdef_parser:advance( token_ids.T_COLON )
	local type = parse_type()

	local get_block, set_block
	local get_scope, set_scope
	if cdef_parser:next_symbol_is( token_ids.T_GET ) then
		cdef_parser:advance()
		get_scope = cdef_parser:push_scope()
		get_block = parse_block()
		cdef_parser:pop_scope()

		if cdef_parser:next_symbol_is( token_ids.T_SET ) then
			cdef_parser:advance()
			set_scope = cdef_parser:push_scope()
			cdef_parser:define_symbol_in_scope( {type = type}, "value" )
			set_block = parse_block()
			cdef_parser:pop_scope()
		end
	elseif cdef_parser:next_symbol_is( token_ids.T_SET ) then
		cdef_parser:advance()
		set_scope = cdef_parser:push_scope()
		cdef_parser:define_symbol_in_scope( {type = type}, "value" )
		set_block = parse_block()
		cdef_parser:pop_scope()

		if cdef_parser:next_symbol_is( token_ids.T_GET ) then
			cdef_parser:advance()
			get_scope = cdef_parser:push_scope()
			get_block = parse_block()
			cdef_parser:pop_scope()
		end
	end

	cdef_parser:advance( token_ids.T_END )

	return {
		is_property = true,
		type = type,
		name = property_name,
		get_statements = get_block,
		set_statements = set_block,
		get_scope = get_scope,
		set_scope = set_scope
	}
end

local function parse_class_statement()
	if cdef_parser:next_symbol_is( token_ids.T_DEF ) then
		return parse_method_definition()
	elseif cdef_parser:next_symbol_is( token_ids.T_VAR ) then
		return parse_member_definition()
	elseif cdef_parser:next_symbol_is( token_ids.T_SIGNAL ) then
		return parse_signal_definition()
	elseif cdef_parser:next_symbol_is( token_ids.T_PROPERTY ) then
		return parse_property_definition()
	else
		error( "Expected class entry "
			.. position_of_token( cdef_parser.tokenizer,
								  cdef_parser.next_symbol.token ) )
	end
end

local function parse_interface_statement()
	if cdef_parser:next_symbol_is( token_ids.T_DEF ) then
		cdef_parser:advance()
		local method_name = cdef_parser:advance( token_ids.T_IDENTIFIER )
		cdef_parser:advance( token_ids.T_BEGIN_PARENT )
		local arguments = {}
		if not cdef_parser:next_symbol_is( token_ids.T_END_PARENT ) then
			arguments = cdef_parser:parse_separated( {token_ids.T_COMMA}, parse_identifier_type_pair )
		end
		cdef_parser:advance( token_ids.T_END_PARENT )
		cdef_parser:advance( token_ids.T_COLON )
		local return_type = cdef_parser:advance( token_ids.T_IDENTIFIER )
		
		local symbol = {
			is_method_declaration = true,
			name = method_name,
			arguments = arguments,
			return_type = {basic_type = return_type.token.value},
		}
		cdef_parser:define_symbol_in_scope( symbol, method_name.token.value )
		return symbol
	else
		error( "Expected interface entry "
			.. position_of_token( cdef_parser.tokenizer,
								  cdef_parser.next_symbol.token ) )
	end
end

local function parse_module_statement()
	if cdef_parser:next_symbol_is( token_ids.T_CLASS ) then
		cdef_parser:advance()
		
		local name_first = cdef_parser:advance( token_ids.T_IDENTIFIER )
		cdef_parser:advance( token_ids.T_DOUBLE_COLON )
		local name_second = cdef_parser:advance( token_ids.T_IDENTIFIER )
		
		local interface
		if cdef_parser:next_symbol_is( token_ids.T_COLON ) then
			cdef_parser:advance()
			local namespace = cdef_parser:advance( token_ids.T_IDENTIFIER )
			cdef_parser:advance( token_ids.T_DOUBLE_COLON )
			interface = {name = cdef_parser:advance( token_ids.T_IDENTIFIER ), namespace = namespace}
			add_reference({class = interface.name.token.value, namespace = interface.namespace.token.value})
		end

		local scope = cdef_parser:push_scope()
		cdef_parser:define_symbol_in_scope( {type = {namespace = name_first.token.value, class = name_second.token.value}}, "this" )

		local statements = {}
		while not cdef_parser:next_symbol_is( token_ids.T_END ) do
			statements[#statements + 1] = parse_class_statement()
		end
		cdef_parser:advance( token_ids.T_END )
		
		cdef_parser:pop_scope()
		
		local symbol = {
			is_class = true,
			scope = scope,
			namespace = name_first,
			name = name_second,
			statements = statements,
			interface = interface,
		}
		cdef_parser:define_symbol_in_scope( symbol, name_second.token.value )
		return symbol

	elseif cdef_parser:next_symbol_is( token_ids.T_INTERFACE ) then
		cdef_parser:advance()
		
		local name_first = cdef_parser:advance( token_ids.T_IDENTIFIER )
		cdef_parser:advance( token_ids.T_DOUBLE_COLON )
		local name_second = cdef_parser:advance( token_ids.T_IDENTIFIER )

		local scope = cdef_parser:push_scope()

		local statements = {}
		while not cdef_parser:next_symbol_is( token_ids.T_END ) do
			statements[#statements + 1] = parse_interface_statement()
		end
		cdef_parser:advance( token_ids.T_END )

		cdef_parser:pop_scope()
		
		local symbol = {
			is_interface = true,
			namespace = name_first,
			name = name_second,
			statements = statements,
			scope = scope
		}
		cdef_parser:define_symbol_in_scope( symbol, name_second.token.value )
		return symbol

	elseif cdef_parser:next_symbol_is( token_ids.T_REQUIRE ) then
		error( "'require' statements must precede any class definitions "
			.. position_of_token( tokenizer, cdef_parser.next_symbol.token ) )
	else
		error( "Statement is not a class definition, starting with '"
			.. tokenizer:token_type_as_string( cdef_parser.next_symbol.token.id ) .. "' "
			.. position_of_token( tokenizer, cdef_parser.next_symbol.token ) )
	end
end

require "cdef_determine_types"
require "cdef_error_checking"

function cdef_parser:parse( s )
	cdef_parser.referenced_classes = {}
	local tokens = tokenizer:tokenize( s )
	cdef_parser:initialize( tokenizer, tokens )
	local cdef_module = parse_module_statement()
	cdef_determine_types( cdef_module )
	cdef_error_checking( cdef_module )
	return cdef_module, cdef_parser.referenced_classes
end
