
/* Copyright HockWare, Inc. 1994 */

#define  INCL_WIN                   /* required to use Win APIs.           */
#define  INCL_DOS
#define  INCL_PM                    /* required to use PM APIs.            */
#define  INCL_WINHELP               /* required to use IPF.                */
#define  INCL_OS2MM                 /* required for MCI and MMIO headers   */
#define  INCL_MMIOOS2               /* required for MMVIDEOHEADER          */
#define  INCL_MMIO_CODEC            /* required for circular, secondary    */
#define  INCL_SW                    /* required for circular, secondary    */

#include <os2.h>
#include <os2me.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sw.h>

#define VPDial_Class_Source
#include "vpDial.ih"

/* function prototype for the VpDial REXX extension */
ULONG   VpDial(PSZ Name, LONG Argc, RXSTRING Argv[], PSZ QueueName,
               PRXSTRING Retstr);

VPLOGICITEM Logic[] = {
          {
          130,                                        /* help panel ID    */
          "Set range",                                /* descriptive name */
          "CALL VpDial window, %s, 'SETRANGE', 0, 10",/* function model   */
          0,
          0
          },
          {
          131,
          "Get range",
          "range=VpDial(window, %s, 'GETRANGE')\r\nPARSE VAR range lo hi",
          0,
          0
          },
          {
          132,
          "Get value",
          "value=VpDial(window, %s, 'GETVALUE')",
          0,
          0
          },
          {
          133,
          "Set value",
          "CALL VpDial window, %s, 'SETVALUE', value",
          0,
          0
          },
          {
          134,
          "Set increment",
          "/* 1st is button increment, 2nd is tick increment */\r\n"
          "CALL VpDial window, %s, 'SETINC', 1, 5",
          0,
          0
          },
          {
          135,
          "Get increment",
          "inc=VpDial(window, %s, 'GETINC')\r\nPARSE VAR inc increment tick",
          0,
          0
          },
          {
          136,
          "Get radius",
          "value=VpDial(window, %s, 'GETRAD')",
          0,
          0
          },
          { /* this record marks the end of the list */
          0,
          "",
          "",
          0,
          0
          }
       };
#define INCL_CIRCULARSLIDER
#include <sw.h>

