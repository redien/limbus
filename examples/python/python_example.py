
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

from opengl_window import *
from OpenGL.GL import *

window = OpenglWindow()
window.caption = "Hello from Python!"
window.width = 800
window.height = 600
window.create()

running = True

def on_close():
    global running
    running = False

window.on_close.append( on_close )

while running:
    window.poll_events()
    glClear(GL_COLOR_BUFFER_BIT)
    window.swap_buffers()

window.destruct()
