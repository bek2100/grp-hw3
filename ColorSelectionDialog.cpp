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
, silhouette_thickness(0)
{
	boundbox_color = RGB(0, 0, 0);
	wireframe_color = RGB(0, 0, 0);
	background_color = RGB(0, 0, 0);
	silhouette_color = RGB(0, 255, 255);
}

ColorSelectionDialog::ColorSelectionDialog(COLORREF m_color_wireframe, COLORREF m_boundbox_color, COLORREF m_background_color, COLORREF m_vertex_norm_color, COLORREF m_polygon_norm_color, COLORREF m_silhouette_color, double m_silhouette_thickness, CWnd* pParent)
: CDialogEx(ColorSelectionDialog::IDD, pParent){
	wireframe_color = RGB(GetBValue(m_color_wireframe), GetGValue(m_color_wireframe), GetRValue(m_color_wireframe));
	boundbox_color = RGB(GetBValue(m_boundbox_color), GetGValue(m_boundbox_color), GetRValue(m_boundbox_color));
	background_color = RGB(GetBValue(m_background_color), GetGValue(m_background_color), GetRValue(m_background_color));
	vertex_norm_color = RGB(GetBValue(m_vertex_norm_color), GetGValue(m_vertex_norm_color), GetRValue(m_vertex_norm_color));
	polygon_norm_color = RGB(GetBValue(m_polygon_norm_color), GetGValue(m_polygon_norm_color), GetRValue(m_polygon_norm_color));
	silhouette_color = RGB(GetBValue(m_silhouette_color), GetGValue(m_silhouette_color), GetRValue(m_silhouette_color));
	silhouette_thickness = m_silhouette_thickness;
}



ColorSelectionDialog::~ColorSelectionDialog()
{
}

void ColorSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	double thick = silhouette_thickness;
	DDX_Text(pDX, IDC_SILLHUOTTE_THICKNESS, silhouette_thickness);
	if (silhouette_thickness <= 0){
		CString er("Error, thickness must be larger than 0, using old value.");
		silhouette_thickness = thick;
		this->MessageBox(er);
	}
}


BEGIN_MESSAGE_MAP(ColorSelectionDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &ColorSelectionDialog::OnBackgroundColorClick)
	ON_BN_CLICKED(IDC_BUTTON1, &ColorSelectionDialog::OnWireframeColorClick)
	ON_BN_CLICKED(IDC_BUTTON3, &ColorSelectionDialog::OnBoundboxColorClick)
	ON_BN_CLICKED(IDC_BUTTON5, &ColorSelectionDialog::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &ColorSelectionDialog::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &ColorSelectionDialog::OnBnClickedButton6)
	ON_EN_CHANGE(IDC_LIGHT_POS_X, &ColorSelectionDialog::OnEnChangeLightPosX)
	ON_EN_CHANGE(IDC_SILLHUOTTE_THICKNESS, &ColorSelectionDialog::OnEnChangeSillhuotteThickness)
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


void ColorSelectionDialog::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK) {
		silhouette_color = colorDlg.GetColor();
	}
}


void ColorSelectionDialog::OnEnChangeLightPosX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void ColorSelectionDialog::OnEnChangeSillhuotteThickness()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