/* dialog procedure for the Styles page dialog procedure */
MRESULT EXPENTRY StyleDlgProc( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   SOMAny * somSelf = (SOMAny *)WinQueryWindowULong(hwndDlg, QWL_USER);
   ULONG ulStyle;
   ULONG ulStyleOld;
   BOOL bRecreate;

   switch (msg) {
   case WM_INITDLG:
      somSelf=(SOMAny *)PVOIDFROMMP(mp2);
      WinSetWindowULong(hwndDlg, QWL_USER, (ULONG)somSelf);
      _SetInitializedFlag(somSelf,FALSE);
      ulStyle = WinQueryWindowULong(_GetHwnd(somSelf), QWL_STYLE);

      /* save for undoing */
      _SetStyle(somSelf, ulStyle);

      WinSendMsg(hwndDlg,WM_VPRSETVALUES,0,0);

      break;
   case WM_VPRSETVALUES:
      ulStyle=_GetStyle(somSelf);
      if (ulStyle & WS_VISIBLE) {
         WinCheckButton(hwndDlg, ID_VISIBLE, 1);
      } else {
         WinCheckButton(hwndDlg, ID_VISIBLE, 0);
      } /* endif */
      if (ulStyle & WS_DISABLED) {
         WinCheckButton(hwndDlg, ID_DISABLED, 1);
      } else {
         WinCheckButton(hwndDlg, ID_DISABLED, 0);
      } /* endif */
      if (ulStyle & WS_GROUP) {
         WinCheckButton(hwndDlg, ID_GROUP, 1);
      } else {
         WinCheckButton(hwndDlg, ID_GROUP, 0);
      } /* endif */
      if (ulStyle & WS_TABSTOP) {
         WinCheckButton(hwndDlg, ID_TABSTOP, 1);
      } else {
         WinCheckButton(hwndDlg, ID_TABSTOP, 0);
      } /* endif */

      if (ulStyle & CSS_NOBUTTON) {
         WinCheckButton(hwndDlg, ID_NOBUTTON, 1);
      } else {
         WinCheckButton(hwndDlg, ID_NOBUTTON, 0);
      } /* endif */
      if (ulStyle & CSS_NOTEXT) {
         WinCheckButton(hwndDlg, ID_NOTEXT, 1);
      } else {
         WinCheckButton(hwndDlg, ID_NOTEXT, 0);
      } /* endif */
      if (ulStyle & CSS_NONUMBER) {
         WinCheckButton(hwndDlg, ID_NONUMBER, 1);
      } else {
         WinCheckButton(hwndDlg, ID_NONUMBER, 0);
      } /* endif */
      if (ulStyle & CSS_POINTSELECT) {
         WinCheckButton(hwndDlg, ID_POINTSELECT, 1);
      } else {
         WinCheckButton(hwndDlg, ID_POINTSELECT, 0);
      } /* endif */
      if (ulStyle & CSS_360) {
         WinCheckButton(hwndDlg, ID_360, 1);
      } else {
         WinCheckButton(hwndDlg, ID_360, 0);
      } /* endif */
      if (ulStyle & CSS_NOBUTTON) {
         WinCheckButton(hwndDlg, ID_MIDPOINT, 1);
      } else {
         WinCheckButton(hwndDlg, ID_MIDPOINT, 0);
      } /* endif */
      if (ulStyle & CSS_PROPORTIONALTICKS) {
         WinCheckButton(hwndDlg, ID_PROPORTIONALTICKS, 1);
      } else {
         WinCheckButton(hwndDlg, ID_PROPORTIONALTICKS, 0);
      } /* endif */

      _SetInitializedFlag(somSelf,TRUE);
      break;
   case WM_CONTROL:
      if (_GetInitializedFlag(somSelf)) {
          ulStyleOld = WinQueryWindowULong(_GetHwnd(somSelf),QWL_STYLE);
          ulStyle = ulStyleOld;
          bRecreate=FALSE;

          switch ( SHORT1FROMMP( mp1 ) ) {
          case ID_VISIBLE:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                WinShowWindow(_GetHwnd(somSelf),
                    WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 )));
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= WS_VISIBLE;
                } else {
                   ulStyle &= ~WS_VISIBLE;
                } /* endif */
             } /* endif */
             break;
          case ID_TABSTOP:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= WS_TABSTOP;
                } else {
                   ulStyle &= ~WS_TABSTOP;
                } /* endif */
             } /* endif */
             break;
          case ID_DISABLED:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= WS_DISABLED;
                } else {
                   ulStyle &= ~WS_DISABLED;
                } /* endif */
             } /* endif */
             break;
          case ID_GROUP:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= WS_GROUP;
                } else {
                   ulStyle &= ~WS_GROUP;
                } /* endif */
             } /* endif */
             break;
          case ID_NOBUTTON:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                bRecreate=TRUE;
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_NOBUTTON;
                } else {
                   ulStyle &= ~CSS_NOBUTTON;
                } /* endif */
             } /* endif */
             break;
          case ID_NOTEXT:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                bRecreate=TRUE;
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_NOTEXT;
                } else {
                   ulStyle &= ~CSS_NOTEXT;
                } /* endif */
             } /* endif */
             break;
          case ID_NONUMBER:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                bRecreate=TRUE;
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_NONUMBER;
                } else {
                   ulStyle &= ~CSS_NONUMBER;
                } /* endif */
             } /* endif */
             break;
          case ID_POINTSELECT:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_POINTSELECT;
                } else {
                   ulStyle &= ~CSS_POINTSELECT;
                } /* endif */
             } /* endif */
             break;
          case ID_360:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                bRecreate=TRUE;
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_360;
                } else {
                   ulStyle &= ~CSS_360;
                } /* endif */
             } /* endif */
             break;
          case ID_MIDPOINT:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                bRecreate=TRUE;
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_MIDPOINT;
                } else {
                   ulStyle &= ~CSS_MIDPOINT;
                } /* endif */
             } /* endif */
             break;
          case ID_PROPORTIONALTICKS:
             if (SHORT2FROMMP(mp1) == BN_CLICKED) {
                bRecreate=TRUE;
                if (WinQueryButtonCheckstate(hwndDlg,SHORT1FROMMP( mp1 ))) {
                   ulStyle |= CSS_PROPORTIONALTICKS;
                } else {
                   ulStyle &= ~CSS_PROPORTIONALTICKS;
                } /* endif */
             } /* endif */
             break;
          } /* endswitch */
          if (ulStyle!=ulStyleOld) {
             _IndicateChanged(somSelf);
             WinSetWindowULong(_GetHwnd(somSelf),QWL_STYLE,ulStyle);
             if (bRecreate) {
               _RecreateObject(somSelf);
             } else {
               _RedrawObject(somSelf);
             } /* endif */
          } /* endif */
      } /* endif */

      break;
   case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
      case ID_RESET:
         /* flag the object as changed so VisPro/REXX will save it */
         _IndicateChanged(somSelf);
         WinSendMsg(hwndDlg,WM_VPRSETVALUES,0,0);
         ulStyle=_GetStyle(somSelf);
         WinShowWindow(_GetHwnd(somSelf),
                 ((ulStyle & WS_VISIBLE)!=0));
         WinSetWindowULong(_GetHwnd(somSelf),QWL_STYLE,ulStyle);
         _RecreateObject(somSelf);

         break;
      } /* endswitch */

      break;
   case WM_HELP:
      WinPostMsg(_QueryHelpInstance(_somGetClass(somSelf)),
                 HM_DISPLAY_HELP,
                 MPFROMLONG(110),
                 HM_RESOURCEID);
      break;
   default :
      return WinDefDlgProc( hwndDlg, msg, mp1, mp2 );
   } /* endswitch */
   return FALSE;
}

