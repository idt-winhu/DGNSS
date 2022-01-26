# Microsoft Developer Studio Project File - Name="cmpl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cmpl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cmpl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cmpl.mak" CFG="cmpl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cmpl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cmpl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cmpl - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "ENAGLO" /D "ENAGAL" /D "ENAQZS" /D "ENACMP" /D "ENALEO" /FR /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\RtkLib.lib"

!ELSEIF  "$(CFG)" == "cmpl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "ENAGLO" /D "ENAGAL" /D "ENAQZS" /D "ENACMP" /D "ENALEO" /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\RtkLib.lib"

!ENDIF 

# Begin Target

# Name "cmpl - Win32 Release"
# Name "cmpl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\convkml.c
# End Source File
# Begin Source File

SOURCE=..\datum.c
# End Source File
# Begin Source File

SOURCE=..\download.c
# End Source File
# Begin Source File

SOURCE=..\ephemeris.c
# End Source File
# Begin Source File

SOURCE=..\geoid.c
# End Source File
# Begin Source File

SOURCE=..\ionex.c
# End Source File
# Begin Source File

SOURCE=..\lambda.c
# End Source File
# Begin Source File

SOURCE=..\options.c
# End Source File
# Begin Source File

SOURCE=..\pntpos.c
# End Source File
# Begin Source File

SOURCE=..\postpos.c
# End Source File
# Begin Source File

SOURCE=..\ppp.c
# End Source File
# Begin Source File

SOURCE=..\ppp_ar.c
# End Source File
# Begin Source File

SOURCE=..\preceph.c
# End Source File
# Begin Source File

SOURCE=..\qzslex.c
# End Source File
# Begin Source File

SOURCE=..\rcvraw.c
# End Source File
# Begin Source File

SOURCE=..\rinex.c
# End Source File
# Begin Source File

SOURCE=..\rtcm.c
# End Source File
# Begin Source File

SOURCE=..\rtcm2.c
# End Source File
# Begin Source File

SOURCE=..\rtcm3.c
# End Source File
# Begin Source File

SOURCE=..\rtcm3e.c
# End Source File
# Begin Source File

SOURCE=..\rtkcmn.c
# End Source File
# Begin Source File

SOURCE=..\rtklib.h
# End Source File
# Begin Source File

SOURCE=..\rtkpos.c
# End Source File
# Begin Source File

SOURCE=..\sbas.c
# End Source File
# Begin Source File

SOURCE=..\solution.c
# End Source File
# Begin Source File

SOURCE=..\tle.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
