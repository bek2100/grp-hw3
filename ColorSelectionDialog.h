#pragma once


// ColorSelectionDialog dialog

class ColorSelectionDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ColorSelectionDialog)

public:
	ColorSelectionDialog(CWnd* pParent = NULL);   // standard constructor
	ColorSelectionDialog( 
		COLORREF m_color_wireframe, 
		COLORREF m_boundbox_color, 
		COLORREF m_background_color,
		COLORREF m_vertex_norm_color,
		COLORREF m_polygon_norm_color,
		CWnd* pParent = NULL);
	virtual ~ColorSelectionDialog();

// Dialog Data
	enum { IDD = IDD_COLOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	COLORREF wireframe_color;
	COLORREF boundbox_color;
	COLORREF background_color;
	COLORREF vertex_norm_color;
	COLORREF polygon_norm_color;
	afx_msg void OnBackgroundColorClick();
	afx_msg void OnWireframeColorClick();
	afx_msg void OnBoundboxColorClick();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
};

