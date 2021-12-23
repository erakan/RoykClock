/**
 * RoykClock ~ A desktop clock inspired from Royksopp's awarded "Remind Me" video.
 * Copyright (c) 20 Erdinc AKAN (erakan@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 **/

#include <afxtempl.h>
#include <afxwin.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////////////
// PlaySound Fonksiyonu için ekledim! Ses dosyalarını çalmak için kullanacaksın!
// winmm.lib dosyasına ihtiyacı var, buna ya project-> Settings->link'den
// Yada pragma comment ile buradan eklenecek!
#include "mmsystem.h"		  
#pragma comment(lib, "winmm") 
							  
#include "Resource.h"
#include "TrayIcon.h"
#include "about.h"
#include "Clock.h"

/////////////////////////////////////////////////////////////////
// Global Değişken declarations...
//
#define SQUARESIZE			20
#define ID_TIMER_CLOCK		1

const COLORREF LEDRENK[] = {
	RGB(255,255,0), //Sarı
	RGB(255,23,29), // Kırmızı
	RGB(128,255,0) // Yeşil
};

const COLORREF PANELRENK[] = {
	RGB(128,128,0), //Sarı
	RGB(92,16,18), // Kırmızı
	RGB(31,62,0) //Yeşi
};

CStringArray Alarms;


CMyApp myApp; // The One and Only application object.

/////////////////////////////////////////////////////////////////////////
// CMyApp member functions

BOOL CMyApp::InitInstance ()
{
	SetRegistryKey (_T ("Erdinc AKAN Products"));
	m_pMainWnd = new CMainWindow;
	// SetDialogBkColor(RGB(0,64,128)); //Dialogların bgsini değiştirme
    if (!((CMainWindow*) m_pMainWnd)->RestoreWindowState ())
        m_pMainWnd->ShowWindow (m_nCmdShow);
    m_pMainWnd->UpdateWindow ();
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// CMainWindow message map and member functions
#define WM_MY_TRAY_NOTIFICATION WM_USER+0
BEGIN_MESSAGE_MAP (CMainWindow, CFrameWnd)
    ON_WM_CREATE ()
    ON_WM_PAINT ()
    ON_WM_TIMER ()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
    ON_WM_GETMINMAXINFO ()
    ON_WM_NCHITTEST ()
    ON_WM_SYSCOMMAND ()
    ON_WM_CONTEXTMENU ()
    ON_WM_ENDSESSION ()
	ON_WM_ERASEBKGND()
    ON_WM_CLOSE ()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION, OnTrayNotification)
END_MESSAGE_MAP ()

CMainWindow::CMainWindow () : m_TrayIcon(IDI_TRAYICON)
{
    m_bAutoMenuEnable = FALSE;

    CTime time = CTime::GetCurrentTime ();
    m_nPrevSecond = time.GetSecond ();
    m_nPrevMinute = time.GetMinute ();
    m_nPrevHour = time.GetHour () % 12;
	m_bLoading = TRUE; //One and only loading constracture
	m_hIcon = myApp.LoadCursor(IDC_HAND);
	m_AlarmPlaying = FALSE;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.hCursor = myApp.LoadCursor(IDC_HAND);
	wc.hIcon = myApp.LoadIcon (IDI_APPICON);
	wc.lpfnWndProc = AfxWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = AfxGetInstanceHandle();
	wc.lpszClassName = "Erdinc AKAN Clock";

	AfxRegisterClass(&wc);
	Create (wc.lpszClassName, _T ("Erdinc AKAN Clock"));

    PlaySound((LPCTSTR) IDR_KICKASS, NULL, 
			  SND_RESOURCE | SND_ASYNC | SND_SYNC);
}

BOOL CMainWindow::PreCreateWindow (CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow (cs))
        return FALSE;

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    return TRUE;
}

