#include "stdafx.h"
#include "CMainWin.h"
#include <string.h>
#include <string>


int ShowBitMap(HWND hWnd, HBITMAP hBit, int x, int y)
//������� ���������� ������� � �������� ������� ���� 
//hWnd - ���������� ����, ���� ��������� ����������� 
//hBit - ���������� ������� 
//(x,y) - ���������� ������ �������� ���� ����������� � ���� ������
{
	//� ��������� BitMap ����������� ��������� �������� 
	BITMAP BitMap;
	GetObject(hBit, sizeof(BITMAP), &BitMap);
	//�� �� ����� bmHeight � bmWidth �������� ������� ��� ����������� 
	int Height = BitMap.bmHeight;
	int Width = BitMap.bmWidth;
	//�������� �������� ����������� 
	HDC ImageDC = GetDC(hWnd);
	//������� �������� ������ 
	HDC MemoryDC = CreateCompatibleDC(ImageDC);
	//� ��������� �������� ������ ������� ���������� ������� �����
	HBITMAP OldBitmap = (HBITMAP)SelectObject(MemoryDC, hBit);
	//���� � �������� ������ ��������� ���� ������  
	//#include<windowsx.h>,�� ���������� hBit �����  
	//�������� � ����������� ������ ����������: 
	//HBITMAP OldBitmap=SelectBitmap(hdcMem,hBit); 
	//�������� hdcMem ����� ������������ ��� ��������� 
	//���������� GDI -����������� ����������� � ������  
	//�������� ��������� �������� ��������� �� ���������� � �������� �������� 
	
	//� ����, � ������� ������ �������� ����������� hdc, 
	// �������� �������������� ������ ������� �����������: 
	BitBlt(ImageDC, x, y, Width, Height, MemoryDC, 0, 0, SRCCOPY );
	//����� ����������� ������������ ��������� ������ ����������� 
	SelectObject(MemoryDC, OldBitmap);
	ReleaseDC(hWnd, ImageDC);
	DeleteDC(MemoryDC);
	return 0;
}