/* the VPDial REXX Extension */

ULONG VpDial(PSZ Name, LONG Argc, RXSTRING Argv[], PSZ QueueName,
             PRXSTRING Retstr)
{
   ULONG ulID;
   HWND hwnd;
   HWND hwndControl;
   LONG lTemp1, lTemp2;

   Retstr->strptr[0]=0;
   Retstr->strlength = 0;
   //check for valid args

   if (Argc<3)
      return 40;

   hwnd = (HWND) atol(Argv[0].strptr);
   if (!WinIsWindow(0,hwnd))
      return 40;

   ulID = atol(Argv[1].strptr);

   hwndControl = WinWindowFromID(hwnd, ulID);

   if (!hwndControl)
      return 40;
   if (!WinIsWindow(0,hwndControl))
      return 40;

   if (!strcmp(Argv[2].strptr, "SETRANGE")) {
      if (Argc != 5)
         return 40;
      lTemp1=atol(Argv[3].strptr);
      lTemp2=atol(Argv[4].strptr);
      WinSendMsg(hwndControl, CSM_SETRANGE, MPFROMLONG(lTemp1),
                 MPFROMLONG(lTemp2));

   } else if (!strcmp(Argv[2].strptr, "GETRANGE")) {
      WinSendMsg(hwndControl, CSM_QUERYRANGE, MPFROMP(&lTemp1),
                 MPFROMP(&lTemp2));
      sprintf(Retstr->strptr,"%d %d",lTemp1, lTemp2);

   } else if (!strcmp(Argv[2].strptr, "SETVALUE")) {
      if (Argc != 4)
         return 40;
      lTemp1=atol(Argv[3].strptr);
      WinSendMsg(hwndControl, CSM_SETVALUE, MPFROMLONG(lTemp1), 0);

   } else if (!strcmp(Argv[2].strptr, "GETVALUE")) {
      WinSendMsg(hwndControl, CSM_QUERYVALUE, MPFROMP(&lTemp1), 0);
      sprintf(Retstr->strptr,"%d",lTemp1);

   } else if (!strcmp(Argv[2].strptr, "GETRAD")) {
      WinSendMsg(hwndControl, CSM_QUERYRADIUS, MPFROMP(&lTemp1), 0);
      sprintf(Retstr->strptr,"%d",lTemp1);

   } else if (!strcmp(Argv[2].strptr, "SETINC")) {
      if (Argc != 5)
         return 40;
      lTemp1=atol(Argv[3].strptr);
      lTemp2=atol(Argv[4].strptr);
      WinSendMsg(hwndControl, CSM_SETINCREMENT, MPFROMLONG(lTemp1),
                 MPFROMLONG(lTemp2));

   } else if (!strcmp(Argv[2].strptr, "GETINC")) {
      WinSendMsg(hwndControl, CSM_QUERYINCREMENT, MPFROMP(&lTemp1),
                 MPFROMP(&lTemp2));
      sprintf(Retstr->strptr,"%d %d",lTemp1, lTemp2);


   } else {
      return 40;
   } /* endif */

   Retstr->strlength = strlen(Retstr->strptr);
   return 0;

}


