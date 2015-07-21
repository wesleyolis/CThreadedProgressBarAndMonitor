#pragma once

class ProgBarDoCancel {

public:
	ProgBarDoCancel() {}
	~ProgBarDoCancel() {}

	virtual void DoCancel() = 0;
private:
};

class ProgBar {

public:
	ProgBar();

	virtual ~ProgBar() {}

	virtual void SetMarqueMode(bool bEnable);

	virtual void SetStatus(CString csStatus);

	inline virtual void SetPos(int iPos, int UpGUIMask);

	virtual void SetRange(int iRange);

	virtual bool IsCancelled();

	virtual void DoCancel();

	virtual void SetCancelHandler(ProgBarDoCancel* pProgBarDoCancel);

	virtual void WaitForThreadAndPumpMessage(HANDLE ThreadFinished);

protected:
	bool m_bCancelled;
private:
	ProgBarDoCancel* m_pProgBarDoCancel;
};
