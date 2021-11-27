#pragma once
#include "stdafx.h"
#include "resource.h"
enum Test
{
	F1 = 0,
	F2 = 1,
	F3 = 2,
	F4 = 3,
	F1234 = 4
};
class CMainWin : public CFrameWnd
{
private:
	Test task = (Test)-1;
public:
	CMainWin();
	afx_msg void OnPaint();
	afx_msg void F1();
	afx_msg void F2();
	afx_msg void F3();
	afx_msg void F4();
	afx_msg void F1234();
	afx_msg void Clear();
	afx_msg void MyClose();
	void PaintF1(CDC& dc);
	void PaintF2(CDC& dc);
	void PaintF3(CDC& dc);
	void PaintF4(CDC& dc);
	void PaintF1234(CDC& dc);
	DECLARE_MESSAGE_MAP()
};
