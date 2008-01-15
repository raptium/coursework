// HostInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetProbe.h"
#include "HostInfoDlg.h"


// HostInfoDlg dialog

IMPLEMENT_DYNAMIC(HostInfoDlg, CDialog)

HostInfoDlg::HostInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HostInfoDlg::IDD, pParent)
{

}

HostInfoDlg::~HostInfoDlg()
{
}

void HostInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HostInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_EXIT, &HostInfoDlg::OnBnClickedExit)
END_MESSAGE_MAP()


// HostInfoDlg message handlers

void HostInfoDlg::OnBnClickedExit()
{
	this->EndDialog(TRUE);
}