int CMainWindow::OnCreate (LPCREATESTRUCT lpcs)
{
    if (CFrameWnd::OnCreate (lpcs) == -1)
        return -1;

	//
	// Set a timer to fire at 1-second intervals.
	//
    if (!SetTimer (ID_TIMER_CLOCK, 1000, NULL)) {
        MessageBox (_T ("SetTimer failed"), _T ("Error"),
		    MB_ICONSTOP | MB_OK);
        return -1;
    }

	//
	// Customize the system menu.
	//
    CMenu* pMenu = GetSystemMenu (FALSE);
    pMenu->AppendMenu (MF_SEPARATOR);
    pMenu->AppendMenu (MF_STRING, IDM_SYSMENU_FULL_WINDOW,
        _T ("Remove &Title"));
    pMenu->AppendMenu (MF_STRING, IDM_SYSMENU_STAY_ON_TOP,
        _T ("Stay on To&p"));
	pMenu->AppendMenu(MF_STRING, IDM_SYSMENU_SHOWINTRAY, "Show in tray &only");
	
	CMenu Thema;
	Thema.CreatePopupMenu();
	Thema.AppendMenu(MF_STRING , IDM_SYSMENU_REDTEMA,
					"Red Led");
	Thema.AppendMenu(MF_STRING, IDM_SYSMENU_YELLOWTEMA,
					"Yellow Led");
	Thema.AppendMenu(MF_STRING, IDM_SYSMENU_GREENTEMA,
					"Green Led");
	pMenu->AppendMenu(MF_POPUP, (UINT) Thema.Detach(),
					"Color Theme");
	
	pMenu->AppendMenu(MF_SEPARATOR);	
	pMenu->AppendMenu(MF_STRING | MFS_GRAYED, IDM_SYSMENU_STOPALARM,
					"Stop Alarm!");
	pMenu->AppendMenu(MF_STRING, IDM_SYSMENU_ALARM,
					"Add/Remove Alarms...");
	
	pMenu->AppendMenu(MF_SEPARATOR);
	pMenu->AppendMenu(MF_STRING, IDM_SYSMENU_HAKKINDA,
					"Hakkında (Means: About)...");
	return 0;
}

void CMainWindow::OnClose ()
{
    SaveWindowState ();
    if(m_AlarmPlaying)
		Alarm(FALSE);
	KillTimer (ID_TIMER_CLOCK);
    CFrameWnd::OnClose ();
}

void CMainWindow::OnEndSession (BOOL bEnding)
{
    if (bEnding)
        SaveWindowState ();
    CFrameWnd::OnEndSession (bEnding);
}

void CMainWindow::OnGetMinMaxInfo (MINMAXINFO* pMMI)
{
    pMMI->ptMinTrackSize.x = 120;
    pMMI->ptMinTrackSize.y = 120;
}

UINT CMainWindow::OnNcHitTest (CPoint point)
{
    UINT nHitTest = CFrameWnd::OnNcHitTest (point);
    if ((nHitTest == HTCLIENT) && (::GetAsyncKeyState (MK_LBUTTON) < 0))
        nHitTest = HTCAPTION;
    return nHitTest;
}

void CMainWindow::OnSysCommand (UINT nID, LPARAM lParam)
{
    UINT nMaskedID = nID & 0xFFF0;
	CMenu *pMenu = GetSystemMenu(FALSE);
	if( nID == IDM_SYSMENU_HAKKINDA)
	{
		CAbout Aboutdlg;
		PlaySound((LPCTSTR) IDR_FALLOW, NULL, SND_RESOURCE 
				 | SND_ASYNC | SND_SYNC);
		Aboutdlg.DoModal();
		return;
	}
	
	if(nID==IDM_SYSMENU_ALARM)
	{
		CAlarms AlarmsDlg;
		AlarmsDlg.DoModal();
		return;
	}
	
	if(nID==IDM_SYSMENU_STOPALARM)
	{
		Alarm(FALSE);
		return;
	}
	
	switch(nMaskedID)
	{
	case IDM_SYSMENU_FULL_WINDOW:
        m_bFullWindow = m_bFullWindow ? 0 : 1;
        SetTitleBarState ();
        return;
		break;
	case IDM_SYSMENU_STAY_ON_TOP:
        m_bStayOnTop = m_bStayOnTop ? 0 : 1;
        SetTopMostState ();
        return;
		break;
	case IDM_SYSMENU_SHOWINTRAY:
		m_bShowInTray = m_bShowInTray ? 0 : 1;
		SetShowInTray();
		return;
		break;
	case IDM_SYSMENU_YELLOWTEMA:
		m_SeciliTema = 0;
		Invalidate();
		pMenu->CheckMenuItem(IDM_SYSMENU_YELLOWTEMA, MF_CHECKED);
		pMenu->CheckMenuItem(IDM_SYSMENU_REDTEMA, MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SYSMENU_GREENTEMA, MF_UNCHECKED);
		return;
		break;
	case IDM_SYSMENU_REDTEMA:
		m_SeciliTema = 1;
		Invalidate();
		pMenu->CheckMenuItem(IDM_SYSMENU_YELLOWTEMA, MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SYSMENU_REDTEMA, MF_CHECKED);
		pMenu->CheckMenuItem(IDM_SYSMENU_GREENTEMA, MF_UNCHECKED);
		return;
		break;
	case IDM_SYSMENU_GREENTEMA:
		m_SeciliTema = 2;
		Invalidate();
		pMenu->CheckMenuItem(IDM_SYSMENU_YELLOWTEMA, MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SYSMENU_REDTEMA, MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SYSMENU_GREENTEMA, MF_CHECKED);
		return;
		break;
	}
    CFrameWnd::OnSysCommand (nID, lParam);
}

