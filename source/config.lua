
-- Limbus - A cross-platform multimedia library.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

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
library_name = "${CMAKE_SHARED_LIBRARY_PREFIX}" .. name .. "${CMAKE_SHARED_LIBRARY_SUFFIX}"

c_source_path = "${INCLUDE_DIR}/"
cdef_source_path = "${BINDINGS_DIR}/"

output_path = "${GENERATED_DIR}/"
cpp_include_output_path = "${CPP_DIR}/limbus"
cpp_include_prefix = "limbus/"

languages = parse_cmake_list( "${BIND_LANGUAGES}" )
c_source_files = parse_cmake_list( "${BIND_HEADERS}" )
cdef_source_files = parse_cmake_list( "${BIND_SOURCES}" )

c_shared_libraries = parse_cmake_list("${SHARED_LIBRARIES}")

enum_filters = {
	"^LB(%w+)$",
}

function_filters = {
	"^lb_([%w_]+)$",
}

