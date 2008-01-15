#pragma once


// CHostInfoDlg dialog

class CHostInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CHostInfoDlg)

public:
	CHostInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHostInfoDlg();
// Dialog Data
	enum { IDD = IDD_HOSTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedLookup();
};
