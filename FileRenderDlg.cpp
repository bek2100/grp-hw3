// FileRenderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "FileRenderDlg.h"
#include "afxdialogex.h"

// FileRenderDlg dialog

IMPLEMENT_DYNAMIC(FileRenderDlg, CDialogEx)

FileRenderDlg::FileRenderDlg(int default_pic_width, int default_pic_height, CString default_pic_name, CWnd* pParent /*=NULL*/)
	: CDialogEx(FileRenderDlg::IDD, pParent)
{
	m_pic_height = default_pic_height;
	m_pic_width = default_pic_width;
	m_pic_name =  default_pic_name;
}

FileRenderDlg::~FileRenderDlg()
{
}

void FileRenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_PIC_WIDTH, m_pic_width);
	DDX_Text(pDX, IDC_PIC_HEIGHT, m_pic_height);
	DDX_Text(pDX, IDC_FILE_NAME, m_pic_name);

}


BEGIN_MESSAGE_MAP(FileRenderDlg, CDialogEx)
	ON_EN_UPDATE(IDC_PIC_WIDTH, &FileRenderDlg::OnEnUpdatePicWidth)
	ON_EN_CHANGE(IDC_FILE_NAME, &FileRenderDlg::OnEnChangeFileName)
END_MESSAGE_MAP()


// FileRenderDlg message handlers


void FileRenderDlg::OnEnUpdatePicWidth()
{
	//CString text;
	//GetWindowText(text);

	//int x = _ttoi(text);

}


void FileRenderDlg::OnEnChangeFileName()
{
	//CString raw_pic_name;
	//GetWindowText(raw_pic_name);

	//// Convert a TCHAR string to a LPCSTR
	//CT2CA pszConvertedAnsiString(raw_pic_name);

	//m_pic_name = std::string(pszConvertedAnsiString);
}
