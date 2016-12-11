#pragma once


// PrespectiveControlDialog dialog

class PrespectiveControlDialog : public CDialogEx
{
	DECLARE_DYNAMIC(PrespectiveControlDialog)

public:
	PrespectiveControlDialog(CWnd* pParent = NULL);   // standard constructor
	PrespectiveControlDialog(double default_d, CWnd* pParent = NULL);   // standard constructor
	virtual ~PrespectiveControlDialog();

// Dialog Data
	enum { IDD = IDD_PRESPECTIVE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double d;
};
