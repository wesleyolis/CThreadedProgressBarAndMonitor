// ProgMonitorDlgDlg.cpp : implementation file
#include "ProgMonitor.h"


// ProgMonitorDlg dialog

IMPLEMENT_DYNAMIC(ProgMonitorDlg, CDialogEx)

ProgMonitorDlg::ProgMonitorDlg(CString csTitle, CWnd* pParent /*=NULL*/)
	: CDialogEx(ProgMonitorDlg::IDD, pParent)
{
	m_pProgBar = nullptr;
	m_bInitialized = false;
	m_MarqueeMode = false;
	m_bRunning = false;

	m_csTitle = csTitle;
}

ProgMonitorDlg::~ProgMonitorDlg()
{
	if (::IsWindow(m_hWnd)) {

		DestroyWindow();
	}
}

void ProgMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_CtlProgBar);
	DDX_Control(pDX, IDC_STATUS, m_CtlStatus);
	DDX_Control(pDX, IDCANCEL, m_CtlCancel);
}


BEGIN_MESSAGE_MAP(ProgMonitorDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &ProgMonitorDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL ProgMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_CtlProgBar.ModifyStyle(NULL, GetStyle() | PBS_MARQUEE);
	::SendMessage(m_CtlProgBar, PBM_SETMARQUEE, TRUE, 100);

	m_bInitialized = true;

	return TRUE;
}

bool ProgMonitorDlg::Create(CWnd* parent)
{
	if (CDialogEx::Create(IDD, parent))
	{
		//ModifyStyle(WS_SYSMENU, 0);
		return true;
	}

	return false;
}

void ProgMonitorDlg::SetProgBar(ProgBar* pProgBar){

	m_pProgBar = pProgBar;

	m_CtlCancel.EnableWindow(m_pProgBar != nullptr);
}

void ProgMonitorDlg::Start(int msDelay) {
	
	if (m_bRunning)
		return;

	if (msDelay != 0){

		SetTimer(eDT_Show, msDelay, nullptr);
	}
	else
	{
		EnsureShowing();
		::SendMessage(m_CtlProgBar, PBM_SETMARQUEE, TRUE, 100);
		m_bRunning = true;
	}
}

void ProgMonitorDlg::Stop() {

	::SendMessage(m_CtlProgBar, PBM_SETMARQUEE, FALSE, 100);
	m_bRunning = false;
}

void ProgMonitorDlg::SetMarqueMode(bool bEnable){
	
	if (bEnable)
	{
		if (m_bRunning)
			return;

		m_CtlProgBar.ModifyStyle(NULL, GetStyle() | PBS_MARQUEE);
		m_MarqueeMode = true;
	}
	else{
	
		if (m_bRunning){

			Stop();
		}
		m_CtlProgBar.ModifyStyle(GetStyle() & PBS_MARQUEE, NULL);
		m_MarqueeMode = false;
	}

	RedrawWindow();
}

void ProgMonitorDlg::SetStatus(CString csStatus) {

	m_CtlStatus.SetWindowTextA(csStatus);
	//m_CtlStatus.RedrawWindow();
}

void ProgMonitorDlg::SetPos(int iPos) {

	m_CtlProgBar.SetPos(iPos);
}

void ProgMonitorDlg::SetRange(int iRange) {

	m_CtlProgBar.SetRange32(0,iRange);
}

void ProgMonitorDlg::EnsureShowing(){

	ShowWindow(SW_SHOW);
	CenterWindow();
	RedrawWindow();
}

afx_msg void ProgMonitorDlg::OnTimer(UINT_PTR nIDEvent) {

	if (nIDEvent == eDT_Show ) {

		KillTimer(eDT_Show);
		EnsureShowing();
		::SendMessage(m_CtlProgBar, PBM_SETMARQUEE, TRUE, 100);
		m_bRunning = true;
	}
}

// ProgMonitorDlg message handlers

