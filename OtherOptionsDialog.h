#pragma once
#include <vector>
#include <atlstr.h>

// OtherOptionsDialog dialog

class OtherOptionsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(OtherOptionsDialog)

public:
	OtherOptionsDialog(CWnd* pParent = NULL);   // standard constructor
	OtherOptionsDialog(double def_finess, CString _models_list, std::vector<bool> active_modules, CWnd* pParent = NULL);
	virtual ~OtherOptionsDialog();
	CString models_list;
// Dialog Data
	enum { IDD = IDD_OTHERS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double finess;
	std::vector<bool> active_modules;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEditFiness();
	afx_msg void OnBnClickedButton1();
};
