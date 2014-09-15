##########################################################
#  VPText.CSC              by Dave Hock                 #
#                                                        #
##########################################################
#
# Include the class definition file for the parent class
#
include <vpobj.sc>

#
#   Define the new class
#
class: VPText,
       file stem = VPText,
       external prefix = vptexte_,
       class prefix = vptextc_,
       major version = 1,
       minor version = 1,
       local;
#
# Specify the parent class
#
parent: VPObj;

#
# Specify the release order of new methods
#
passthru: C.ih;



endpassthru;


passthru: C.h, after;

#define ID_POINTER 256

#define ID_ENTERPTR 257
#define ID_CHANGEDPTR  258
#define ID_EXITPTR 259

#include "VPTextdg.h"

endpassthru;
#
#
# Define instance data for the class
#
data:

HMODULE hModResource,class;
ULONG ulStyle;
BOOL  bInitialized;

#
# Define new methods
#
methods:


ULONG GetStyle();
--
--  METHOD:   GetStyle
--  PURPOSE:  Get the style word used for reseting
--  INVOKED:  From style dialog in editor session
--

VOID SetStyle(ULONG ulStyle);
--
--  METHOD:   SetStyle
--  PURPOSE:  Set the style word used for reseting
--  INVOKED:  From style dialog in editor session
--

BOOL GetInitializedFlag();
--
--  METHOD:   GetInitializedFlag
--  PURPOSE:  Get the initialized word
--  INVOKED:  From style dialog in editor session
--

VOID SetInitializedFlag(BOOL bInitialized);
--
--  METHOD:   SetInitializedFlag
--  PURPOSE:  Set the initialized flag
--  INVOKED:  From style dialog in editor session
--


override QueryClassInfo, class;
override somInitClass, class;
override InitHelpInstance, class;
