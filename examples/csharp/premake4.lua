
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
	kind "WindowedApp"
	language "C#"
	location "build"
	targetdir "build"
	files {"audio_stream_example.cs", "../../generated/csharp/*.cs"}
	flags {"unsafe"}

