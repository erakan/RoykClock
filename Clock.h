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

#include "TrayIcon.h"	// Added by ClassView
class CMyApp : public CWinApp
{
public:
    virtual BOOL InitInstance ();
};

class CMainWindow : public CFrameWnd
{
protected:
    BOOL m_bFullWindow;
    BOOL m_bStayOnTop;

    int m_nPrevSecond;		
    int m_nPrevMinute;
    int m_nPrevHour;

    void DrawClockFace (CDC* pDC);
    void DrawSecondHand (CDC* pDC, int nLength, int nScale, int nDegrees,
	    COLORREF clrColor);

    void SetTitleBarState ();
    void SetTopMostState ();
    void SaveWindowState ();
    void UpdateSystemMenu (CMenu* pMenu);

public:
    CMainWindow ();

	CTrayIcon m_TrayIcon;
    
	virtual BOOL PreCreateWindow (CREATESTRUCT& cs);
    BOOL RestoreWindowState ();

protected:
	BOOL m_TrayIconDU;
	BOOL m_AlarmPlaying;
	BOOL m_bShowInTray;
	BOOL m_bLoading;
	int m_SeciliTema;
	HICON m_hIcon;

	BOOL AlarmCheck(CString Saat);
	LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);
	void AlarmTrayAnim();
	void Alarm(BOOL bBaslat=TRUE);
	void Markala(CDC *pDC);
	void SetShowInTray();
	void KadranCiz(CDC *pDC);
	void GetBitmapNum(char Num, BITMAP *bm, CBitmap *bmp, CDC *dcMem, CDC *pDC);

    afx_msg int OnCreate (LPCREATESTRUCT lpcs);
	afx_msg void OnSize( UINT nType, int cx, int cy );
    afx_msg void OnGetMinMaxInfo (MINMAXINFO* pMMI);
    afx_msg void OnTimer (UINT nTimerID);
    afx_msg void OnPaint ();
    afx_msg void OnSysCommand (UINT nID, LPARAM lParam);
    afx_msg void OnContextMenu (CWnd* pWnd, CPoint point);
    afx_msg void OnEndSession (BOOL bEnding);
    afx_msg void OnClose ();
	afx_msg void OnLButtonDblClk( UINT, CPoint );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
    afx_msg UINT OnNcHitTest (CPoint point);

    DECLARE_MESSAGE_MAP ()
};

class CAlarms : public CDialog
{
public:
	enum { IDD = ID_ALARM};
	CAlarms();

protected:
	CListBox m_AlarmList;

	void OnOK();

	afx_msg void OnAlarmSil();
	afx_msg void OnSetAlarm();
	afx_msg BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};