// FileRenderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "FileRenderDlg.h"
#include "afxdialogex.h"


// FileRenderDlg dialog

IMPLEMENT_DYNAMIC(FileRenderDlg, CDialogEx)

FileRenderDlg::FileRenderDlg(int default_pic_width, int default_pic_height, CWnd* pParent /*=NULL*/)
	: CDialogEx(FileRenderDlg::IDD, pParent)
{
	m_pic_height = default_pic_height;
	m_pic_width = default_pic_width;
}

FileRenderDlg::~FileRenderDlg()
{
}

void FileRenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_PIC_WIDTH, m_pic_width);
	DDX_Text(pDX, IDC_PIC_HEIGHT, m_pic_height);
}


BEGIN_MESSAGE_MAP(FileRenderDlg, CDialogEx)
	ON_EN_UPDATE(IDC_PIC_WIDTH, &FileRenderDlg::OnEnUpdatePicWidth)
END_MESSAGE_MAP()


// FileRenderDlg message handlers


void FileRenderDlg::OnEnUpdatePicWidth()
{
	//CString text;
	//GetWindowText(text);

	//int x = _ttoi(text);

}
