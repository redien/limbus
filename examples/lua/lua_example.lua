
-- Limbus - A cross-platform multimedia library.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

package.cpath = package.cpath .. ";?.dylib"

require "limbus"
require "OpenglWindow"

local window = limbus.OpenglWindow()
window.width = 800
window.height = 600
window.caption = "Window"
window:create()

local running = true

window:onClose(function()
	running = false
end)

while running do
	window:pollEvents()
	window:swapBuffers()
end

window:destruct()
