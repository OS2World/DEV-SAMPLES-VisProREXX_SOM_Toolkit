
/* Copyright HockWare, Inc. 1994 */

#define  INCL_WIN                   /* required to use Win APIs.           */
#define  INCL_DOS
#define  INCL_PM                    /* required to use PM APIs.            */
#define  INCL_WINHELP               /* required to use IPF.                */

#include <os2.h>
#include <os2me.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define VPText_Class_Source
#include "VPText.ih"

VPLOGICITEM Logic[] = {
          {
          0,
          "Get item value",
          "value = VpGetItemValue(window,%s)",
          0,
          0
          },
          {
          0,
          "Set item value",
          "CALL VpSetItemValue window,%s,value ",
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


PSZ szClass="TestWndClass";

typedef struct _MYCDATA{
   ULONG cb;
   /* remaining data is programmer defined */
   GRADIENTL gradient;  /* angle of rotation */
}MYCDATA;
typedef MYCDATA *PMYCDATA;

/* default angle of 0 degrees */
MYCDATA defaultData= { sizeof(MYCDATA), {1,0}};

#define MAXTEXTLENGTH 255

/* We use QWL_USER+4 to save our control data pointer.        */
/* Note that QWL_USER is reserved for VisPro/REXX             */
/* When we registered our window class, we allocated 12 extra */
/* bytes for window words for three LONGs.                    */
/*      QWL_USER   - reserved for Vispro/REXX                 */
/*      QWL_USER+4 - reserved for control data                */
/*      QWL_USER+8 - reserved for text                        */
MRESULT EXPENTRY TestWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  PMYCDATA pCData = (PMYCDATA)WinQueryWindowULong(hwnd, QWL_USER+4);
  PSZ pszText = (PSZ)WinQueryWindowULong(hwnd, QWL_USER+8);
  PMYCDATA pCreate;
  PCREATESTRUCT pCreateStruct;

  switch (msg) {
  /* we are creating our control */
  case WM_CREATE:
     /* if data was passed in */
     if (pCreate=(PMYCDATA)PVOIDFROMMP(mp1)) {
        /* allocate memory for the control data */
        DosAllocMem((PPVOID) &pCData,
                           (ULONG) pCreate->cb,
                           PAG_COMMIT|PAG_READ | PAG_WRITE);

        /* copy the data that is passed in */
        memcpy((PBYTE)pCData, (PBYTE) pCreate, pCreate->cb);

        /* save the control data pointer */
        WinSetWindowULong(hwnd, QWL_USER+4, (ULONG)pCData);
     } /* endif */
     /* allocate memory for the text */
     DosAllocMem((PPVOID) &pszText,
                        (ULONG) MAXTEXTLENGTH+1,
                        PAG_COMMIT|PAG_READ | PAG_WRITE);
     /* save the text pointer */
     WinSetWindowULong(hwnd, QWL_USER+8, (ULONG)pszText);
     /* initialize to a NULL string */
     strcpy(pszText,"");
     pCreateStruct=(PCREATESTRUCT)mp2;
     if (pCreateStruct) {
        /* copy the text into the buffer provided */
        strncpy(pszText, pCreateStruct->pszText,  MAXTEXTLENGTH);

        /* NULL terminate the string */
        pszText[MAXTEXTLENGTH]=0;
     }
     break;
  case WM_QUERYWINDOWPARAMS:
     {
        PWNDPARAMS pWndParams = (PWNDPARAMS)PVOIDFROMMP(mp1);
        MRESULT mr;

        /* call the default window proc first so that presentation
        /* parameters are handled */

        mr = WinDefWindowProc( hwnd, msg, mp1, mp2 );
        if (pWndParams) {
           if (pCData) {
              if (pWndParams->fsStatus & WPM_CBCTLDATA) {
                 pWndParams->cbCtlData = pCData->cb;
                 mr=(MRESULT)TRUE;
              } /* endif */
              if (pWndParams->fsStatus & WPM_CTLDATA) {
                 pWndParams->cbCtlData = pCData->cb;
                 memcpy((PBYTE)pWndParams->pCtlData, (PBYTE)pCData,  pCData->cb);
                 mr=(MRESULT)TRUE;
              } /* endif */
           } /* endif */

           /* responding to WinQueryWindowTextLength */
           if (pWndParams->fsStatus & WPM_CCHTEXT) {
              pWndParams->cchText = strlen(pszText);
              mr=(MRESULT)TRUE;
           } /* endif */

           /* responding to WinQueryWindowText */
           if (pWndParams->fsStatus & WPM_TEXT) {

              /* copy the text into the buffer provided */
              strncpy(pWndParams->pszText, pszText,  pWndParams->cchText);

              /* NULL terminate the string */
              pWndParams->pszText[pWndParams->cchText]=0;
              mr=(MRESULT)TRUE;
           } /* endif */
        } /* endif */
        return mr;
     }
     break;
  case WM_SETWINDOWPARAMS:
     {
        PWNDPARAMS pWndParams = (PWNDPARAMS)PVOIDFROMMP(mp1);
        MRESULT mr;

        mr = WinDefWindowProc( hwnd, msg, mp1, mp2 );
        if (pWndParams) {
           if (pWndParams->fsStatus & WPM_CTLDATA) {
              if (pCData) {
                 DosFreeMem(pCData);
              } /* endif */
              DosAllocMem((PPVOID) &(pCData),
                                 (ULONG) pWndParams->cbCtlData,
                                 PAG_COMMIT|PAG_READ | PAG_WRITE);
              WinSetWindowULong(hwnd, QWL_USER+4, (ULONG)pCData);
              memcpy((PBYTE)pCData, (PBYTE)pWndParams->pCtlData,  pWndParams->cbCtlData);
              WinInvalidateRect(hwnd, 0, 0);
              mr=(MRESULT)TRUE;
           } /* endif */

           /* responding to WinQueryWindowText */
           if (pWndParams->fsStatus & WPM_TEXT) {

              /* copy the text into the buffer provided */
              strncpy(pszText, pWndParams->pszText,  MAXTEXTLENGTH);

              /* NULL terminate the string */
              pszText[MAXTEXTLENGTH]=0;
              WinInvalidateRect(hwnd, 0, 0);
              mr=(MRESULT)TRUE;
           } /* endif */
        } /* endif */
        return mr;
     }
     break;
  case WM_PAINT:
     {
        HPS hps;
        RECTL rectlPaint;
        ULONG lTextColor, lBackColor;
        ULONG ulPPIndex;
        SWP swp;
        POINTL ptlPoint;

        WinQueryWindowPos(hwnd,&swp);
        hps=WinBeginPaint(hwnd, 0, &rectlPaint);

        /* put PS into RGB color mode */
        GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, 0 );

        if (!WinQueryPresParam(hwnd,PP_BACKGROUNDCOLOR,0,&ulPPIndex,
              sizeof(LONG),&lBackColor,QPF_NOINHERIT)) {
           lBackColor=WinQuerySysColor(HWND_DESKTOP,SYSCLR_WINDOW, 0);
        } /* endif */

        if (!WinQueryPresParam(hwnd,PP_FOREGROUNDCOLOR,0,&ulPPIndex,
              sizeof(LONG),&lTextColor,QPF_NOINHERIT)) {
           lTextColor=WinQuerySysColor(HWND_DESKTOP,SYSCLR_ICONTEXT, 0);
        } /* endif */

        WinFillRect(hps, &rectlPaint, lBackColor);
        ptlPoint.x=0;
        ptlPoint.y=0;
        GpiMove(hps,&ptlPoint);
        ptlPoint.x=swp.cx-1;
        ptlPoint.y=swp.cy-1;
        GpiBox(hps,DRO_OUTLINE,&ptlPoint,0,0);


        /* draw your control and its text here */
        ptlPoint.x=0;
        ptlPoint.y=0;
        GpiSetCharAngle(hps,&pCData->gradient);
        GpiSetCharMode(hps,CM_MODE3);
        GpiSetColor(hps,lTextColor);
        GpiSetDrawControl(hps,DCTL_DISPLAY,DCTL_OFF);
        GpiSetDrawControl(hps,DCTL_BOUNDARY,DCTL_ON);
        GpiCharStringAt(hps, &ptlPoint,strlen(pszText),pszText);
        GpiQueryBoundaryData(hps,&rectlPaint);
        GpiSetDrawControl(hps,DCTL_DISPLAY,DCTL_ON);
        GpiSetDrawControl(hps,DCTL_BOUNDARY,DCTL_OFF);
        ptlPoint.x=-rectlPaint.xLeft;
        ptlPoint.y=-rectlPaint.yBottom;
        GpiCharStringAt(hps, &ptlPoint,strlen(pszText),pszText);

        WinEndPaint(hps);
     }
     break;
  case WM_DESTROY:
     if (pCData)
        DosFreeMem(pCData);
     if (pszText)
        DosFreeMem(pszText);
     break;
  case WM_PRESPARAMCHANGED:
     WinInvalidateRect(hwnd,0,FALSE);
     break;
  default:
     return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  } /* endswitch */
  return (MRESULT) FALSE;
}
/* dialog procedure for the Styles page dialog procedure */
MRESULT EXPENTRY StyleDlgProc( HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   SOMAny * somSelf = (SOMAny *)WinQueryWindowULong(hwndDlg, QWL_USER);
   ULONG ulStyle;
   ULONG ulStyleOld;
   BOOL bRecreate;
   PMYCDATA pCData;
   BOOL bCDataChanged;
   WNDPARAMS wndparams;
   MYCDATA cData;
   CHAR pszText[32];

   switch (msg) {
   case WM_INITDLG:
      somSelf=(SOMAny *)PVOIDFROMMP(mp2);
      WinSetWindowULong(hwndDlg, QWL_USER, (ULONG)somSelf);
      _SetInitializedFlag(somSelf,FALSE);
      ulStyle = WinQueryWindowULong(_GetHwnd(somSelf), QWL_STYLE);
      /* set the limits for the spin buttons */
      WinSendDlgItemMsg(hwndDlg,ID_HORZSB,SPBM_SETLIMITS,
                     MPFROMLONG(1000),MPFROMLONG(-1000));
      WinSendDlgItemMsg(hwndDlg,ID_VERTSB,SPBM_SETLIMITS,
                     MPFROMLONG(1000),MPFROMLONG(-1000));

      /* save for undoing */
      _SetStyle(somSelf, ulStyle);

      WinSendMsg(hwndDlg,WM_VPRSETVALUES,0,0);

      break;
   case WM_VPRSETVALUES:
      ulStyle=_GetStyle(somSelf);
      pCData = (PMYCDATA)WinQueryWindowULong(_GetHwnd(somSelf), QWL_USER+4);
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
      wndparams.fsStatus = WPM_CTLDATA;
      wndparams.pCtlData = (PVOID)&cData;
      wndparams.cbCtlData=sizeof(cData);
      WinSendMsg(_GetHwnd(somSelf),WM_QUERYWINDOWPARAMS,
                  MPFROMP(&wndparams),0);
      WinSendDlgItemMsg(hwndDlg,ID_HORZSB,SPBM_SETCURRENTVALUE,
                   MPFROMLONG(cData.gradient.x),0);
      WinSendDlgItemMsg(hwndDlg,ID_VERTSB,SPBM_SETCURRENTVALUE,
                   MPFROMLONG(cData.gradient.y),0);


      _SetInitializedFlag(somSelf,TRUE);
      break;
   case WM_CONTROL:
      if (_GetInitializedFlag(somSelf)) {
          ulStyleOld = WinQueryWindowULong(_GetHwnd(somSelf),QWL_STYLE);
          ulStyle = ulStyleOld;
          bRecreate=FALSE;
          bCDataChanged=FALSE;

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
          case ID_HORZSB:
          case ID_VERTSB:
             if ((SHORT2FROMMP(mp1) == SPBN_ENDSPIN) ||
                (SHORT2FROMMP(mp1) == SPBN_KILLFOCUS)) {
                cData.cb=sizeof(cData);
                wndparams.fsStatus=WPM_CTLDATA;
                wndparams.cbCtlData=cData.cb;
                wndparams.pCtlData=(PBYTE)&cData;

                WinSendDlgItemMsg(hwndDlg,ID_VERTSB,SPBM_QUERYVALUE,
                    MPFROMP(pszText),
                    MPFROM2SHORT(sizeof(pszText),SPBQ_ALWAYSUPDATE));

                cData.gradient.x=atol(pszText);

                WinSendDlgItemMsg(hwndDlg,ID_HORZSB,SPBM_QUERYVALUE,
                    MPFROMP(pszText),
                    MPFROM2SHORT(sizeof(pszText),SPBQ_ALWAYSUPDATE));

                cData.gradient.y=atol(pszText);

                WinSendMsg(_GetHwnd(somSelf),
                           WM_SETWINDOWPARAMS, MPFROMP(&wndparams),0);

                bCDataChanged=TRUE;
             } /* endif */
             break;
          } /* endswitch */
          if (ulStyle != ulStyleOld || bCDataChanged) {
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
         _RedrawObject(somSelf);

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


/*
 *
 *   METHOD:   GetStyle
 *   PURPOSE:  Get the style word used for reseting
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope ULONG   SOMLINK vptexte_GetStyle(VPText *somSelf)
{
    VPTextData *somThis = VPTextGetData(somSelf);
    VPTextMethodDebug("VPText","vptexte_GetStyle");

    return (ULONG) _ulStyle;
}

/*
 *
 *   METHOD:   SetStyle
 *   PURPOSE:  Set the style word used for reseting
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope VOID   SOMLINK vptexte_SetStyle(VPText *somSelf,
                ULONG ulStyle)
{
    VPTextData *somThis = VPTextGetData(somSelf);
    VPTextMethodDebug("VPText","vptexte_SetStyle");

    _ulStyle=ulStyle;

}

/*
 *
 *   METHOD:   GetInitializedFlag
 *   PURPOSE:  Get the initialized word
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope BOOL   SOMLINK vptexte_GetInitializedFlag(VPText *somSelf)
{
    VPTextData *somThis = VPTextGetData(somSelf);
    VPTextMethodDebug("VPText","vptexte_GetInitializedFlag");

    return (BOOL) _bInitialized;
}

/*
 *
 *   METHOD:   SetInitializedFlag
 *   PURPOSE:  Set the initialized flag
 *   INVOKED:  From style dialog in editor session
 *
 */

SOM_Scope VOID   SOMLINK vptexte_SetInitializedFlag(VPText *somSelf,
                BOOL bInitialized)
{
    VPTextData *somThis = VPTextGetData(somSelf);
    VPTextMethodDebug("VPText","vptexte_SetInitializedFlag");

    _bInitialized=bInitialized;

}
#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta
SOM_Scope VOID   SOMLINK vptextc_QueryClassInfo(M_VPText *somSelf,
                PVPCLASSINFO pVpClassInfo)
{
    M_VPTextData *somThis = M_VPTextGetData(somSelf);
    M_VPTextMethodDebug("M_VPText","vptextc_QueryClassInfo");

    if (pVpClassInfo) {
       pVpClassInfo->cbSize=sizeof(VPCLASSINFO);
       pVpClassInfo->pszDescName       = "Text Sample";
       pVpClassInfo->pszAbbrevName     = "TXTSMP";
       pVpClassInfo->bHasText          = TRUE;
       pVpClassInfo->pszDefaultText    = "text samp";
       pVpClassInfo->bHasCtrlData      = TRUE;
       pVpClassInfo->pszWindowClass    = szClass;
       pVpClassInfo->lDefaultWidth     = 48;
       pVpClassInfo->lDefaultHeight    = 16;
       pVpClassInfo->ulDefaultStyle    = WS_VISIBLE;
       pVpClassInfo->pDefaultCtrlData  = &defaultData;
       pVpClassInfo->hModResource      = _hModResource;
       pVpClassInfo->lIconResID        = ID_POINTER;
       pVpClassInfo->lStyleDlgID       = ID_TEXTDLG;
       pVpClassInfo->pfnwpStyleDlgProc = StyleDlgProc;
       pVpClassInfo->lToolHelpID       = 100;
       pVpClassInfo->lStylesPageHelpID = 110;
       pVpClassInfo->hwndHelpInstance  = 0;
       pVpClassInfo->lNumEvents        = 0;
       pVpClassInfo->Logic             = Logic;
    } /* endif */
}

SOM_Scope void   SOMLINK vptextc_somInitClass(M_VPText *somSelf,
                IN zString className,
                IN SOMAny *parentClass,
                IN integer4 instanceSize,
                IN int maxStaticMethods,
                IN integer4 majorVersion,
                IN integer4 minorVersion)
{
    M_VPTextData *somThis = M_VPTextGetData(somSelf);
    CHAR ErrorBuffer[100];
    M_VPTextMethodDebug("M_VPText","vptextc_somInitClass");

    DosLoadModule((PSZ) ErrorBuffer, sizeof(ErrorBuffer), "VPText",
                  &_hModResource);
    WinRegisterClass(                     /* Register window class        */
         0,                             /* Anchor block handle          */
         szClass,                       /* Window class name            */
         TestWndProc,              /* Address of window procedure  */
         0,                               /* Class style                  */
         12                                /* No extra window words        */
         );

    parent_somInitClass(somSelf,className,parentClass,instanceSize,
                        maxStaticMethods,majorVersion,minorVersion);
}

SOM_Scope HWND   SOMLINK vptextc_InitHelpInstance(M_VPText *somSelf)
{
    M_VPTextData *somThis = M_VPTextGetData(somSelf);
    HELPINIT hmiHelpData;       /* Help initialization structure     */
    HWND hwndHelpInstance;
    CHAR pszHelp[CCHMAXPATH];
    PSZ pszTemp;

    M_VPTextMethodDebug("M_VPText","vptextc_InitHelpInstance");

    if (DosScanEnv("VISPRORX",&pszTemp)) {
       strcpy(pszHelp,"C:\\VISPRORX");
    } else {
       strcpy(pszHelp,pszTemp);
    } /* endif */
    strcat(pszHelp,"\\VPText.HLP");

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