/*
 *
 *   METHOD:   GetStyle
 *   PURPOSE:  Get the style word used for reseting
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope ULONG   SOMLINK vpdia_GetStyle(VPDial *somSelf)
{
    VPDialData *somThis = VPDialGetData(somSelf);
    VPDialMethodDebug("VPDial","vpdia_GetStyle");

    return (ULONG) _ulStyle;
}

/*
 *
 *   METHOD:   SetStyle
 *   PURPOSE:  Set the style word used for reseting
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope VOID   SOMLINK vpdia_SetStyle(VPDial *somSelf,
                ULONG ulStyle)
{
    VPDialData *somThis = VPDialGetData(somSelf);
    VPDialMethodDebug("VPDial","vpdia_SetStyle");

    _ulStyle=ulStyle;

}

/*
 *
 *   METHOD:   GetInitializedFlag
 *   PURPOSE:  Get the initialized word
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope BOOL   SOMLINK vpdia_GetInitializedFlag(VPDial *somSelf)
{
    VPDialData *somThis = VPDialGetData(somSelf);
    VPDialMethodDebug("VPDial","vpdia_GetInitializedFlag");

    return (BOOL) _bInitialized;
}

/*
 *
 *   METHOD:   SetInitializedFlag
 *   PURPOSE:  Set the initialized flag
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope VOID   SOMLINK vpdia_SetInitializedFlag(VPDial *somSelf,
                BOOL bInitialized)
{
    VPDialData *somThis = VPDialGetData(somSelf);
    VPDialMethodDebug("VPDial","vpdia_SetInitializedFlag");

    _bInitialized=bInitialized;

}
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta
SOM_Scope VOID   SOMLINK vpdiac_QueryClassInfo(M_VPDial *somSelf,
                PVPCLASSINFO pVpClassInfo)
{
    M_VPDialData *somThis = M_VPDialGetData(somSelf);
    M_VPDialMethodDebug("M_VPDial","vpdiac_QueryClassInfo");

    if (pVpClassInfo) {
       pVpClassInfo->cbSize=sizeof(VPCLASSINFO);
       pVpClassInfo->pszDescName       = "Circular Slider";
       pVpClassInfo->pszAbbrevName     = "CSLD";
       pVpClassInfo->bHasText          = TRUE;
       pVpClassInfo->pszDefaultText    = 0;
       pVpClassInfo->bHasCtrlData      = FALSE;
       pVpClassInfo->pszWindowClass    = WC_CIRCULARSLIDER;
       pVpClassInfo->lDefaultWidth     = 66;
       pVpClassInfo->lDefaultHeight    = 66;
       pVpClassInfo->ulDefaultStyle    =
                  WS_GROUP | WS_VISIBLE | WS_NOTEXT | WS_TABSTOP |
                  CSS_MIDPOINT | CSS_PROPORTIONALTICKS;
       pVpClassInfo->pDefaultCtrlData  = 0;
       pVpClassInfo->hModResource      = _hModResource;
       pVpClassInfo->lIconResID        = ID_POINTER;
       pVpClassInfo->lStyleDlgID       = ID_DIALDLG;
       pVpClassInfo->pfnwpStyleDlgProc = StyleDlgProc;
       pVpClassInfo->lToolHelpID       = 100;
       pVpClassInfo->lStylesPageHelpID = 110;
       pVpClassInfo->hwndHelpInstance  = 0;
       pVpClassInfo->lNumEvents        = 3;
       pVpClassInfo->Events[0].pszDescName="Entered";
       pVpClassInfo->Events[0].pszAbbrevName="ENT";
       pVpClassInfo->Events[0].pszModel=0;
       pVpClassInfo->Events[0].lIconResID=ID_ENTERPTR;
       pVpClassInfo->Events[0].lHelpID=120;
       pVpClassInfo->usTranslate[0]=CSN_SETFOCUS;
       pVpClassInfo->Events[1].pszDescName="Changed";
       pVpClassInfo->Events[1].pszAbbrevName="CHG";
       pVpClassInfo->Events[1].pszModel=0;
       pVpClassInfo->Events[1].lIconResID=ID_CHANGEDPTR;
       pVpClassInfo->Events[1].lHelpID=121;
       pVpClassInfo->usTranslate[1]=CSN_CHANGED;
       pVpClassInfo->Events[2].pszDescName="Tracking";
       pVpClassInfo->Events[2].pszAbbrevName="TRK";
       pVpClassInfo->Events[2].pszModel=0;
       pVpClassInfo->Events[2].lIconResID=ID_TRACKINGPTR;
       pVpClassInfo->Events[2].lHelpID=122;
       pVpClassInfo->usTranslate[2]=CSN_TRACKING;
       pVpClassInfo->Logic             = Logic;
    } /* endif */
}