void CMainWindow::OnContextMenu (CWnd* pWnd, CPoint point)
{
    CRect rect;
    GetClientRect (&rect);
    ClientToScreen (&rect);

    if (rect.PtInRect (point)) {
        CMenu* pMenu = GetSystemMenu (FALSE);
        UpdateSystemMenu (pMenu);

        int nID = (int) pMenu->TrackPopupMenu (TPM_LEFTALIGN |
            TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x,
            point.y, this);

        if (nID > 0)
            SendMessage (WM_SYSCOMMAND, nID, 0);

        return;
    }
    CFrameWnd::OnContextMenu (pWnd, point);
}

void CMainWindow::OnTimer (UINT nTimerID)
{
    CTime time = CTime::GetCurrentTime ();
    int nSecond = time.GetSecond ();
    int nMinute = time.GetMinute ();
    int nHour = time.GetHour () % 12;

    if (IsIconic ())
	{
		CString time;
		time.Format("    %0.2d:%0.2d:%0.2d   ", nHour, nMinute, nSecond);
		SetWindowText(time);
        return;
	}
	else
		SetWindowText("Erdinc AKAN Clock");


    if ((nSecond == m_nPrevSecond) &&
        (nMinute == m_nPrevMinute) &&
        (nHour == m_nPrevHour))
        return;

    CRect rect;
    GetClientRect (&rect);

    CClientDC dc (this);
	dc.SetMapMode (MM_ISOTROPIC);
    dc.SetWindowExt (1000, 1000);
    dc.SetViewportExt (rect.Width (), -rect.Height ());
    dc.SetViewportOrg (rect.Width () / 2, rect.Height () / 2);

    COLORREF clrColor = ::GetSysColor (COLOR_3DFACE);
	CString saat;
	saat.Format("%0.2d",nSecond);
	dc.DrawText(saat, CRect(-100,-100,100,100), DT_CENTER);

    if (nMinute != m_nPrevMinute) {
        m_nPrevMinute = nMinute;
        m_nPrevHour = nHour;
		KadranCiz(&dc);
		saat.Format("%0.2d:%0.2d", time.GetHour(), nMinute);
		if(AlarmCheck(saat))
			Alarm(TRUE);
    }

	if (nSecond != m_nPrevSecond) {
        DrawSecondHand (&dc, 450, 8, nSecond * 6, RGB (0, 0, 0));
        m_nPrevSecond = nSecond;
		if(m_AlarmPlaying)
			AlarmTrayAnim();
    }

}

void CMainWindow::OnPaint ()
{
    CRect rect;
    GetClientRect (&rect);

    CPaintDC dc (this);
	dc.SetMapMode (MM_ISOTROPIC);
    dc.SetWindowExt (1000, 1000);
    dc.SetViewportExt (rect.Width (), -rect.Height ());
    dc.SetViewportOrg (rect.Width () / 2, rect.Height () / 2);

    DrawClockFace (&dc);
    DrawSecondHand (&dc, 450, 8, m_nPrevSecond * 6, RGB (0, 0, 0));
	Markala(&dc);
}

void CMainWindow::DrawClockFace (CDC* pDC)
{
    CBrush br;
	CFont mFont;
	CPen pen;
	CRect r(370,150,-370,-150);

	pen.CreatePen(PS_SOLID, 10, RGB(0,0,0));
	br.CreateSolidBrush(PANELRENK[m_SeciliTema]);

	pDC->SelectObject(&br);
	CPen *pOldPen = pDC->SelectObject(&pen);

	pDC->Rectangle(r);
	pDC->DrawEdge(r, EDGE_ETCHED, BF_RECT);
	pDC->SelectObject(pOldPen);

	KadranCiz(pDC);
   
}

