
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

match = {}
match.Wildcard = {}
match.parameter_count = 0

local Parameter = {}
local mt = {}
function mt.__index( table, key )
	if key == "P" or key == "Parameter" then
		table.parameter_count = table.parameter_count + 1
		return {Parameter, table.parameter_count}
	end
end
setmetatable( match, mt )

local function match_table( case, value, parameters )
	local key = next( case )
	while key do
		if case[key] == match.Wildcard then
		elseif type( case[key] ) == "table" and case[key][1] == Parameter then
			parameters[case[key][2]] = value[key]
			if case[key][2] < parameters.least_index then
				parameters.least_index = case[key][2]
			end
		elseif type( case[key] ) == "table" and type( value[key] ) == "table" then
			if not match_table( case[key], value[key], parameters ) then
				return false
			end
		elseif case[key] ~= value[key] then
			return false
		end
		key = next( case, key )
	end

	return true
end

-- Unpack is moved to table in Lua 5.2
local unpack = unpack or table.unpack

function match.new( cases )
	match.parameter_count = 0
	return function( value )
		if type( value ) == "table" then
			local i = 1
			while i <= #cases do
				if type( cases[i] ) == "table" then
					local parameters = {}
					parameters.least_index = 1000
					if match_table( cases[i], value, parameters ) then
						local tbl, l = {}, parameters.least_index
						while parameters[l] do tbl[#tbl + 1] = parameters[l]; l = l + 1 end
						return cases[i + 1]( unpack( tbl ) )
					end
				end
				i = i + 2
			end
		else
			local i = 1
			while i <= #cases do
				if cases[i] == match.Wildcard then
					return cases[i + 1]()
				else
					if cases[i] == value then
						return cases[i + 1]()
					end
				end
				i = i + 2
			end
		end
	end
end

function match.search( tbl, pattern, func )
	local matcher = match.new( {pattern, func} )
	local iter
	iter = function( tbl )
		matcher( tbl )
		local key = next( tbl )
		while key do
			if type( tbl[key] ) == "table" then
				iter( tbl[key] )
			end
			key = next( tbl, key )
		end
	end
	iter( tbl )
end

local _w = match.Wildcard
local matcher = match.new {
	0,		function() return nil end,
	1,		function() return true end,
	"2",	function() return false end,
	3,		function() return "test" end,
}
assert( matcher( 0 ) == nil )
assert( matcher( 1 ) == true )
assert( matcher( "2" ) == false )
assert( matcher( 3 ) == "test" )

matcher = match.new {
	1,		function() return true end,
	_w, 	function() return false end,
}
assert( matcher( 0 ) == false )
assert( matcher( 1 ) == true )
assert( matcher( 2 ) == false )
assert( matcher( 3 ) == false )

matcher = match.new {
	{},		function() return true end,
	_w,		function() return false end,
}
assert( matcher( 1 ) == false )
assert( matcher( nil ) == false )
assert( matcher( true ) == false )
assert( matcher( "test" ) == false )
assert( matcher( {} ) == true )
assert( matcher( {test = true} ) == true )

matcher = match.new {
	{abc = "123"},	function() return true end,
	_w,				function() return false end,
}
assert( matcher( 1 ) == false )
assert( matcher( nil ) == false )
assert( matcher( true ) == false )
assert( matcher( "test" ) == false )
assert( matcher( {abc = "123"} ) == true )
assert( matcher( {test = true} ) == false )
assert( matcher( {} ) == false )

matcher = match.new {
	{1, 2, 3},		function() return true end,
	_w,				function() return false end,
}
assert( matcher( {abc = 1} ) == false )
assert( matcher( {1, 2} ) == false )
assert( matcher( {1, 2, 3} ) == true )
assert( matcher( {1, 2, 3, 4} ) == true )

matcher = match.new {
	{identifier = true, 1, 2, 3},		function() return true end,
	_w,									function() return false end,
}
assert( matcher( {abc = true} ) == false )
assert( matcher( {identifier = true, 1, 2} ) == false )
assert( matcher( {identifier = true, 1, 2, 3} ) == true )
assert( matcher( {identifier = true, 1, 2, 3, 4} ) == true )

matcher = match.new {
	{a = 2, b = match.P},	function( b ) return b end,
	{a = 3, b = match.P},	function( b ) return "a" end,
	_w,					function() return false end,
}
assert( matcher( {abc = 1} ) == false )
assert( matcher( {1} ) == false )
assert( matcher( 1 ) == false )
assert( matcher( "a" ) == false )
assert( matcher( {a = 3, b = 3} ) == "a" )
assert( matcher( {a = 1, b = 3} ) == false )
assert( matcher( {a = 2, b = 3} ) == 3 )

matcher = match.new {
	{a = match.P, b = match.P},	function( a, b ) return a .. b end,
}
assert( matcher( {a = "a", b = "b"} ) == "ab" )

matcher = match.new {
	{b = match.P, a = match.P},	function( a, b ) return a .. b end,
}
assert( matcher( {a = "a", b = "b"} ) == "ba" )

matcher = match.new {
	{a = {match.P}, b = match.P},	function( a, b ) return a .. b end,
}
assert( matcher( {a = {"a"}, b = "b"} ) == "ab" )

matcher = match.new {
	{a = _w, b = _w, c = 4},	function() return true end,
	_w, 						function() return false end,
}
assert( matcher( {a = "a", b = "b"} ) == false )
assert( matcher( {a = 1, b = 1, c = 3} ) == false )
assert( matcher( {a = 2, b = 1, c = 4} ) == true )


matcher = match.new {
	{a = {}},	function() return true end,
	_w, 		function() return false end,
}
assert( matcher( {} ) == false )
assert( matcher( {b = {}} ) == false )
assert( matcher( {a = {}} ) == true )
assert( matcher( {a = {c = 2}} ) == true )
assert( matcher( {a = {a = 2}} ) == true )

matcher = match.new {
	{a = {b = match.P}},		function( b ) return b end,
}
assert( matcher( {a = {a = 1}} ) == nil )
assert( matcher( {a = {b = 1}, b = 2} ) == 1 )


local found_num
match.search({a=true, 2}, {a=true, match.P}, function(num)
	found_num = num
end)
assert( found_num == 2 )
