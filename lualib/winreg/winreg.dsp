# Microsoft Developer Studio Project File - Name="winreg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=winreg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "winreg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winreg.mak" CFG="winreg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winreg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHELL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\..\src\scite\lua\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D CRTAPI1=_cdecl /D CRTAPI2=_cdecl /D "WIN32_LEAN_AND_MEAN" /D _WIN32_WINNT=0x0400 /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ../lib/SciTE.lib shlwapi.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib user32.lib kernel32.lib /nologo /dll /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "winreg - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lua_int64.c
# End Source File
# Begin Source File

SOURCE=.\lua_mtutil.c
# End Source File
# Begin Source File

SOURCE=.\lua_tstring.c
# End Source File
# Begin Source File

SOURCE=.\luawin_dllerror.c
# End Source File
# Begin Source File

SOURCE=.\win_privileges.c
# End Source File
# Begin Source File

SOURCE=.\win_registry.c
# End Source File
# Begin Source File

SOURCE=.\win_trace.c
# End Source File
# Begin Source File

SOURCE=.\winreg.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lua_int64.h
# End Source File
# Begin Source File

SOURCE=.\lua_mtutil.h
# End Source File
# Begin Source File

SOURCE=.\lua_tstring.h
# End Source File
# Begin Source File

SOURCE=.\luamacro.h
# End Source File
# Begin Source File

SOURCE=.\luareg.h
# End Source File
# Begin Source File

SOURCE=.\luawin_dllerror.h
# End Source File
# Begin Source File

SOURCE=.\luawinmacro.h
# End Source File
# Begin Source File

SOURCE=.\stdmacro.h
# End Source File
# Begin Source File

SOURCE=.\win_privileges.h
# End Source File
# Begin Source File

SOURCE=.\win_registry.h
# End Source File
# Begin Source File

SOURCE=.\win_trace.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