void ProgMonitorDlg::OnBnClickedCancel() {

	if (m_pProgBar)
	{
		m_pProgBar->DoCancel();
	}
	else {
		CDialogEx::OnCancel();
	}
	
}



IMPLEMENT_DYNCREATE(ProgMonitorDlgThread, CWinThread)

BEGIN_MESSAGE_MAP(ProgMonitorDlgThread, CWinThread)

END_MESSAGE_MAP()

ProgMonitorDlgThread::ProgMonitorDlgThread() {

	m_pProgMonitorDlg = nullptr;
	m_pProgMonitor = nullptr;
	m_pParent = nullptr;
}

int ProgMonitorDlgThread::Run() {

	m_bAutoDelete = false;
	ProgMonitorDlg ProgMon(m_csTitle, m_pParent);

	m_pProgMonitorDlg = &ProgMon;

	SetEvent(m_hThreadRunning);

	bool bCancelled = (ProgMon.DoModal() == IDCANCEL);
	m_pProgMonitorDlg = nullptr;

	if (bCancelled && m_pProgMonitor != nullptr) {

		m_pProgMonitor->DoCancel();
	}

	return 0;
}




ProgMonitor::ProgMonitor(const CString csTitle, bool bMarquee, CWnd* pParent, int msDelay ){

	m_pProgMonitorDlgThread = nullptr;
	m_iMsDelay = msDelay;
	m_ProgMon = nullptr;
	m_csTitle = csTitle;
	m_pParent = pParent;

	SetMarqueMode(bMarquee);
}

ProgMonitor::~ProgMonitor(){

	if (m_pProgMonitorDlgThread == nullptr) {
	
		if (m_ProgMon) {

			VERIFY(m_ProgMon->DestroyWindow());
			delete m_ProgMon;
		}
	}
	else {
		m_pProgMonitorDlgThread->m_pProgMonitor = nullptr;
		m_ProgMon->SetProgBar(nullptr);

		m_ProgMon->OnBnClickedCancel();

		DWORD dExitCode = STILL_ACTIVE;
		while (dExitCode == STILL_ACTIVE) {

			m_pProgMonitorDlgThread->PumpMessage();
			GetExitCodeThread(m_pProgMonitorDlgThread->m_hThread, &dExitCode);
		}

		delete m_pProgMonitorDlgThread;
	}
}

void ProgMonitor::SetMarqueMode(bool bEnable){

	// Reset any cancelling.
	m_bCancelled = false;

	if (m_ProgMon)
	{
		m_ProgMon->SetMarqueMode(bEnable);
		
		if (bEnable){

			m_ProgMon->SetProgBar(nullptr);
			m_ProgMon->Start(0);
		}
		else
		{
			m_ProgMon->SetProgBar(this);
		}
		
		
		return;
	}

	if (bEnable)
	{
		// Imediately create the dialog and start it, their be no futher calls to the Progbar.
		// Later I could look into this runing on its own thread, potentially try pumping the message query in the run method,
		// So that the dialog message has a chance to process interaction. However I am skeptically about this approach, not working again.		
		CreateThreaded();
		
		//m_ProgMon = new ProgMonitorDlg(m_csTitle, m_pParent);

		//VERIFY(m_ProgMon->Create(m_pParent));

		m_ProgMon->SetMarqueMode(true);

		m_ProgMon->Start(m_iMsDelay);

		m_ProgMon->SetStatus(m_csStatus);
		
		EnsureShowing();
	}
	else
	{
		m_InitTime = clock();
	}

}

void ProgMonitor::SetStatus(CString csStatus) {

	if (CreateDlgIfNotRunning()){

		m_ProgMon->SetStatus(csStatus);
	}
	else {
		m_csStatus = csStatus;
	}
}

void ProgMonitor::SetPos(int iPos, int UpGUIMask) {

	if ((iPos & UpGUIMask) == 1) {
	
		SetPos(iPos);
	}
}

