#pragma once


// FileRenderDlg dialog

class FileRenderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FileRenderDlg)

public:
	FileRenderDlg(int default_pic_width, int default_pic_height, CWnd* pParent = NULL);   // standard constructor
	virtual ~FileRenderDlg();

	int m_pic_width;
	int m_pic_height;

// Dialog Data
	enum { IDD = IDD_FILERENDER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnUpdatePicWidth();
};