SOM_Scope void   SOMLINK vpdiac_somInitClass(M_VPDial *somSelf,
                IN zString className,
                IN SOMAny *parentClass,
                IN integer4 instanceSize,
                IN int maxStaticMethods,
                IN integer4 majorVersion,
                IN integer4 minorVersion)
{
    M_VPDialData *somThis = M_VPDialGetData(somSelf);
    CHAR ErrorBuffer[100];
    M_VPDialMethodDebug("M_VPDial","vpdiac_somInitClass");

    DosLoadModule((PSZ) ErrorBuffer, sizeof(ErrorBuffer), "VPDIAL",
                  &_hModResource);
    WinRegisterCircularSlider();
    RexxRegisterFunctionDll("VpDial","VPDIAL","VpDial");

    parent_somInitClass(somSelf,className,parentClass,instanceSize,
                        maxStaticMethods,majorVersion,minorVersion);
}

SOM_Scope HWND   SOMLINK vpdiac_InitHelpInstance(M_VPDial *somSelf)
{
    M_VPDialData *somThis = M_VPDialGetData(somSelf);
    HELPINIT hmiHelpData;       /* Help initialization structure     */
    HWND hwndHelpInstance;
    CHAR pszHelp[CCHMAXPATH];
    PSZ pszTemp;

    M_VPDialMethodDebug("M_VPDial","vpdiac_InitHelpInstance");

    if (DosScanEnv("VISPRORX",&pszTemp)) {
       strcpy(pszHelp,"C:\\VISPRORX");
    } else {
       strcpy(pszHelp,pszTemp);
    } /* endif */
    strcat(pszHelp,"\\VPDIAL.HLP");

    /**********************************************************************/
    /* IPF Initialization Structure                                       */
    /**********************************************************************/
    /* size of initialization structure                                   */
    /**********************************************************************/
    hmiHelpData.cb = sizeof(HELPINIT);
    /**********************************************************************/
    /* store HM return code from init.                                    */
    /**********************************************************************/
    hmiHelpData.ulReturnCode = (ULONG)NULL;
    /**********************************************************************/
    /* no tutorial program                                                */
    /**********************************************************************/
    hmiHelpData.pszTutorialName = NULL;
    /**********************************************************************/
    /* indicates help table is defined in the RC file.                    */
    /**********************************************************************/
    hmiHelpData.phtHelpTable = 0;
    /**********************************************************************/
    /* action bar is not tailored                                         */
    /**********************************************************************/
    hmiHelpData.hmodAccelActionBarModule = 0;
    hmiHelpData.idAccelTable = 0;
    hmiHelpData.idActionBar = 0;
    /**********************************************************************/
    /* help window title                                                  */
    /**********************************************************************/
    hmiHelpData.pszHelpWindowTitle = "VisPro/REXX Circular Slider Help Window";
    /**********************************************************************/
    /* help table in not in a DLL                                         */
    /**********************************************************************/
    hmiHelpData.hmodHelpTableModule = 0;
    /**********************************************************************/
    /* help panels ID is not displayed                                    */
    /**********************************************************************/
    hmiHelpData.fShowPanelId = 0;
    /**********************************************************************/
    /* library with help panels                                           */
    /**********************************************************************/
    hmiHelpData.pszHelpLibraryName = pszHelp;
    /**********************************************************************/
    /* Create Instance of IPF pass Anchor Block handle and address of IPF */
    /* initialization structure, and check that creation was successful.  */
    /**********************************************************************/
    hwndHelpInstance = WinCreateHelpInstance(0, &hmiHelpData);


    return (hwndHelpInstance);
}
