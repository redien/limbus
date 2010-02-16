
--          Copyright Jesper Oskarsson 2009 - 2010.
-- Distributed under the Boost Software License, Version 1.0.
--    (See accompanying file LICENSE_1_0.txt or copy at
--          http://www.boost.org/LICENSE_1_0.txt)

local function parse_cmake_list( list )
	local tbl = {}
	list:gsub( "[^;]+", function( match )
		tbl[#tbl + 1] = match
	end)
	return tbl
end

include_directory = "${INCLUDE_DIR}"

local function make_cpp_includes( list )
	local tbl = {}
	for _, header in ipairs( list ) do
		tbl[#tbl + 1] = header:match( "^" .. include_directory .. "%/(.*)" )
	end
	return tbl
end

bind_headers = "${BIND_HEADERS}"
bind_dependancies = "${BIND_HEADERS}"

bind_headers = parse_cmake_list( bind_headers )
bind_dependancies = parse_cmake_list( bind_dependancies )

cpp_includes = make_cpp_includes( bind_headers )
cpp_imports = ""
for _, header in ipairs( cpp_includes ) do
	cpp_imports = cpp_imports .. "\
#include <" .. header .. ">"
end

options = {}
options.enum_filters = {
    "^LB(%w+)$",
}
options.function_filters = {
    "^lb_([%w_]+)$",
}

