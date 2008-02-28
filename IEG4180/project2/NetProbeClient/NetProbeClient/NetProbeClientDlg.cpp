// NetProbeClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetProbeClient.h"
#include "NetProbeClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetProbeClientDlg dialog




CNetProbeClientDlg::CNetProbeClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetProbeClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetProbeClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOSTNAME, textbox_hostname);
	DDX_Control(pDX, IDC_PORT, textbox_port);
	DDX_Control(pDX, IDC_RI, textbox_refreshInterval);
	DDX_Control(pDX, IDC_PS, textbox_packetSize);
	DDX_Control(pDX, IDC_SR, textbox_sendingRate);
	DDX_Control(pDX, IDC_NPS, textbox_packets2Send);
	DDX_Control(pDX, IDC_DTR, label_transferRate);
	DDX_Control(pDX, IDC_PL, label_packetLoss);
	DDX_Control(pDX, IDC_NPT, label_packetsTransferred);
	DDX_Control(pDX, IDC_NPL, label_packetsLost);
	DDX_Control(pDX, IDC_TE, label_timeElapsed);
}

BEGIN_MESSAGE_MAP(CNetProbeClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDEXIT, &CNetProbeClientDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDCONNECT, &CNetProbeClientDlg::OnBnClickedConnect)
END_MESSAGE_MAP()


// CNetProbeClientDlg message handlers

BOOL CNetProbeClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetProbeClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetProbeClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNetProbeClientDlg::OnBnClickedExit()
{
	exit(0);
}
void CNetProbeClientDlg::OnBnClickedConnect()
{
	char hostname[256];
	GetDlgItemTextA(IDC_HOSTNAME, hostname, 256);
	NetProbe *theProbe = new NetProbe(this, hostname, GetDlgItemInt(IDC_PORT));

	theProbe->wThread = AfxBeginThread((AFX_THREADPROC)theProbe->threadTCP, theProbe);
	AfxBeginThread((AFX_THREADPROC)theProbe->threadUpdateUI, theProbe);
}
