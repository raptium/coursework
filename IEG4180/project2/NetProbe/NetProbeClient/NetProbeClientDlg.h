// NetProbeClientDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CNetProbeClientDlg dialog
class CNetProbeClientDlg : public CDialog
{
// Construction
public:
	CNetProbeClientDlg(CWnd* pParent = NULL);	// standard constructor

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
	CEdit textbox_hostname;
public:
	CEdit textbox_port;
public:
	CEdit textbox_refreshInterval;
public:
	afx_msg void OnEnChangePs();
public:
	CEdit textbox_packetSize;
public:
	CEdit textbox_sendingRate;
public:
	CEdit textbox_packets2Send;
public:
	CStatic label_transferRate;
public:
	CStatic label_packetLoss;
public:
	CStatic label_packetsTransferred;
public:
	CStatic label_packetsLost;
public:
	CStatic label_timeElapsed;
public:
	afx_msg void OnBnClickedConnect();
};
