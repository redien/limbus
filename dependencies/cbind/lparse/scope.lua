
-- cbind - A library for generating bindings.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

function add_scope_to( parser )
	parser.scope = {symbols = {}}
	
	function parser:find_symbol_in_scope( name, current_scope )
		local found
		if not current_scope then current_scope = self.scope end

		while true do
			if current_scope.symbols[name] then
				found = true
				break
			end

			if not current_scope.parent then break end

			current_scope = current_scope.parent
		end

		if found then
			return current_scope.symbols[name]
		end
	end

	function parser:define_symbol_in_scope( symbol, symbol_id )
		local scope = self.scope

		if not scope.symbols[symbol_id] then
			scope.symbols[symbol_id] = symbol
			return symbol
		else
			error( "Symbol already defined " .. symbol_id )
		end
	end

	function parser:push_scope()
		self.scope = {parent = self.scope, symbols = {}}
		return self.scope
	end

	function parser:pop_scope()
		self.scope = self.scope.parent
	end
end
