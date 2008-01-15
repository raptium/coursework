// HostInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetProbe.h"
#include "HostInfoDlg.h"
#include <winsock2.h>
#include <string.h>


// CHostInfoDlg dialog

IMPLEMENT_DYNAMIC(CHostInfoDlg, CDialog)

CHostInfoDlg::CHostInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHostInfoDlg::IDD, pParent)
{

}

CHostInfoDlg::~CHostInfoDlg()
{
}

void CHostInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHostInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CHostInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_LOOKUP, &CHostInfoDlg::OnBnClickedLookup)
END_MESSAGE_MAP()


// CHostInfoDlg message handlers

void CHostInfoDlg::OnBnClickedOk()
{
	OnOK();
}
void CHostInfoDlg::OnBnClickedLookup()
{
	char *ip;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	struct sockaddr_in addr;
	int retVal;


	// Initialize Winsock



	memset(&aiHints,0,sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	aiHints.ai_flags = AI_CANONNAME;

	ip=(char *)malloc(sizeof(char)*128);
	memset(ip,0,sizeof(ip));
	GetDlgItemTextA(IDC_HOST,ip,127);


	if((retVal=getaddrinfo(ip,NULL,&aiHints,&aiList))!=0){
		printf("getaddrinfo() failed.\n");
	}
	free(ip);


	CListBox *iplist = (CListBox *)this->GetDlgItem(IDC_IPLIST);
	iplist->ResetContent();

	memcpy(&addr,(aiList->ai_addr),14);
	this->SetDlgItemTextA(IDC_IP,inet_ntoa(addr.sin_addr));
	this->SetDlgItemTextA(IDC_OFFICIAL,aiList->ai_canonname);

	while(1){
		memcpy(&addr,(aiList->ai_addr),14);
		iplist->AddString(inet_ntoa(addr.sin_addr));

		if(aiList->ai_next!=NULL)
			aiList=aiList->ai_next;
		else
			break;
	}

	freeaddrinfo(aiList);
	

}
