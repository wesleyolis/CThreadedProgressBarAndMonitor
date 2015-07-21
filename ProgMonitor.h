#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "afxdialogex.h"
#include "ProgBar.h"

class ProgMonitorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ProgMonitorDlg)

public:
	ProgMonitorDlg::ProgMonitorDlg(CString csTitle, CWnd* pParent);
	virtual ~ProgMonitorDlg();

// Dialog Data
	enum { IDD = IDD_PROGBAR };

	enum eDelayTimers {
		eDT_Show = 0,
	};

	bool			m_bInitialized;
	HANDLE			m_hProgMonDlgInit;

	virtual BOOL OnInitDialog();

	bool Create(CWnd* parent);

	void SetProgBar(ProgBar* pProgBar);

	void Start(int msDelay);

	void Stop();

	void SetMarqueMode(bool bEnable);

	void SetStatus(CString csStatus);

	void SetPos(int iPos);

	void SetRange(int iRange);

	void EnsureShowing();

	afx_msg void OnBnClickedCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl	m_CtlProgBar;
	ProgBar*		m_pProgBar;
	CStatic			m_CtlStatus;
	bool			m_bRunning;
	bool			m_MarqueeMode;
	CString			m_csTitle;
	HWND			m_TopWnd;

	CButton m_CtlCancel;
};

class ProgMonitor;

class ProgMonitorDlgThread : public CWinThread {

	DECLARE_DYNCREATE(ProgMonitorDlgThread)

public:

	ProgMonitorDlgThread();

	~ProgMonitorDlgThread() {}

	virtual BOOL InitInstance() {

		m_pMainWnd = m_pParent;
		return true;
	};

	virtual int Run();

	DECLARE_MESSAGE_MAP()

public:
	ProgMonitorDlg*	m_pProgMonitorDlg;
	CString			m_csTitle;
	CWnd*			m_pParent;
	HANDLE			m_hThreadRunning;
	ProgMonitor*	m_pProgMonitor;
};



class ProgMonitor : public ProgBar{

public:

	ProgMonitor(const CString csTitle, bool bMarquee, CWnd* pParent, int msDelay = 300);

	~ProgMonitor();

	virtual void SetMarqueMode(bool bEnable);

	virtual void SetStatus(CString csStatus);

	virtual void SetPos(int iPos, int UpGUIMask);

	virtual void SetRange(int iRange);

	virtual void SetCancelHandler(ProgBarDoCancel* pProgBarDoCancel);
	
	virtual void WaitForThreadAndPumpMessage(HANDLE ThreadFinished);

private:
	int					m_iMsDelay;
	ProgMonitorDlg*				m_ProgMon;
	clock_t					m_InitTime;
	CString					m_csTitle;
	CWnd*					m_pParent;
	CString					m_csStatus;
	ProgMonitorDlgThread*			m_pProgMonitorDlgThread;

	void SetPos(int iPos);

	bool CreateDlgIfNotRunning();

	void EnsureShowing();

	void CreateThreaded();
	
	
};
