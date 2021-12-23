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

#if !defined(AFX_ABOUT_H__5588C499_8D0A_4D02_840C_FACEF488B388__INCLUDED_)
#define AFX_ABOUT_H__5588C499_8D0A_4D02_840C_FACEF488B388__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define IDD_ABOUT 109
class CAbout : public CDialog  
{
public:
	CAbout();
	virtual ~CAbout();
	enum { IDD = IDD_ABOUT};
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_ABOUT_H__5588C499_8D0A_4D02_840C_FACEF488B388__INCLUDED_)
