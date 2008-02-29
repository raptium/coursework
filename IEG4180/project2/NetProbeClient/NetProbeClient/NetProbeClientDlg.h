// NetProbeClientDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CNetProbeClientDlg dialog
class CNetProbeClientDlg : public CDialog
{
// Construction
public:
	CNetProbeClientDlg(CWnd* pParent = NULL);	// standard
	static const long WM_WINSOCK = WM_USER + 1;

// Dialog Data
	enum { IDD = IDD_NETPROBECLIENT_DIALOG };

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
	afx_msg void OnBnClickedExit();
public:
	afx_msg void OnEnChangePs();
public:
	afx_msg void OnBnClickedConnect();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	LPVOID pNetProbe;


};
