#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "afxdialogex.h"
#include "ProgBar.h"

#define WM_PROG_MONITOR_PROGBAR			WM_APP + 200
#define WM_PROG_MONITOR_START			WM_APP + 201
#define WM_PROG_MONITOR_STOP			WM_APP + 202
#define WM_PROG_MONITOR_SETMARGUE_MODE  WM_APP + 203
#define WM_PROG_MONITOR_SETSTATUS		WM_APP + 204
#define WM_PROG_MONITOR_SETPOS			WM_APP + 205
#define WM_PROG_MONITOR_SETRANGE		WM_APP + 206

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

	bool ProgMonitorDlg::Create(CWnd* parent);

	void ProgMonitorDlg::Destroy();

	void SetProgBar(ProgBar* pProgBar) {

		this->SendMessage(WM_PROG_MONITOR_PROGBAR, 0, (LPARAM)pProgBar);
	}

	void Start(int msDelay) {

		this->SendMessage(WM_PROG_MONITOR_START, 0, (LPARAM) msDelay);
	}

	void Stop() {

		this->SendMessage(WM_PROG_MONITOR_STOP, 0, 0);
	}

	void SetMarqueMode(bool bEnable) {

		this->SendMessage(WM_PROG_MONITOR_SETMARGUE_MODE, 0, (LPARAM) bEnable);
	}

	void SetStatus(CString csStatus) {

		m_csStatus = csStatus;

		this->SendMessage(WM_PROG_MONITOR_SETSTATUS, 0, 0);
	}

	void SetPos(int iPos) {

		this->SendMessage(WM_PROG_MONITOR_SETPOS, 0, iPos);
	}

	void SetRange(int iRange) {

		this->SendMessage(WM_PROG_MONITOR_SETRANGE, 0, iRange);
	}

	void EnsureShowing();

	afx_msg void OnBnClickedCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

	LRESULT OnSetProgBar(WPARAM wParam, LPARAM lParam);
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	LRESULT OnStop(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetMarqueMode(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetStatus(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetRange(WPARAM wParam, LPARAM lParam);

private:
	CProgressCtrl	m_CtlProgBar;
	ProgBar*		m_pProgBar;
	CStatic			m_CtlStatus;
	CStatic			m_CtlProgStatus;
	bool			m_bRunning;
	bool			m_MarqueeMode;
	CString			m_csTitle;
	HWND			m_TopWnd;
	CString			m_csStatus;
	int				m_iRange;

	CButton m_CtlCancel;

	void SetProgBarIntenal(ProgBar* pProgBar);

	void StartIntenal(int msDelay);

	void StopIntenal();

	void SetMarqueModeIntenal(bool bEnable);

	void SetStatusIntenal(CString csStatus);

	void SetPosIntenal(int iPos);

	void SetRangeIntenal(int iRange);
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
	int						m_iMsDelay;
	ProgMonitorDlg*			m_ProgMon;
	clock_t					m_InitTime;
	CString					m_csTitle;
	CWnd*					m_pParent;
	CString					m_csStatus;
	ProgMonitorDlgThread*	m_pProgMonitorDlgThread;

	void SetPos(int iPos);

	bool CreateDlgIfNotRunning();

	void EnsureShowing();

	void CreateThreaded();
	
	
};