void CMainWindow::DrawSecondHand (CDC* pDC, int nLength, int nScale,
    int nDegrees, COLORREF clrColor)
{
    
	CPoint point;
	CBrush br;
    double nRadians = (double) nDegrees * 0.017453292;

    point.x = (int) (nLength * sin (nRadians));
    point.y = (int) (nLength * cos (nRadians));

	br.CreateSolidBrush(LEDRENK[m_SeciliTema]);
	CBrush *pOldBrush = pDC->SelectObject(&br);

	pDC->Ellipse(point.x - SQUARESIZE,point.y + SQUARESIZE,
		point.x + SQUARESIZE, point.y - SQUARESIZE);

    pDC->SelectObject (pOldBrush);
}

void CMainWindow::SetTitleBarState ()
{
    CMenu* pMenu = GetSystemMenu (FALSE);

    if (m_bFullWindow ) {
        ModifyStyle (WS_CAPTION, 0);
        pMenu->ModifyMenu (IDM_SYSMENU_FULL_WINDOW, MF_STRING,
            IDM_SYSMENU_FULL_WINDOW, _T ("Restore &Title"));
    }
    else {
        ModifyStyle (0, WS_CAPTION);
        pMenu->ModifyMenu (IDM_SYSMENU_FULL_WINDOW, MF_STRING,
            IDM_SYSMENU_FULL_WINDOW, _T ("Remove &Title"));
    }
    SetWindowPos (NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE |
        SWP_NOZORDER | SWP_DRAWFRAME);
}

