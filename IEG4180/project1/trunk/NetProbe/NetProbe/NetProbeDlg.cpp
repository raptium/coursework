// NetProbeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetProbe.h"
#include "NetProbeDlg.h"
#include "HostInfoDlg.h"
#include "es_TIMER.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CNetProbeDlg dialog

UINT __cdecl UIRefresh(LPVOID pParam);


CNetProbeDlg::CNetProbeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetProbeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetProbeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetProbeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HOSTINFO, &CNetProbeDlg::OnBnClickedHostinfo)
	ON_BN_CLICKED(IDEXIT, &CNetProbeDlg::OnBnClickedExit)
	ON_EN_CHANGE(IDC_LOCAL, &CNetProbeDlg::OnEnChangeLocal)
	ON_EN_CHANGE(IDC_REMOTE, &CNetProbeDlg::OnEnChangeRemote)
	ON_BN_CLICKED(IDC_TCP, &CNetProbeDlg::OnBnClickedTcp)
	ON_BN_CLICKED(IDC_UDP, &CNetProbeDlg::OnBnClickedUdp)
	ON_BN_CLICKED(IDC_RECV, &CNetProbeDlg::OnBnClickedRecv)
	ON_BN_CLICKED(IDC_SEND, &CNetProbeDlg::OnBnClickedSend)
	ON_EN_CHANGE(IDC_LPORT, &CNetProbeDlg::OnEnChangeLport)
	ON_EN_CHANGE(IDC_PS, &CNetProbeDlg::OnEnChangePs)
	ON_EN_CHANGE(IDC_RI, &CNetProbeDlg::OnEnChangeRi)
	ON_EN_CHANGE(IDC_RPORT, &CNetProbeDlg::OnEnChangeRport)
	ON_EN_CHANGE(IDC_SR, &CNetProbeDlg::OnEnChangeSr)
END_MESSAGE_MAP()


// CNetProbeDlg message handlers

BOOL CNetProbeDlg::OnInitDialog()
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

void CNetProbeDlg::OnPaint()
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
HCURSOR CNetProbeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNetProbeDlg::OnBnClickedHostinfo()
{

	CHostInfoDlg dlg;
	dlg.DoModal();

	//dlg->Create(IDD_HOSTINFO, this);
	//dlg->ShowWindow(SW_SHOW);
}

void CNetProbeDlg::OnBnClickedExit()
{
	exit(0);
}

void CNetProbeDlg::OnEnChangeLocal()
{
	char *tmp;
	tmp = (char *)malloc(128);

	this->GetDlgItemTextA(IDC_LOCAL, tmp, 127);
	theProbe.setLocal(tmp);
	free(tmp);
}

void CNetProbeDlg::OnEnChangeRemote()
{
	char *tmp;
	tmp = (char *)malloc(128);

	this->GetDlgItemTextA(IDC_REMOTE, tmp, 127);
	theProbe.setRemote(tmp);
	free(tmp);
}

void CNetProbeDlg::OnBnClickedTcp()
{
	theProbe.setProtocol(1);
}

void CNetProbeDlg::OnBnClickedUdp()
{
	theProbe.setProtocol(2);
}


void CNetProbeDlg::OnBnClickedRecv()
{
	if(theProbe.getStatus()==0){
		if(theProbe.startReceive() == true){
			this->SetDlgItemTextA(IDC_RECV, "Stop");
			GetDlgItem(IDC_SEND)->EnableWindow(false);
			AfxBeginThread(UIRefresh, this);
		}
	}else{
		theProbe.stop();
		GetDlgItem(IDC_SEND)->EnableWindow(true);
		this->SetDlgItemTextA(IDC_RECV, "Receive");
	}
	
}

void CNetProbeDlg::OnBnClickedSend()
{
	if(theProbe.getStatus()==0){
		if(theProbe.startSend() == true){
			this->SetDlgItemTextA(IDC_SEND, "Stop");
			GetDlgItem(IDC_RECV)->EnableWindow(false);
			AfxBeginThread(UIRefresh, this);
		}
	}else{
		theProbe.stop();
		GetDlgItem(IDC_RECV)->EnableWindow(true);
		this->SetDlgItemTextA(IDC_SEND, "Send");
	}
}

void CNetProbeDlg::OnEnChangeLport()
{
	theProbe.setLocalPort(this->GetDlgItemInt(IDC_LPORT));
}

void CNetProbeDlg::OnEnChangePs()
{
	theProbe.setPacketSize(this->GetDlgItemInt(IDC_PS));
}

UINT __cdecl UIRefresh(LPVOID pParam){
	CNetProbeDlg *dlg = (CNetProbeDlg *)pParam;

	int count = 0;
	char t[10];
	double bps;
	int flag = 0;
	long sp;

	while(1){

		while(1){
			Sleep(10);
			if(theProbe.timer.Elapsed() > count * theProbe.getInterval()){
				count = theProbe.timer.Elapsed() / theProbe.getInterval() + 1;
				break;
			}
		}

		if(theProbe.getStatus() == 0){
			break;
		}

		sprintf(t, "%.2f sec",theProbe.timer.Elapsed()/1000.0);
		dlg->SetDlgItemTextA(IDC_TE, t);

		sprintf(t, "%d", theProbe.getPacketTransfer());
		dlg->SetDlgItemTextA(IDC_NPT, t);

		sprintf(t, "%d", theProbe.getPacketLost());
		dlg->SetDlgItemTextA(IDC_NPL, t);

		sprintf(t, "%.2f%", theProbe.getPacketLoss()*100);
		dlg->SetDlgItemTextA(IDC_PL, t);

		if(!flag && theProbe.getByteTransfer() != 0){
			sp = theProbe.timer.Elapsed();
			flag = 1;
		}

		bps = theProbe.getByteTransfer()  / ((theProbe.timer.Elapsed() - sp) / 1000.0);
		sprintf(t, "%.2f Bps", bps);
		dlg->SetDlgItemTextA(IDC_DTR, t);

		if(theProbe.getStatus() > 2 && theProbe.getPacketTransfer() >= dlg->GetDlgItemInt(IDC_NPS)){
			theProbe.stop();
		}
	}

	dlg->GetDlgItem(IDC_SEND)->EnableWindow(true);
	dlg->SetDlgItemTextA(IDC_SEND, "Send");
	dlg->GetDlgItem(IDC_RECV)->EnableWindow(true);
	dlg->SetDlgItemTextA(IDC_RECV, "Receive");

	AfxEndThread(0);

	return 0;
}

void CNetProbeDlg::OnEnChangeRi()
{
	theProbe.setRefreshInterval(this->GetDlgItemInt(IDC_RI));
}

void CNetProbeDlg::OnEnChangeRport()
{
	theProbe.setRemotePort(this->GetDlgItemInt(IDC_RPORT));
}

void CNetProbeDlg::OnEnChangeSr()
{
	theProbe.setSendingRate(this->GetDlgItemInt(IDC_SR));
}
