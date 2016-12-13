// ColorSelectionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "ColorSelectionDialog.h"
#include "afxdialogex.h"


// ColorSelectionDialog dialog

IMPLEMENT_DYNAMIC(ColorSelectionDialog, CDialogEx)

ColorSelectionDialog::ColorSelectionDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(ColorSelectionDialog::IDD, pParent)
{
	boundbox_color = RGB(0, 0, 0);
	wireframe_color = RGB(0, 0, 0);
	background_color = RGB(0, 0, 0);
}

ColorSelectionDialog::ColorSelectionDialog(COLORREF m_color_wireframe, COLORREF m_boundbox_color, COLORREF m_background_color, COLORREF m_vertex_norm_color, COLORREF m_polygon_norm_color, CWnd* pParent)
: CDialogEx(ColorSelectionDialog::IDD, pParent){
	wireframe_color = RGB(GetBValue(m_color_wireframe), GetGValue(m_color_wireframe), GetRValue(m_color_wireframe));
	boundbox_color = RGB(GetBValue(m_boundbox_color), GetGValue(m_boundbox_color), GetRValue(m_boundbox_color));
	background_color = RGB(GetBValue(m_background_color), GetGValue(m_background_color), GetRValue(m_background_color));
	vertex_norm_color = RGB(GetBValue(m_vertex_norm_color), GetGValue(m_vertex_norm_color), GetRValue(m_vertex_norm_color));
	polygon_norm_color = RGB(GetBValue(m_polygon_norm_color), GetGValue(m_polygon_norm_color), GetRValue(m_polygon_norm_color));
}



ColorSelectionDialog::~ColorSelectionDialog()
{
}

void ColorSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ColorSelectionDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &ColorSelectionDialog::OnBackgroundColorClick)
	ON_BN_CLICKED(IDC_BUTTON1, &ColorSelectionDialog::OnWireframeColorClick)
	ON_BN_CLICKED(IDC_BUTTON3, &ColorSelectionDialog::OnBoundboxColorClick)
	ON_BN_CLICKED(IDC_BUTTON5, &ColorSelectionDialog::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &ColorSelectionDialog::OnBnClickedButton4)
END_MESSAGE_MAP()


// ColorSelectionDialog message handlers


void ColorSelectionDialog::OnBackgroundColorClick()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		background_color = colorDlg.GetColor();
	}
}


void ColorSelectionDialog::OnWireframeColorClick()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		wireframe_color = colorDlg.GetColor();
	}
}


void ColorSelectionDialog::OnBoundboxColorClick()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		boundbox_color = colorDlg.GetColor();
	}
}


void ColorSelectionDialog::OnBnClickedButton5()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		vertex_norm_color = colorDlg.GetColor();
	}
}


void ColorSelectionDialog::OnBnClickedButton4()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		polygon_norm_color = colorDlg.GetColor();
	}
}
