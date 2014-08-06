
-- Limbus - A cross-platform multimedia library.
-- Written in 2014 by Jesper Oskarsson jesosk@gmail.com
--
-- To the extent possible under law, the author(s) have dedicated all copyright
-- and related and neighboring rights to this software to the public domain worldwide.
-- This software is distributed without any warranty.
--
-- You should have received a copy of the CC0 Public Domain Dedication along with this software.
-- If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

solution "CSharpExamples"
	configurations {"Debug", "Release"}
	location "build"

project "csharp_example"
	kind "WindowedApp"
	language "C#"
	location "build"
	targetdir "build"
	files {"csharp_example.cs", "../../generated/csharp/*.cs"}

project "audio_stream_example"
	kind "ConsoleApp"
	language "C#"
	location "build"
	targetdir "build"
	files {"audio_stream_example.cs", "../../generated/csharp/*.cs"}
	flags {"unsafe"}

