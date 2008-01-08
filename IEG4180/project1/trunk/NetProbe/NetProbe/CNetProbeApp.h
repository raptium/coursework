#include <afxwin.h>

class CNetProbeApp : public CWinApp{
public:
	virtual BOOL InitInstance();
};

class CMainFrame : public CFrameWnd{
public:
	CMainFrame();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()
