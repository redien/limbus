
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

tokenizer = tokenizer or {}

function tokenizer.new( env )
	local self = {}
	local token_types = {}
	env = env or _G

	self.set_keyword_terminal_class = ""
	self.str = nil

	function self:add_token_type( id, as_string, pattern )
		if id then env[id] = #token_types + 1 end
		token_types[#token_types + 1] = {pattern, id, as_string}
	end

	function self:token_type_as_string( id )
		return token_types[id][3]
	end

	function self:set_keyword_terminal_class( class )
		self.keyword_terminal_class = class
	end

	function self:add_keyword( id, pattern )
		self:add_token_type( id, pattern, "(" .. pattern .. ")" .. self.keyword_terminal_class )
		self:add_token_type( id, pattern, "(" .. pattern .. ")$" )
	end

	function self:skip_token_type( pattern )
		self:add_token_type( nil, nil, pattern )
	end

	local function for_all_newlines_before( offset, to_do )
		local i = 1
		while true do
			local _, newline = self.str:find( "\n", i, true )
			if not newline then break end
			i = newline + 1
			if i <= offset then
				to_do( newline )
			else
				break
			end
		end
	end

	local function line_at( offset )
		local count = 1
		for_all_newlines_before( offset, function() count = count + 1 end )
		return count
	end

	local function column_at( offset )
		local last_newline = 0
		for_all_newlines_before( offset, function( newline ) last_newline = newline end )
		return offset - last_newline
	end

	function self:line_of_token( token )
		return line_at( token.token_start )
	end

	function self:column_of_token( token )
		return column_at( token.token_start )
	end

	function self:tokenize( str )
		local tokens = {}
		local string_offset = 1
		self.str = str

		local function add_token( pattern, id )
			local token_start, token_end, match

			if type( pattern ) == "string" then
				token_start, token_end, match = str:find( "^" .. pattern, string_offset )
				if match then
					token_end = token_start + match:len() - 1
				end
			else
				token_start, token_end = pattern( id, str, string_offset )
			end

			if token_start and token_end then
				if id then
					tokens[#tokens + 1] = {
						token_start = token_start,
						token_end = token_end,
						id = id,
						value = str:sub( token_start, token_end ),
					}
				end
				string_offset = token_end + 1
				return true
			end
		end

		local recognized_token
		while string_offset <= #str do
			recognized_token = false
			for _, token_type in ipairs( token_types ) do
				if token_type[2] then
					if add_token( token_type[1], env[token_type[2]] ) then
						recognized_token = true
						break
					end
				else
					if add_token( token_type[1] ) then
						recognized_token = true
						break
					end
				end
			end

			if not recognized_token then
				error( "Did not recognize token starting at line "
					.. line_at( string_offset )
					.. ", column "
					.. column_at( string_offset ) )
			end
		end

		return tokens
	end

	return self
end

function tokenizer.test()
	local token_ids = {}
	local tokenizer = tokenizer.new( token_ids )
	tokenizer:set_keyword_terminal_class( "[^%w_]" )

	tokenizer:skip_token_type( "%s+" )
	tokenizer:add_token_type( "T_STRANGE_PREFIX_OP", "((", "%(%(" )
	tokenizer:add_token_type( "T_BEGIN_PARENT", "(", "%(" )
	tokenizer:add_token_type( "T_END_PARENT", ")", "%)" )
	tokenizer:add_token_type( "T_QUOTE", "'", "%'" )
	tokenizer:add_token_type( "T_COMMA", ",", "%," )
	tokenizer:add_keyword( "T_IF",   "if" )
	tokenizer:add_keyword( "T_THEN", "then" )
	tokenizer:add_keyword( "T_END",  "end" )
	tokenizer:add_keyword( "T_TRUE", "true" )
	tokenizer:add_token_type( "T_NUMBER", "<number>", "%d*%.?%d+" )
	tokenizer:add_token_type( "T_IDENTIFIER", "<identifier>", "[%a_][%w_]*" )

	assert( token_ids.T_STRANGE_PREFIX_OP )
	assert( token_ids.T_IF )
	assert( token_ids.T_THEN )
	assert( token_ids.T_END )
	assert( token_ids.T_TRUE )
	assert( token_ids.T_NUMBER )
	assert( token_ids.T_IDENTIFIER )

	local tokens = tokenizer:tokenize( "if ((true then end_world('2', 3) end" )

	assert( #tokens == 13 )
	assert( tokens[1].id == token_ids.T_IF )
	assert( tokens[2].id == token_ids.T_STRANGE_PREFIX_OP )
	assert( tokens[3].id == token_ids.T_TRUE )
	assert( tokens[4].id == token_ids.T_THEN )
	assert( tokens[5].id == token_ids.T_IDENTIFIER )
	assert( tokens[6].id == token_ids.T_BEGIN_PARENT )
	assert( tokens[7].id == token_ids.T_QUOTE )
	assert( tokens[8].id == token_ids.T_NUMBER )
	assert( tokens[9].id == token_ids.T_QUOTE )
	assert( tokens[10].id == token_ids.T_COMMA )
	assert( tokens[11].id == token_ids.T_NUMBER )
	assert( tokens[12].id == token_ids.T_END_PARENT )
	assert( tokens[13].id == token_ids.T_END )

	assert( tokens[1].value == "if" )
	assert( tokens[2].value == "((" )
	assert( tokens[3].value == "true" )
	assert( tokens[4].value == "then" )
	assert( tokens[5].value == "end_world" )
	assert( tokens[6].value == "(" )
	assert( tokens[7].value == "'" )
	assert( tokens[8].value == "2" )
	assert( tokens[9].value == "'" )
	assert( tokens[10].value == "," )
	assert( tokens[11].value == "3" )
	assert( tokens[12].value == ")" )
	assert( tokens[13].value == "end" )
end
