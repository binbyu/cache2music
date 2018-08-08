
// CacheDlg.h : header file
//

#pragma once


struct CMusicInfo
{
	CString szName;
};

struct CUpdateListInfo
{
	BYTE cbCheck;
	int nItem;
	int nSubItem;
	CString* szText;

	CUpdateListInfo()
	{
		cbCheck = 0;
		nItem = -1;
		nSubItem = -1;
		szText = NULL;
	}
};

// CCacheDlg dialog
class CCacheDlg : public CDialogEx
{
// Construction
public:
	CCacheDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CACHE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonStart();
	afx_msg LRESULT OnMessageLoadFiles(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageUpdateList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageUpdateProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageEnableButton(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageLog(WPARAM wParam, LPARAM lParam);

private:
	BOOL CheckCache(CString szUc, CString& szFormat);
	BOOL GetMusicInfo(CString szId, CMusicInfo& info);
	BOOL ConvertMusic(CString szUc, CString szMusic);
	BOOL HttpRequest(CString szId, std::string& response);
	char* tstr_to_utf8(const TCHAR* in);
	TCHAR* utf8_to_tstr(const char* in);
	void log_print(CString szLog);
	static DWORD WINAPI ThreadProcDownload(void* param);
	static DWORD WINAPI ThreadProcStart(void* param);

private:
	HANDLE m_hThreadDown;
	HANDLE m_hThreadStart;
	CString m_szLastError;
};
