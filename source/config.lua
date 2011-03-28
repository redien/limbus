
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


verbose = true
write_debug_output = false
lua_enable_argument_type_checking = false

name = "limbus"

c_source_path = "${INCLUDE_DIR}/"
cdef_source_path = "${BINDINGS_DIR}/"

output_path = "${GENERATED_DIR}/"
cpp_include_output_path = "${CPP_DIR}/limbus"
cpp_include_prefix = "limbus/"

languages = parse_cmake_list( "${BIND_LANGUAGES}" )
c_source_files = parse_cmake_list( "${BIND_HEADERS}" )
cdef_source_files = parse_cmake_list( "${BIND_SOURCES}" )

enum_filters = {
	"^LB(%w+)$",
}

function_filters = {
	"^lb_([%w_]+)$",
}

