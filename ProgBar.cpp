#include "ProgBar.h"

ProgBar::ProgBar() {

	m_bCancelled = false;
	m_pProgBarDoCancel = nullptr;
}

void ProgBar::SetMarqueMode(bool bEnable) {}

void ProgBar::SetStatus(CString csStatus) {}

void ProgBar::SetPos(int iPos, int UpGUIMask) {}

void ProgBar::SetRange(int iRange) {}

bool ProgBar::IsCancelled() {

	return m_bCancelled;
}

void ProgBar::DoCancel()
{
	if (m_pProgBarDoCancel) {

		m_pProgBarDoCancel->DoCancel();
	}
	m_bCancelled = true;
}

void ProgBar::SetCancelHandler(ProgBarDoCancel* pProgBarDoCancel) {

	m_pProgBarDoCancel = pProgBarDoCancel;
}

void ProgBar::WaitForThreadAndPumpMessage(HANDLE ThreadFinished) {

	while (WAIT_OBJECT_0 != WaitForSingleObject(ThreadFinished, INFINITE));

	CloseHandle(ThreadFinished);
}

