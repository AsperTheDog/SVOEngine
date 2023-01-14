-- premake5.lua
workspace "SVOEngine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "SVOEngine"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "SVOEngine"