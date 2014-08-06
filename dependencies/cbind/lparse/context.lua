
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

context = context or {}

function context.new()
	local self = {}
	self.scope = {symbols = {}}

	function self:find_symbol_by_value( value )
		local found
		local current_scope = self.scope

		while true do
			if current_scope.symbols[value] then
				found = true
				break
			end

			if not current_scope.parent then break end

			current_scope = current_scope.parent
		end

		if found then
			return current_scope.symbols[value]
		end
	end

	function self:define_symbol( symbol )
		if not self.scope.symbols[symbol.value] then
			self.scope.symbols[symbol.value] = symbol
			symbol.is_defined = true
			return symbol
		else
			return nil
		end
	end

	function self:push_scope()
		self.scope = {parent = self.scope, symbols = {}}
	end

	function self:pop_scope()
		self.scope = self.scope.parent
	end
	
	return self
end

function context.test()
	local con = context.new()
	
	local symbol1 = {value = "1"}
	
	assert( not con:find_symbol_by_value( symbol1.value ) )
	assert( con:define_symbol( symbol1 ) )
	assert( symbol1.is_defined )
	assert( not con:define_symbol( symbol1 ) )
	assert( con:find_symbol_by_value( symbol1.value ) )
	assert( con:find_symbol_by_value( symbol1.value ).value == symbol1.value )
	
	local symbol2 = {value = "2"}
	con:push_scope()
	assert( con:find_symbol_by_value( symbol1.value ) )
	assert( not con:find_symbol_by_value( symbol2.value ) )
	assert( con:define_symbol( symbol1 ) )
	assert( con:define_symbol( symbol2 ) )
	assert( symbol2.is_defined )
	assert( not con:define_symbol( symbol2 ) )
	assert( con:find_symbol_by_value( symbol2.value ) )
	assert( con:find_symbol_by_value( symbol2.value ).value == symbol2.value )
	
	con:pop_scope()
	assert( con:find_symbol_by_value( symbol1.value ) )
	assert( not con:find_symbol_by_value( symbol2.value ) )
end

