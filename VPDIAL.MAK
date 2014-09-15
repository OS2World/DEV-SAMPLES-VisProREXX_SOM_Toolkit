#
#  Copyright (c) HockWare Inc. 1993
#
#****************************************************************************
#  Environment Setup for the component
#****************************************************************************
SCPATH=\TOOLKT20\SC
DPATH=\TOOLKT20\OS2BIN;\TOOLKT20\OS2HELP;\IBMC\LOCALE;\IBMC\HELP;$(DPATH)
HELP=\TOOLKT20\OS2HELP;\IBMC\HELP;$(HELP)
IPF_KEYS=SBCS
INCLUDE=\TOOLKT20\C\OS2H;\IBMC\INCLUDE;\mmos2\mmtoolkt\h;\sqllib;\muglib;\qelib\c\include
LIB=\TOOLKT20\OS2LIB;\IBMC\LIB;\MMOS2\MMTOOLKT\LIB;\sqllib;\muglib;\qelib\c\lib
IPFC=\TOOLKT20\IPFC;
PATH=\TOOLKT20\OS2BIN;\IBMC\BIN;\VISPRO20\TOOLS;\sqllib;$(PATH)
SMEMIT=H;IH;PH;SC;c
SOMLIBS = DDE4NBS + OS2386 + SOM


.SUFFIXES: .rc .res .ipf .inf .hlp .csc .sc .h .ih .ph .psc

#
# Compiler/tools Macros
#
CLOCALFLAGS= /Ge-
LLOCALFLAGS= /noi
SOMTEMP = .\somtemp
CFLAGS=/c+ /Gd-e+m+s- /Ses+ /Q+ /Ms /Kb+c+e+o-r+ /W2 /DNODEBUG /O+
LFLAGS=/NOI /NOE /NOD /A:16 /MAP:full /NOLOGO /EXEPACK /BASE:0x10000
LINK=LINK386  $(LFLAGS)
RC_FLAGS=-r

.c.obj :
       icc  $(CFLAGS) $(CLOCALFLAGS) $<
.rc.res:
    rc $(RC_FLAGS) $< $@
.csc.ih:
        sc $*

all:        vpdial.dll


vpdial.ih: vpdial.csc

vpdial.res: vpdial.rc vpdial.mak vpdial.ptr vpdial.h
         rc $(RC_FLAGS) vpdial.rc vpdial.res

# C files
#
vpdial.obj: vpdial.c vpdial.ih vpdial.h  vpdial.sc vpobj.h

# Main files: vpdial.DLL
#
vpdial.dll: vpdial.obj vpdial.res vpdial.mak
         $(LINK) vpdial.obj,$@,,$(SOMLIBS)+REXX+VPOBJ,$*;
         rc vpdial.res vpdial.dll

