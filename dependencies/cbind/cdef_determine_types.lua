
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require "lparse.match"

local function each( list, func )
	for _, item in ipairs( list ) do
		func( item )
	end
end

local function types_are_equal( a, b )
	if a.basic_type == "n/a" or b.basic_type == "n/a" then return true end

	if a.basic_type then
		if a.basic_type == b.basic_type then
			return true
		else
			return false
		end
	end
	
	if a.namespace == b.namespace and a.class == b.class then
		return true
	end
	return false
end

local function expression( symbol )
	if symbol.is_infix_op then
		expression( symbol.left )
		expression( symbol.right )
		
		if not types_are_equal( symbol.left.type, symbol.right.type ) then
			error( "Type-missmatch for operator '" .. symbol.op .. "'" )
		end

		if symbol.op == "Plus" or symbol.op == "Minus" then
			symbol.type = symbol.left.type
		end

		if symbol.op == "LessThan" or symbol.op == "GreaterThan"
		or symbol.op == "LessThanEqual" or symbol.op == "GreaterThanEqual"
		or symbol.op == "Equal" or symbol.op == "NotEqual" then
			symbol.type = {basic_type = "bool"}
		end
	end
end

local function statement( symbol )
	if symbol.is_return then
		expression( symbol.expression )
	end
	if symbol.is_if then
		expression( symbol.expression )
		each( symbol.statements, statement )
	end
	if symbol.is_assert then
		expression( symbol.expression )
	end
	if symbol.is_while then
		expression( symbol.expression )
		each( symbol.statements, statement )
	end
	if symbol.is_assign_member then
		expression( symbol.expression )
	end
	if symbol.is_method_call then
		each( symbol.arguments, expression )
	end
	if symbol.is_call then
		each( symbol.arguments, expression )
	end
	if symbol.is_assign_local then
		expression( symbol.expression )
	end
	if symbol.is_signal_call then
		each( symbol.arguments, expression )
	end
end

local function class_statement( symbol )
	if symbol.is_method_definition then
		each( symbol.statements, statement )
	end
	if symbol.is_property then
		if get then each( symbol.get_statements, statement ) end
		if set then each( symbol.set_statements, statement ) end
	end
end

function cdef_determine_types( symbol )
	if symbol.is_class then
		each( symbol.statements, class_statement )
	end
end
