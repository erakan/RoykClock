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

#if !defined(AFX_TRAYICON_H__1D38775B_7B92_4032_8203_207F8CA0CBEA__INCLUDED_)
#define AFX_TRAYICON_H__1D38775B_7B92_4032_8203_207F8CA0CBEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _TRAYICON_H
#define _TRAYICON_H
#endif


class CTrayIcon : public CCmdTarget   
{
protected:
	DECLARE_DYNAMIC(CTrayIcon)
	NOTIFYICONDATA m_nid;         // struct for Shell_NotifyIcon args
public:
	BOOL SetIcon(UINT nID, CString lpTip);
	BOOL SetIcon(LPCTSTR lpResName, LPCSTR lpTip);
	BOOL RemoveIcon();
	CTrayIcon(UINT nID);
	virtual ~CTrayIcon();
   // Call this to receive tray notifications
	void SetNotificationWnd(CWnd* pNotifyWnd, UINT uCbMsg);
	// SetIcon functions. To remove icon, call SetIcon(0)   //   
	BOOL SetIcon(UINT uID, BOOL bLoadFromRes = NULL /* Resoruce'dan load etme */);
	// main variant you want to use   
	BOOL SetIcon(HICON hicon, LPCSTR lpTip);   
	BOOL SetStandardIcon(LPCTSTR lpszIconName, LPCSTR lpTip)
	{ return SetIcon(::LoadIcon(NULL, lpszIconName), lpTip); }
	virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);
};

#endif // !defined(AFX_TRAYICON_H__1D38775B_7B92_4032_8203_207F8CA0CBEA__INCLUDED_)
