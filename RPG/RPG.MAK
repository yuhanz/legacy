# Microsoft Developer Studio Generated NMAKE File, Based on RPG.DSP
!IF "$(CFG)" == ""
CFG=RPG - Win32 Debug
!MESSAGE No configuration specified. Defaulting to RPG - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RPG - Win32 Release" && "$(CFG)" != "RPG - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "RPG - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\RPG.exe"

!ELSE 

ALL : "$(OUTDIR)\RPG.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\RPG.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\RPG.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RPG.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\RPG.pdb" /machine:I386 /out:"$(OUTDIR)\RPG.exe" 
LINK32_OBJS= \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\RPG.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RPG - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\RPG.exe"

!ELSE 

ALL : "$(OUTDIR)\RPG.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\RPG.exe"
	-@erase "$(OUTDIR)\RPG.ilk"
	-@erase "$(OUTDIR)\RPG.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\RPG.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RPG.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\RPG.pdb" /debug /machine:I386 /out:"$(OUTDIR)\RPG.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\RPG.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "RPG - Win32 Release" || "$(CFG)" == "RPG - Win32 Debug"
SOURCE=..\common\common.cpp
DEP_CPP_COMMO=\
	{$(INCLUDE)}"common.h"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\main.cpp

!IF  "$(CFG)" == "RPG - Win32 Release"

DEP_CPP_MAIN_=\
	{$(INCLUDE)}"basetsd.h"\
	{$(INCLUDE)}"common.h"\
	{$(INCLUDE)}"commonDX.h"\
	{$(INCLUDE)}"commonwin.h"\
	{$(INCLUDE)}"dinput.h"\
	{$(INCLUDE)}"dls1.h"\
	{$(INCLUDE)}"dmdls.h"\
	{$(INCLUDE)}"dmerror.h"\
	{$(INCLUDE)}"dmplugin.h"\
	{$(INCLUDE)}"dmusbuff.h"\
	{$(INCLUDE)}"dmusicc.h"\
	{$(INCLUDE)}"dmusici.h"\
	{$(INCLUDE)}"tile.h"\
	{$(INCLUDE)}"TSprite.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RPG - Win32 Debug"

DEP_CPP_MAIN_=\
	{$(INCLUDE)}"basetsd.h"\
	{$(INCLUDE)}"common.h"\
	{$(INCLUDE)}"commonDX.h"\
	{$(INCLUDE)}"commonwin.h"\
	{$(INCLUDE)}"dinput.h"\
	{$(INCLUDE)}"dls1.h"\
	{$(INCLUDE)}"dmdls.h"\
	{$(INCLUDE)}"dmerror.h"\
	{$(INCLUDE)}"dmplugin.h"\
	{$(INCLUDE)}"dmusbuff.h"\
	{$(INCLUDE)}"dmusicc.h"\
	{$(INCLUDE)}"dmusici.h"\
	{$(INCLUDE)}"tile.h"\
	{$(INCLUDE)}"TSprite.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ENDIF 


!ENDIF 

