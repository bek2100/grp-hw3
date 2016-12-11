// PrespectiveControlDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "PrespectiveControlDialog.h"
#include "afxdialogex.h"


// PrespectiveControlDialog dialog

IMPLEMENT_DYNAMIC(PrespectiveControlDialog, CDialogEx)

PrespectiveControlDialog::PrespectiveControlDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(PrespectiveControlDialog::IDD, pParent)
	, d(1)
{

}

PrespectiveControlDialog::PrespectiveControlDialog(double default_d, CWnd* pParent /*=NULL*/)
	: CDialogEx(PrespectiveControlDialog::IDD, pParent)
	, d(1)
{
	d = default_d;
}

PrespectiveControlDialog::~PrespectiveControlDialog()
{
}

void PrespectiveControlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_PRESPECTIVE_D, d);
	DDV_MinMaxDouble(pDX, d, 0, d*d + 1);
}


BEGIN_MESSAGE_MAP(PrespectiveControlDialog, CDialogEx)
END_MESSAGE_MAP()


// PrespectiveControlDialog message handlers
