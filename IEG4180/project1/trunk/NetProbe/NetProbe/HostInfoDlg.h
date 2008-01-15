#pragma once


// HostInfoDlg dialog

class HostInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(HostInfoDlg)

public:
	HostInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~HostInfoDlg();

// Dialog Data
	enum { IDD = IDD_HOSTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();
};
