
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

parser = parser or {}

function parser:create_symbol( id )
	local self = {}
	self.id = id
	return self
end

function parser:clone_symbol( obj )
	local self = {}
	local key = next( obj )
	while key do self[key] = obj[key]; key = next( obj, key ) end
	return self
end

function parser:define_symbol( token_id )
	local symbol = parser:create_symbol()
	self.defined_symbols[token_id] = symbol
	return symbol
end

function parser:next_symbol_is( token_id )
	if self.next_symbol.token and self.next_symbol.token.id == token_id then
		return true
	end
end

function parser:expect( token_id )
	if not self:next_symbol_is( token_id ) then
		local str = "Expected token '" .. self.tokenizer:token_type_as_string( token_id ) .. "', got "
		if self.next_symbol.token then
			str = str .. "'" .. self.tokenizer:token_type_as_string( self.next_symbol.token.id )
					  .. "' at line " .. self.tokenizer:line_of_token( self.next_symbol.token )
					  .. ", column " .. self.tokenizer:column_of_token( self.next_symbol.token )
		else
			str = str .. "'<EOF>'"
		end
		error( str )
	end
end

parser.EOF = {}
function parser:advance( expected )
	local last_symbol = self.next_symbol
	if self.next_symbol and self.next_symbol.id == parser.EOF then
		error( "Unexpected EOF" )
	end
	
	if expected and self.next_symbol then
		self:expect( expected )
	end
	
	if self.token_index > #self.tokens then
		self.next_symbol = {id = parser.EOF}
		return last_symbol
	end

	local token = self.tokens[self.token_index]
	self.token_index = self.token_index + 1

	self.next_symbol = self:token_to_symbol( token )

	return last_symbol
end

function parser:parse_separated( separators, parser, terminal )
	local items = {}
	while true do
		items[#items + 1] = parser( self )

		local is_separator = false
		for _, sep in ipairs( separators ) do
			if self:next_symbol_is( sep ) then is_separator = true end
		end

		if not is_separator then break end
		self:advance()

		if self.next_symbol.id == self.EOF then
			local separators_string = ""
			for i = 1, #separators do
				local sep = separators[i]
				separators_string = separators_string .. self.tokenizer:token_type_as_string( sep )
				if i < #separators then str = str .. " or " end
			end
			error( "Unexpected EOF, expected " .. separators_string )
		end
		-- Allow for an extra separator at the end
		if terminal and self:next_symbol_is( terminal ) then break end
	end

	return items
end

function parser:initialize( tokenizer, tokens )
	self.next_symbol = nil
	self.tokenizer = tokenizer
	self.tokens = tokens
	self.states = {}
	self.token_index = 1
	self:advance()
end

function parser:push_state()
	self.states[#self.states + 1] = {self.next_symbol, self.token_index}
end

function parser:revert_state()
	self.next_symbol = self.states[#self.states][1]
	self.token_index = self.states[#self.states][2]
	self.states[#self.states] = nil
end

function parser:pop_state()
	self.states[#self.states] = nil
end

function parser.new()
	local self = {}
	local key = next( parser, nil )
	while key do self[key] = parser[key]; key = next( parser, key ) end
	self.defined_symbols = {}
	return self
end

function parser.test()
	local tokens = {}
	local token_number = {}
	local token_operator = {}
	local parser = parser.new()
	
	tokens[#tokens + 1] = {id = token_number, value = "1"}
	tokens[#tokens + 1] = {id = token_operator, value = "+"}
	tokens[#tokens + 1] = {id = token_number, value = "2"}
	
	function parser:token_to_symbol( token )
		if token.id == token_number then
			return {is_number = true, token = token}
		elseif token.id == token_operator then
			return {is_operator = true, token = token}
		end

		assert( false )
	end

	local symbol

	local function assert_throws( f, ... )
		local a, b = pcall( f, ... )
		assert( not a, b )
	end
	
	parser:initialize( nil, tokens )
	assert_throws( parser.expect, parser, token_operator )
	symbol = parser:advance()
	assert( symbol )
	assert( symbol.is_number )

	assert_throws( parser.expect, parser, token_number )
	symbol = parser:advance()
	assert( symbol )
	assert( symbol.is_operator )

	symbol = parser:advance()
	assert( symbol )
	assert( symbol.is_number )
	assert( parser.next_symbol.id == parser.EOF )

	parser:initialize( nil, {} )
	assert( parser.next_symbol.id == parser.EOF )
	
	parser:initialize( nil, tokens )
	parser:push_state()
	parser:push_state()
	assert( parser:next_symbol_is( token_number ) )
	parser:revert_state()
	assert( parser:next_symbol_is( token_number ) )
	parser:advance()
	assert( parser:next_symbol_is( token_operator ) )
	parser:revert_state()
	assert( parser:next_symbol_is( token_number ) )
	parser:advance()
	assert( parser:next_symbol_is( token_operator ) )
end

