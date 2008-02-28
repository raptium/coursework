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


	
	this->SetDlgItemTextA(IDC_RI, "100");
	this->SetDlgItemTextA(IDC_PS, "1024");
	this->SetDlgItemTextA(IDC_SR, "102400");
	this->SetDlgItemTextA(IDC_NPS, "0");
	this->SetDlgItemTextA(IDC_HOSTNAME, "localhost");
	this->SetDlgItemTextA(IDC_PORT, "12345");
	this->CheckRadioButton(IDC_TCP, IDC_UDP, IDC_TCP);
	this->CheckRadioButton(IDC_BLOCKING, IDC_MSG, IDC_BLOCKING);


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
	static NetProbe *theProbe = NULL;

	if(theProbe == NULL){
		GetDlgItemTextA(IDC_HOSTNAME, hostname, 256);
		theProbe = new NetProbe(this, hostname, GetDlgItemInt(IDC_PORT));
		if(GetCheckedRadioButton(IDC_TCP, IDC_UDP) == IDC_TCP){
			if(GetCheckedRadioButton(IDC_BLOCKING, IDC_MSG) == IDC_BLOCKING){
				theProbe->TCPConnect(theProbe);
				AfxBeginThread((AFX_THREADPROC)theProbe->threadTCPReceive, theProbe);
			}
			else{

			}
		}
		else{
			if(GetCheckedRadioButton(IDC_BLOCKING, IDC_MSG) == IDC_BLOCKING)
				theProbe->wThread = AfxBeginThread((AFX_THREADPROC)theProbe->threadUDP, theProbe);
			else{
			
			}
		}
		AfxBeginThread((AFX_THREADPROC)theProbe->threadUpdateUI, theProbe);
		SetDlgItemTextA(IDCONNECT, "Stop");
	}else{
		theProbe->stop();
		Sleep(20);
		delete theProbe;
		theProbe = NULL;
		SetDlgItemTextA(IDCONNECT, "Connect");
		
	}
}

LRESULT CNetProbeClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_WINSOCK){


	}

	return CDialog::WindowProc(message, wParam, lParam);
}
