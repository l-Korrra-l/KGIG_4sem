#pragma once
#include "stdafx.h"
#include "resource.h"
class CMainWin : public CFrameWnd
{
private:
	RECT SaveRect;
	bool saved = false, opened = false, screen = false;;
	CFileDialog *FileDialog;
	CPoint PaintPosition;
public:
	CMainWin();
	afx_msg void OnPaint();
	afx_msg void Open();
	afx_msg void OnLButtonDown(UINT Flags, CPoint Location);
	afx_msg void OnLButtonUp(UINT Flags, CPoint Location);
	afx_msg void Clear();
	afx_msg void Scr();
	DECLARE_MESSAGE_MAP()
};