void ProgMonitor::SetPos(int iPos) {

	if (CreateDlgIfNotRunning()) {

		m_ProgMon->SetPos(iPos);

		MSG Msg;
		while (PeekMessage(&Msg, m_ProgMon->m_hWnd, 0, 0, PM_REMOVE)) {

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}

void ProgMonitor::SetRange(int iRange) {

	if (CreateDlgIfNotRunning()){

		m_ProgMon->SetRange(iRange);
	}

}

void ProgMonitor::SetCancelHandler(ProgBarDoCancel* pProgBarDoCancel) {

	ProgBar::SetCancelHandler(pProgBarDoCancel);

	if (m_ProgMon) {
	
		if (pProgBarDoCancel == nullptr) {

			m_ProgMon->SetProgBar(nullptr);
		} else {

			m_ProgMon->SetProgBar(this);
		}
	}
}

void ProgMonitor::WaitForThreadAndPumpMessage(HANDLE ThreadFinished){

	if (m_ProgMon == nullptr)
	{
		return;
	}

	while (WAIT_OBJECT_0 != WaitForSingleObject(ThreadFinished, 100))
	{
		if (m_pProgMonitorDlgThread) {

			m_pProgMonitorDlgThread->PumpMessage();
		}
		
		MSG Msg;
		PeekMessage(&Msg, m_ProgMon->m_hWnd, 0, 0, PM_REMOVE);
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	CloseHandle(ThreadFinished);
}


bool ProgMonitor::CreateDlgIfNotRunning()
{
	if (m_ProgMon != nullptr)
	{
		return true;
	}
	else
	{
		clock_t CurrTime = clock();

		double dDiffMs = (CurrTime - m_InitTime);

		if (dDiffMs >= m_iMsDelay)
		{
			CreateThreaded();
			//m_ProgMon = new ProgMonitorDlg(m_csTitle, m_pParent);

			//m_ProgMon->SetProgBar(this);

			//VERIFY(m_ProgMon->Create(m_pParent));

			EnsureShowing();

			m_ProgMon->SetStatus(m_csStatus);

			return true;
		}
		else
		{
			return false;
		}
	}
}

void ProgMonitor::EnsureShowing() {

	if (!IsWindowVisible(m_ProgMon->m_hWnd)) {
		// Ensure that the dialog starts imediately.
		MSG Msg;
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);

			m_ProgMon->EnsureShowing();

			if (IsWindowVisible(m_ProgMon->m_hWnd))
				return;
		}
	}

}

void ProgMonitor::CreateThreaded() {

	m_pProgMonitorDlgThread = new ProgMonitorDlgThread();

	m_pProgMonitorDlgThread->m_hThreadRunning = CreateEvent(0, FALSE, FALSE, 0);
	m_pProgMonitorDlgThread->m_csTitle = m_csTitle;
	m_pProgMonitorDlgThread->m_pParent = m_pParent;
	m_pProgMonitorDlgThread->m_pProgMonitor = this;
	m_pProgMonitorDlgThread->CreateThread();
	m_pProgMonitorDlgThread->ResumeThread();

	while (WaitForSingleObject(m_pProgMonitorDlgThread->m_hThreadRunning, INFINITE) != WAIT_OBJECT_0);

	while (m_pProgMonitorDlgThread->m_pProgMonitorDlg->m_hWnd == nullptr) {

		m_pProgMonitorDlgThread->PumpMessage();
	}

	CloseHandle(m_pProgMonitorDlgThread->m_hThreadRunning);

	MSG Msg;

	while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		m_pProgMonitorDlgThread->PumpMessage();

		if (m_pProgMonitorDlgThread->m_pProgMonitorDlg->m_hWnd != nullptr) {

			if (IsWindowVisible(m_pProgMonitorDlgThread->m_pProgMonitorDlg->m_hWnd)) {

				break;
			}

			m_pProgMonitorDlgThread->m_pProgMonitorDlg->EnsureShowing();
		}
	}

	m_ProgMon = m_pProgMonitorDlgThread->m_pProgMonitorDlg;
}