void CMainWindow::SetTopMostState ()
{
    CMenu* pMenu = GetSystemMenu (FALSE);

    if (m_bStayOnTop) {
        SetWindowPos (&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        pMenu->CheckMenuItem (IDM_SYSMENU_STAY_ON_TOP, MF_CHECKED);
    }       
    else {
        SetWindowPos (&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        pMenu->CheckMenuItem (IDM_SYSMENU_STAY_ON_TOP, MF_UNCHECKED);
    }
}

BOOL CMainWindow::RestoreWindowState ()
{
    CString version = _T ("Version 1.0");
	m_bFullWindow = myApp.GetProfileInt (version, _T ("FullWindow"), 0);
    SetTitleBarState ();
    m_bStayOnTop = myApp.GetProfileInt (version, _T ("StayOnTop"), 0);
    SetTopMostState ();
	m_bShowInTray = myApp.GetProfileInt(version, "ShowInTray",0);
	SetShowInTray ();
	m_bLoading = FALSE;
	m_SeciliTema = myApp.GetProfileInt(version, "SelectedTheme", 1);
	CMenu *pMenu = GetSystemMenu(FALSE);
	pMenu->CheckMenuItem(IDM_SYSMENU_YELLOWTEMA, (m_SeciliTema==0)?MF_CHECKED:MF_UNCHECKED);
	pMenu->CheckMenuItem(IDM_SYSMENU_REDTEMA,    (m_SeciliTema==1)?MF_CHECKED:MF_UNCHECKED);
	pMenu->CheckMenuItem(IDM_SYSMENU_GREENTEMA,  (m_SeciliTema==2)?MF_CHECKED:MF_UNCHECKED);

	//Alarm Listesi Load Hususu
	CFileStatus filedurum;
	if(CFile::GetStatus(filename, filedurum))
	{
		if(filedurum.m_size >0)
		{
			CFile file(filename,CFile::modeRead 
					  | CFile::modeNoTruncate);
			CArchive ar( &file, CArchive::load);
			Alarms.Serialize(ar);
			ar.Close();
			file.Close(); //*/
		}
	}

    WINDOWPLACEMENT wp;
    wp.length = sizeof (WINDOWPLACEMENT);
    GetWindowPlacement (&wp);

    if (((wp.flags =
            myApp.GetProfileInt (version, _T ("flags"), -1)) != -1) &&
        ((wp.showCmd =
            myApp.GetProfileInt (version, _T ("showCmd"), -1)) != -1) &&
        ((wp.rcNormalPosition.left =
            myApp.GetProfileInt (version, _T ("x1"), -1)) != -1) &&
        ((wp.rcNormalPosition.top =
            myApp.GetProfileInt (version, _T ("y1"), -1)) != -1) &&
        ((wp.rcNormalPosition.right =
            myApp.GetProfileInt (version, _T ("x2"), -1)) != -1) &&
        ((wp.rcNormalPosition.bottom =
            myApp.GetProfileInt (version, _T ("y2"), -1)) != -1)) {

        wp.rcNormalPosition.left = min (wp.rcNormalPosition.left,
            ::GetSystemMetrics (SM_CXSCREEN) -
            ::GetSystemMetrics (SM_CXICON));
        wp.rcNormalPosition.top = min (wp.rcNormalPosition.top,
            ::GetSystemMetrics (SM_CYSCREEN) -
            ::GetSystemMetrics (SM_CYICON));

        SetWindowPlacement (&wp);
        return TRUE;
    }
    return FALSE;
}

void CMainWindow::SaveWindowState ()
{
    CString version = _T ("Version 1.0");
    myApp.WriteProfileInt (version, _T ("FullWindow"), m_bFullWindow);
    myApp.WriteProfileInt (version, _T ("StayOnTop"), m_bStayOnTop);
	myApp.WriteProfileInt(version, "ShowInTray", m_bShowInTray);
	myApp.WriteProfileInt(version, "SelectedTheme", m_SeciliTema);

    WINDOWPLACEMENT wp;
    wp.length = sizeof (WINDOWPLACEMENT);
    GetWindowPlacement (&wp);

    myApp.WriteProfileInt (version, _T ("flags"), wp.flags);
    myApp.WriteProfileInt (version, _T ("showCmd"), wp.showCmd);
    myApp.WriteProfileInt (version, _T ("x1"), wp.rcNormalPosition.left);
    myApp.WriteProfileInt (version, _T ("y1"), wp.rcNormalPosition.top);
    myApp.WriteProfileInt (version, _T ("x2"), wp.rcNormalPosition.right);
    myApp.WriteProfileInt (version, _T ("y2"), wp.rcNormalPosition.bottom);

	//Alarm Listesi Save Hususu
	CFile file(filename,CFile::modeWrite 
			  | CFile::modeCreate 
			  | CFile::modeNoTruncate);
	CArchive ar( &file, CArchive::store);
	Alarms.Serialize(ar);
	ar.Close();
	file.Close();
}

void CMainWindow::UpdateSystemMenu (CMenu* pMenu)
{
    static UINT nState[3][5] = {
        { MFS_GRAYED,  MFS_ENABLED, MFS_ENABLED,
          MFS_ENABLED, MFS_DEFAULT },
        { MFS_DEFAULT, MFS_GRAYED,  MFS_GRAYED,
          MFS_ENABLED, MFS_GRAYED  },
        { MFS_ENABLED, MFS_GRAYED,  MFS_GRAYED,
          MFS_GRAYED,  MFS_ENABLED  }
    };


    int i = 0;
    if (IsZoomed ())
        i = 1;
    if (IsIconic () || !IsWindowVisible())
        i = 2;

    CString strMenuText;
    pMenu->GetMenuString (SC_RESTORE, strMenuText, MF_BYCOMMAND);
    pMenu->ModifyMenu (SC_RESTORE, MF_STRING | nState[i][0], SC_RESTORE,
        strMenuText);

    pMenu->GetMenuString (SC_MOVE, strMenuText, MF_BYCOMMAND);
    pMenu->ModifyMenu (SC_MOVE, MF_STRING | nState[i][1], SC_MOVE,
        strMenuText);

    pMenu->GetMenuString (SC_SIZE, strMenuText, MF_BYCOMMAND);
    pMenu->ModifyMenu (SC_SIZE, MF_STRING | nState[i][2], SC_SIZE,
        strMenuText);

    pMenu->GetMenuString (SC_MINIMIZE, strMenuText, MF_BYCOMMAND);
    pMenu->ModifyMenu (SC_MINIMIZE, MF_STRING | nState[i][3], SC_MINIMIZE,
        strMenuText);

    pMenu->GetMenuString (SC_MAXIMIZE, strMenuText, MF_BYCOMMAND);
    pMenu->ModifyMenu (SC_MAXIMIZE, MF_STRING | nState[i][4], SC_MAXIMIZE,
        strMenuText);

    SetMenuDefaultItem (pMenu->m_hMenu, i ? SC_RESTORE :
        SC_MAXIMIZE, FALSE);
}

void CMainWindow::OnLButtonDblClk(UINT, CPoint)
{
	m_bFullWindow = m_bFullWindow ? 0 : 1;
	SetTitleBarState ();
}

void CMainWindow::GetBitmapNum(char Num, BITMAP *bm, CBitmap *bmp, CDC *dcMem, CDC *pDC)
{
	CFont mFont;

	if (bmp->DeleteObject())
		bmp->Detach();
	
	bmp->CreateCompatibleBitmap(pDC,300,300);	
    dcMem->SelectObject (bmp);

	dcMem->FillSolidRect(CRect(0,0,300,300), PANELRENK[m_SeciliTema]);
	dcMem->SetBkColor(PANELRENK[m_SeciliTema]);
	
	mFont.CreateFont( 440, 250, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
				OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, "Ms Sans Serif");
	dcMem->SelectObject(&mFont);

	dcMem->SetTextColor(LEDRENK[m_SeciliTema]);
	dcMem->DrawText(Num,CRect(0,-70,300,300),DT_CENTER);

    bmp->GetBitmap (bm);

}

void CMainWindow::KadranCiz(CDC *pDC)
{
    CBrush Yanik, Sonuk;
    CBitmap bitmap;
    CDC dcMem;
    BITMAP bm;
	CTime time= CTime::GetCurrentTime();
	CString saat;
	CPoint point;

	Sonuk.CreateSolidBrush(PANELRENK[m_SeciliTema]);
	Yanik.CreateSolidBrush(LEDRENK[m_SeciliTema]);


	int nSeconds = time.GetSecond();
	int nMinutes = time.GetMinute();
	int nHours = time.GetHour() % 12;

    dcMem.CreateCompatibleDC (pDC);
	saat.Format("%0.2d%0.2d",nHours ,nMinutes);

    GetBitmapNum(':', &bm, &bitmap, &dcMem, pDC);
	pDC->StretchBlt(-110,140, 145, -280, 
					&dcMem, 0, 0, bm.bmWidth , bm.bmHeight + 40, SRCCOPY);

    //Saat X_:__
	GetBitmapNum(saat.GetAt(0), &bm, &bitmap, &dcMem, pDC);
	pDC->StretchBlt(-355,140, 140, -280, 
					&dcMem, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);
	
	//Saat _X:__
    GetBitmapNum(saat.GetAt(1), &bm, &bitmap, &dcMem, pDC);
    pDC->StretchBlt(-200,140, 140, -280, 
					&dcMem, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);

	//Saat __:X_
    GetBitmapNum(saat.GetAt(2), &bm, &bitmap, &dcMem, pDC);
    pDC->StretchBlt(-10,140, 140, -280, 
					&dcMem, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);
	//Saat __:_X
    GetBitmapNum(saat.GetAt(3), &bm, &bitmap, &dcMem, pDC);
    pDC->StretchBlt(150,140, 140, -280, 
					&dcMem, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);


    if ((time.GetHour())< 12)
	{
		GetBitmapNum('a', &bm, &bitmap, &dcMem, pDC);
		pDC->StretchBlt(280,140, 80, -100, 
						&dcMem, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	else
	{
		GetBitmapNum('P', &bm, &bitmap, &dcMem, pDC);
		pDC->StretchBlt(280, -30, 80, -100, 
						&dcMem, 0, 0,bm.bmWidth, bm.bmHeight, SRCCOPY);
	}


	for(int i=0; i<60;i++)
	{
		double nRadians = (double) (i*6) * 0.017453292;

		point.x = (int) (450 * sin (nRadians));
		point.y = (int) (450 * cos (nRadians));

		if(i<nSeconds)
			pDC->SelectObject(&Yanik);
		else
			pDC->SelectObject(&Sonuk);
		
		pDC->Ellipse(point.x - SQUARESIZE,point.y + SQUARESIZE,
			point.x + SQUARESIZE, point.y - SQUARESIZE);
	}

}

LRESULT CMainWindow::OnTrayNotification(WPARAM uID, LPARAM lEvent)
{
	if (lEvent == WM_RBUTTONDOWN || lEvent == WM_RBUTTONUP)
	{
		CPoint mouse;
		GetCursorPos(&mouse);
		CMenu *pMenu = GetSystemMenu(FALSE);
		UpdateSystemMenu(pMenu);
		::SetForegroundWindow(GetSafeHwnd());
        int nID = (int) pMenu->TrackPopupMenu (TPM_LEFTALIGN |
            TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, mouse.x,
            mouse.y, this);

        if (nID > 0)
            SendMessage (WM_SYSCOMMAND, nID, 0);
		return 1;
	}
	if(lEvent == WM_LBUTTONUP)
	{
		if(!IsWindowVisible())
			SendMessage(WM_SYSCOMMAND, SC_RESTORE,0 );
		::SetForegroundWindow(GetSafeHwnd());
	}
		
	return m_TrayIcon.OnTrayNotification(uID, lEvent);
}

void CMainWindow::SetShowInTray()
{
   	CMenu *pMenu = GetSystemMenu(FALSE);
	if (!m_bLoading)
			ShowWindow(SW_HIDE);

	if(m_bShowInTray)
	{
		m_TrayIcon.SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);
		m_TrayIcon.SetIcon(IDI_TRAYICON);
		ModifyStyleEx( 0, WS_EX_TOOLWINDOW);
	} 
	else
	{
		ModifyStyleEx( WS_EX_TOOLWINDOW, 0);
		m_TrayIcon.RemoveIcon();
	}
	pMenu->CheckMenuItem(IDM_SYSMENU_SHOWINTRAY, 
						 m_bShowInTray ? MF_CHECKED : MF_UNCHECKED);

	if (!m_bLoading)
			ShowWindow(SW_SHOW);

}

void CMainWindow::OnSize(UINT nType, int cx, int cy)
{
	if (!m_bLoading)
	{
		if((nType == SIZE_RESTORED || nType == SIZE_MINIMIZED ||
			nType == SIZE_MAXIMIZED) && m_bShowInTray )
			ShowWindow(SW_SHOW);
		if ((nType == SIZE_MINIMIZED) && m_bShowInTray)
			ShowWindow(SW_HIDE);
	}
	CFrameWnd::OnSize(nType, cx, cy);
}

BOOL CMainWindow::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
	switch(nHitTest)
	{
	case HTBORDER:
	case HTBOTTOM:
	case HTBOTTOMLEFT:
	case HTBOTTOMRIGHT:
	case HTRIGHT:
	case HTSIZE:
	case HTSYSMENU:
	case HTTOP:
	case HTTOPLEFT:
	case HTTOPRIGHT:
	case HTLEFT:
	case HTMENU:
		SetCursor(myApp.LoadCursor(IDC_TUT));
		break;
	default:
		SetCursor(m_hIcon);
		;
	}
	return -1;
}

void CMainWindow::Markala(CDC *pDC)
{
	CFont mFont;
	CBitmap bmp;
	CDC dcMem;
	BITMAP bm;
	dcMem.CreateCompatibleDC(pDC);
		
	bmp.CreateCompatibleBitmap(pDC,600,300);
	dcMem.SelectObject (&bmp);

	dcMem.FillSolidRect(CRect(0,0,600,300), RGB(0,64,128));
	dcMem.SetBkColor(RGB(0,64,128));

	mFont.CreateFont( 120, 0, 0, 0, FW_BOLD, 0, 0, 0,DEFAULT_CHARSET,
				OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE,"Ms Sans Serif");
	dcMem.SelectObject(&mFont);

	dcMem.SetTextColor(RGB(0,0,0));
	dcMem.DrawText("Erdinç AKAN\nQuality Quartz",
					CRect(0,0,600,600), DT_CENTER | DT_VCENTER);

    bmp.GetBitmap (&bm);

	pDC->SetStretchBltMode(BLACKONWHITE);
	pDC->StretchBlt(-160,-240, 300, -100, 
					&dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
}

CAlarms::CAlarms() : CDialog(CAlarms::IDD)
{

}
BOOL CAlarms::OnInitDialog()
{
	CRect r;
	
	GetDlgItem(IDC_LIST)->GetWindowRect(r);
	ScreenToClient(r);
	GetDlgItem(IDC_LIST)->DestroyWindow();

	m_AlarmList.Create(WS_CHILD | WS_VISIBLE |WS_VSCROLL | WS_TABSTOP | LBS_SORT | 
					   LBS_NOINTEGRALHEIGHT |  WS_VSCROLL | LBS_NOTIFY | LBS_DISABLENOSCROLL |
                       WS_TABSTOP, r, this, IDC_LIST);
	
	m_AlarmList.ModifyStyle( WS_BORDER, 0);
	m_AlarmList.ModifyStyleEx( 0,WS_EX_TRANSPARENT | 
							  WS_EX_STATICEDGE | WS_EX_LEFTSCROLLBAR |
							  WS_EX_PALETTEWINDOW);
	
	m_AlarmList.SetFont(GetFont());
	m_AlarmList.SetFocus();
	
	for(int i=0; i<Alarms.GetSize(); i++)
		m_AlarmList.AddString(Alarms[i]);
	
	CWnd *pDTSelect = GetDlgItem(IDC_ALARMTIME);
	pDTSelect->SendMessage(DTM_SETFORMAT, 0, (DWORD) "  HH:mm");
	return TRUE;
}
BEGIN_MESSAGE_MAP(CAlarms, CDialog)
	ON_BN_CLICKED(IDC_SETALARM, OnSetAlarm)
	ON_LBN_DBLCLK(IDC_LIST, OnAlarmSil)
END_MESSAGE_MAP()

void CAlarms::OnSetAlarm()
{
	CString ToAddAlarm;
	GetDlgItem(IDC_ALARMTIME)->GetWindowText(ToAddAlarm);
	ToAddAlarm.TrimLeft(" ");
	
	//Bu Alarm Var mı kontrol edelim...
	for (int i=0; i<Alarms.GetSize();i++)
	{
		if(Alarms[i] == ToAddAlarm)
		{
			//Alarm zaten var!
			MessageBox(Alarms[i] + " already added!\nYou can not add same alarms twice!" ,
					   "Add Alarms", MB_OK | MB_ICONASTERISK );
			return; // Geri dön
		}
	}
	// Alarmı ekle
	Alarms.Add(ToAddAlarm);
	m_AlarmList.AddString(ToAddAlarm);
	Alarms.FreeExtra(); //Memory'i boşaltalım...
}

void CAlarms::OnAlarmSil()
{
	if(m_AlarmList.GetCurSel() == LB_ERR) // Olamaz ama "lets be safe"
		return;
	CString ToDeleteAlarm;
	m_AlarmList.GetText( m_AlarmList.GetCurSel(), ToDeleteAlarm);
	for(int i=0; i<Alarms.GetSize(); i++)
		if(Alarms[i] ==  ToDeleteAlarm)
			break;
	Alarms.RemoveAt(i);
	m_AlarmList.DeleteString(m_AlarmList.GetCurSel());
}

BOOL CMainWindow::AlarmCheck(CString Saat)
{
	if(m_AlarmPlaying) 
		return FALSE;

	for(int i=0; i<Alarms.GetSize(); i++)
		if(Alarms[i] == Saat)
		{
			m_AlarmPlaying = TRUE;
			return TRUE;
		}
	

	return FALSE;

}

void CMainWindow::Alarm(BOOL bBaslat)
{
	if(bBaslat)
	{
			PlaySound((LPCTSTR) IDR_ALARM,NULL,
					  SND_LOOP | SND_RESOURCE | SND_ASYNC);
	}
	else
	{
			m_AlarmPlaying = FALSE;
			PlaySound(NULL,NULL, SND_LOOP);
			m_TrayIconDU = FALSE;
			AlarmTrayAnim();
	}

	CMenu *pMenu=GetSystemMenu(FALSE);
	pMenu->ModifyMenu(IDM_SYSMENU_STOPALARM, MF_STRING | 
					 (m_AlarmPlaying ? MFS_ENABLED:MFS_GRAYED),
					 IDM_SYSMENU_STOPALARM, (m_AlarmPlaying) ? "STOP ALARM!": 
					 "Stop Alarm!");
}

void CMainWindow::AlarmTrayAnim()
{
	if(m_TrayIconDU)
		m_TrayIconDU = FALSE;
	else
		m_TrayIconDU = TRUE;

	if(m_bShowInTray)
		m_TrayIcon.SetIcon(m_TrayIconDU?IDI_TRAYICON:IDI_TRAYICON_D);
	else
		FlashWindow(FALSE);
}

void CAlarms::OnOK()
{
	CWnd *pWnd= GetFocus();
	if(IDOK== pWnd->GetDlgCtrlID())
	{
		CDialog::OnOK();
	}
	else
	{
		NextDlgCtrl();
	}
}

BOOL CMainWindow::OnEraseBkgnd(CDC *pDC)
{
	// Set brush to desired background color
	CBrush backBrush(RGB(0,64,128));

	// Save old brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);

	CRect rect;
	pDC->GetClipBox(&rect);     // Erase the area needed

	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
	  PATCOPY);
	pDC->SelectObject(pOldBrush);
	return TRUE;
}
