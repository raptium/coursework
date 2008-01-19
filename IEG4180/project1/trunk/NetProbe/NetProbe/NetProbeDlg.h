// NetProbeDlg.h : header file
//

#pragma once


// CNetProbeDlg dialog
class CNetProbeDlg : public CDialog
{
// Construction
public:
	CNetProbeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NETPROBE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedHostinfo();
public:
	afx_msg void OnBnClickedExit();
public:
	afx_msg void OnEnChangeLocal();
public:
	afx_msg void OnEnChangeRemote();
public:
	afx_msg void OnBnClickedTcp();
public:
	afx_msg void OnBnClickedUdp();
public:
	afx_msg void OnBnClickedRecv();
public:
	afx_msg void OnBnClickedSend();
public:
	afx_msg void OnEnChangeLport();
};
