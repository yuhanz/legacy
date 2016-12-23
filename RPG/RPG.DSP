# Microsoft Developer Studio Project File - Name="RPG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RPG - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RPG.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RPG.MAK" CFG="RPG - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RPG - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RPG - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RPG - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "RPG - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib dinput8.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "RPG - Win32 Release"
# Name "RPG - Win32 Debug"
# Begin Source File

SOURCE=.\AI.cpp
# End Source File
# Begin Source File

SOURCE=.\AI.h
# End Source File
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\CombatStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\CombatStatus.h
# End Source File
# Begin Source File

SOURCE=..\common\common.cpp
# End Source File
# Begin Source File

SOURCE=..\common\common.h
# End Source File
# Begin Source File

SOURCE=..\common\commonDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\Control.h
# End Source File
# Begin Source File

SOURCE=.\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\Game45.cpp
# End Source File
# Begin Source File

SOURCE=.\Game45.h
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\Manager.h
# End Source File
# Begin Source File

SOURCE=.\Map.cpp
# End Source File
# Begin Source File

SOURCE=.\Map.h
# End Source File
# Begin Source File

SOURCE=..\common\MusicDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MusicDevice.h
# End Source File
# Begin Source File

SOURCE=.\MyLogo.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLogo.h
# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# Begin Source File

SOURCE=.\SubList.h
# End Source File
# Begin Source File

SOURCE=..\common\Tile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Tile.h
# End Source File
# Begin Source File

SOURCE=..\common\TSprite.cpp
# End Source File
# Begin Source File

SOURCE=..\common\TSprite.h
# End Source File
# Begin Source File

SOURCE=..\common\Utility.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Utility.h
# End Source File
# End Target
# End Project
