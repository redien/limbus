
--          Copyright Jesper Oskarsson 2009 - 2010.
-- Distributed under the Boost Software License, Version 1.0.
--    (See accompanying file LICENSE_1_0.txt or copy at
--          http://www.boost.org/LICENSE_1_0.txt)

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

