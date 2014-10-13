
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.


require "./limbus"
require "./screen"
require "./opengl_window"
require "./timer"

require "opengl"

window = Limbus::OpenglWindow.new
window.width = 800
window.height = 600
window.caption = "Window"
window.create

running = true

window.on_close do
	running = false
end

while running do
	window.poll_events
    GL.Clear GL::COLOR_BUFFER_BIT
	window.swap_buffers
end

window.destruct