int ClientToBmp(HWND hWnd, RECT& rect, CString str)
//���������� ����� ������� ������� ���� � ����� Name.bmp 
//hWnd - ���������� ����, ������� ������� �������� ����������� 
//Name - ��� ����� ��� ���������� 
{
	BITMAPFILEHEADER bitFile;
	BITMAPINFOHEADER bitInfo;

	RECT ImageRect = rect;

	LONG Width = ImageRect.right - ImageRect.left,
		Height = ImageRect.bottom - ImageRect.top;

	int BitToPixel = 16; //������� ���� - ���-�� ��� �� ��� - ����� High Color (24 - True)
	HDC ImageDC = GetDC(hWnd);
	HDC MemoryDC = CreateCompatibleDC(ImageDC);
	//������� ������� ����� BitMap �� ������� ������� ������� ���� 
	HBITMAP BitMap = CreateCompatibleBitmap(ImageDC, Width, Height);
	HBITMAP OldBitmap = (HBITMAP)SelectObject(MemoryDC, BitMap);

	BitBlt(MemoryDC, 0, 0, Width, Height, ImageDC, ImageRect.left, ImageRect.top, SRCCOPY );
	BitMap = (HBITMAP)SelectObject(MemoryDC, OldBitmap);

	ZeroMemory(&bitInfo, sizeof(BITMAPINFOHEADER)); //��� ������ ������� memset(), 
													//������� ��������� ��������� ������. 

	bitInfo.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.biWidth = Width;
	bitInfo.biHeight = Height;
	bitInfo.biPlanes = 1;
	bitInfo.biBitCount = BitToPixel;
	//� ��������  ����� 16 ��� ���� �����������, 
	//��� 24 - ��� DIB,�� 8 ��� �� ������ �������� ��  
	//����� � �� �������� �����  ��������� �������� ������� 
	//��� ���������� ������� ����������� � ������ �� 
	//����������� �������� �.right * BitToPixel/8 ����  
	//�� ������ �� ��������, �������� �������. ���  
	//���������� ����� ��������� � ������� GetDIBits() 
	bitInfo.biSizeImage = (Width * BitToPixel + 31) / 32 * 4 * Height;

	DWORD dwWritten; //���������� ���������� ������
	//��������� ����
	HANDLE FileHandle = CreateFile(str, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (FileHandle == INVALID_HANDLE_VALUE) 
		return 2;
	
	bitFile.bfType = ('M' << 8) | 'B'; //��������� ���������
	bitFile.bfSize = bitInfo.biSizeImage + sizeof(bitFile) + bitInfo.biSize; // �-� �����
	bitFile.bfReserved1 = bitFile.bfReserved2 = 0;
	bitFile.bfOffBits = sizeof(bitFile) + bitInfo.biSize; 
	//�������� � ������ ������

	//������ ���������
	WriteFile(FileHandle, (LPSTR)&bitFile, sizeof(bitFile), &dwWritten, NULL);
	//������ � ���� ������������ ���������  
	WriteFile(FileHandle, (LPSTR)&bitInfo, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	//�������� ����� � ������ ��� ����,����� ������� 
	//GetDIBits()��������� ���� ���� ����� � DIB-������� 
	char *lp = (char *)GlobalAlloc(GMEM_FIXED, bitInfo.biSizeImage);
	//�� ����� BitMap ������ � ������� �� bi.biHeight 
	//������� ���������� � ������ �� ������� bi (������������� ������) 
	GetDIBits(ImageDC, BitMap, 0, (UINT)Height, lp, (LPBITMAPINFO)&bitInfo, DIB_RGB_COLORS);
	//������ ����������� �� ����
	WriteFile(FileHandle, lp, bitInfo.biSizeImage, &dwWritten, NULL);
	//������������ ������ � �������� �����
	GlobalFree(GlobalHandle(lp));
	CloseHandle(FileHandle);
	ReleaseDC(hWnd, ImageDC);
	DeleteDC(MemoryDC);
	if (dwWritten == 0) 
		return 2;
	return 0;
}


int Screen(HWND hWnd, CString str)
{
	BITMAPFILEHEADER bmfHdr; 
	BITMAPINFOHEADER bi; 
	RECT r; 
	int BitToPixel = 16;
	GetClientRect(hWnd, &r);
	HDC hdc = GetDC(hWnd);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP BitMap = CreateCompatibleBitmap(hdc, r.right, r.bottom); 
	HBITMAP OldBitmap = (HBITMAP)SelectObject(hdcMem, BitMap);
	BitBlt(hdcMem, 0, 0, r.right, r.bottom, hdc, 0, 0, SRCCOPY); 
	BitMap = (HBITMAP)SelectObject(hdcMem, OldBitmap); 
	ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER); 
	bi.biWidth = r.right; 
	bi.biHeight = r.bottom; 
	bi.biPlanes = 1; 
	bi.biBitCount = BitToPixel;
	bi.biSizeImage = (r.right * BitToPixel + 31) / 32 * 4 * r.bottom; 
	DWORD dwWritten;
	HANDLE fh = CreateFile(str, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE) return 2; bmfHdr.bfType = ('M' << 8) | 'B';
	bmfHdr.bfSize = bi.biSizeImage + sizeof(bmfHdr) + bi.biSize;
	bmfHdr.bfReserved1 = bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = sizeof(bmfHdr) + bi.biSize;
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(bmfHdr), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	char *lp = (char *)GlobalAlloc(GMEM_FIXED, bi.biSizeImage);
	GetDIBits(hdc, BitMap, 0, (UINT)r.bottom, lp, (LPBITMAPINFO)&bi, DIB_RGB_COLORS);
	WriteFile(fh, lp, bi.biSizeImage, &dwWritten, NULL);
	GlobalFree(GlobalHandle(lp)); 
	CloseHandle(fh);
	ReleaseDC(hWnd, hdc);
	DeleteDC(hdcMem); 
	if (dwWritten == 0) 
		return 2; 
	return 0;
}

CMainWin::CMainWin()
{
	this->Create(0, (LPCTSTR)L"Hello World", WS_OVERLAPPEDWINDOW, rectDefault, 0, (LPCTSTR)IDR_MAINFRAME);
}

void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	if (opened)
	{
	
		FileDialog = new CFileDialog(TRUE, NULL, NULL, NULL, L" bmp (*.bmp) |*.bmp||", NULL);

		FileDialog->DoModal();

		HBITMAP bit = (HBITMAP)LoadImage(NULL, FileDialog->GetPathName(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		//loadbitmap - �-��� �������� ����������� � ����� � ���������� ��������� ����������� �����������
		//NULL - �������� �� �����
		//IMAGE_BITMAP - ��� �����������
		//0, 0 - ������� (�� ���� ������� � ��������� �����)
		//LR_LOADFROMFILE | LR_CREATEDIBSECTION - ����� (�� �����)/(���������-�����������)

		ShowBitMap(this->m_hWnd, bit, PaintPosition.x, PaintPosition.y);
		delete FileDialog;
		FileDialog = nullptr;
		opened = false;
	} 
	if (saved)
	{
		FileDialog = new CFileDialog(false, L"*.bmp", NULL, NULL, L" bmp (*.bmp) |*.bmp||", NULL);
		FileDialog->DoModal();
		ClientToBmp(m_hWnd, SaveRect, FileDialog->GetPathName());
		delete FileDialog;
		FileDialog = nullptr;
		saved = false;
	}
	if (screen)
	{
		FileDialog = new CFileDialog(false, L"*.bmp", NULL, NULL, L" bmp (*.bmp) |*.bmp||", NULL);
		FileDialog->DoModal();
		Screen(m_hWnd, FileDialog->GetPathName());
		delete FileDialog;
		FileDialog = nullptr;
		screen = false;
	}
}

void CMainWin::OnLButtonDown(UINT Flags, CPoint Location)
{
	SaveRect.left = Location.x;
	SaveRect.top = Location.y;
	PaintPosition = Location;
}
void CMainWin::OnLButtonUp(UINT Flags, CPoint Location)
{
	SaveRect.right = Location.x;
	SaveRect.bottom = Location.y;
	if (SaveRect.left > SaveRect.right)
	{
		LONG buffer = SaveRect.left;
		SaveRect.left = SaveRect.right;
		SaveRect.right = buffer;
	}
	if (SaveRect.top > SaveRect.bottom)
	{
		LONG buffer = SaveRect.top;
		SaveRect.top = SaveRect.bottom;
		SaveRect.bottom = buffer;
	}
	if (SaveRect.bottom - SaveRect.top > 5 && SaveRect.right - SaveRect.left > 5)
		saved = true;
	OnPaint();
}
void CMainWin::Open()
{
	opened = true;
	InvalidateRect(0);
}
void CMainWin::Clear()
{
	InvalidateRect(0);
}
void CMainWin::Scr()
{
	screen = true;
	OnPaint();
}



BEGIN_MESSAGE_MAP( CMainWin, CFrameWnd )
	ON_COMMAND(ID_CLEAR, &CMainWin::Clear)
	ON_COMMAND(ID_TESTS_IMAGE, &CMainWin::Open)
	ON_COMMAND(ID_TESTS_FULLSCREEN, &CMainWin::Scr)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()