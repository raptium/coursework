#include "CNetProbeApp.h"

BOOL CNetProbeApp::InitInstance(){
	m_pMainWnd = new CMainFrame;
	m_pMainWnd->ShowWindow(SW_NORMAL);

	return TRUE;
}

CMainFrame::CMainFrame(){
	Create(NULL, L"MFC Fundamentals", WS_OVERLAPPEDWINDOW, CRect(120, 100, 700, 480), NULL);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct){

	if(CFrameWnd::OnCreate(lpCreateStruct) == 0){
		MessageBox(L"The windows has been created!!!");
		return 0;
	}
	return -1;
}

CNetProbeApp theApp;