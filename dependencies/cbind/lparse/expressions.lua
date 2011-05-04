
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

function add_expression_parser_to( parser )

	local special_expressions = {}
	function parser:define_special_expression( special )
		special_expressions[#special_expressions + 1] = special
	end

	function parser:parse_expression( right_binding_power )
		local left
		local symbol = parser.next_symbol
		right_binding_power = right_binding_power or 0

		local result
		for _, special in ipairs( special_expressions ) do
			result = special( right_binding_power )
		end
	
		if result then
			left = result
		else
			parser:advance()

			if not symbol.as_prefix then
				if symbol.token then
					error( "Symbol "
						.. "'" .. parser.tokenizer:token_type_as_string( symbol.token.id )
						.. "' is not a prefix operator"
						.. " at line " .. parser.tokenizer:line_of_token( symbol.token )
						.. ", column " .. parser.tokenizer:column_of_token( symbol.token ) )
				else
					error( "Symbol '<EOF>' is not a prefix operator" )
				end
			end

			left = symbol:as_prefix()
		end

		while parser.next_symbol.left_binding_power and
			  right_binding_power < parser.next_symbol.left_binding_power do
			symbol = parser.next_symbol
			parser:push_state()
			parser:advance()
			local result = symbol:as_infix( left )
			if result then
				left = result
				parser:pop_state()
			else
				parser:revert_state()
				break
			end
		end

		return left
	end

	function parser:define_prefix( token_id, right_binding_power, as_prefix )
		local symbol = parser:define_symbol( token_id )
		symbol.as_prefix = function( self )
			return as_prefix( self, parser:parse_expression( right_binding_power ) )
		end
		return symbol
	end

	function parser:define_infix( token_id, left_binding_power, as_infix )
		local symbol = parser:define_symbol( token_id )
		symbol.left_binding_power = left_binding_power
		symbol.as_infix = function( self, left )
			return as_infix( self, left, parser:parse_expression( left_binding_power ) )
		end
		return symbol
	end

	function parser:define_infix_right( token_id, left_binding_power, as_infix )
		local symbol = parser:define_symbol( token_id )
		symbol.left_binding_power = left_binding_power
		symbol.as_infix = as_infix or function( self, left )
			return as_infix( self, left, parser:parse_expression( left_binding_power - 1 ) )
		end
		return symbol
	end
end

