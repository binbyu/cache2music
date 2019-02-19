
// CacheDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cache.h"
#include "CacheDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LOG_PRINT(x) log_print(x)
#define LOG_PRINT_EX(x) _this->log_print(x)


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCacheDlg dialog




CCacheDlg::CCacheDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCacheDlg::IDD, pParent)
	, m_hThreadDown(NULL)
	, m_hThreadStart(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCacheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCacheDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CCacheDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER2, &CCacheDlg::OnBnClickedButtonBrowser2)
	ON_BN_CLICKED(IDC_BUTTON_START, &CCacheDlg::OnBnClickedButtonStart)
	ON_MESSAGE(WM_LOAD_FILES, &CCacheDlg::OnMessageLoadFiles)
	ON_MESSAGE(WM_UPDATE_LIST, &CCacheDlg::OnMessageUpdateList)
	ON_MESSAGE(WM_UPDATE_PROGRESS, &CCacheDlg::OnMessageUpdateProgress)
	ON_MESSAGE(WM_ENABLE_BTN, &CCacheDlg::OnMessageEnableButton)
	ON_MESSAGE(WM_LOG, &CCacheDlg::OnMessageLog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CCacheDlg::OnBnClickedButtonDown)
END_MESSAGE_MAP()


// CCacheDlg message handlers

BOOL CCacheDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//PostMessage(WM_LBUTTONDBLCLK);

	// Init list
	CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST);
	CRect rect;
	list->GetClientRect(rect);
	list->SetExtendedStyle(list->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES /*| LVS_EX_GRIDLINES | LVS_EX_AUTOCHECKSELECT*/);
	list->InsertColumn(1, _T("Cache"), LVCFMT_LEFT, rect.Width() / 2, 0);
	list->InsertColumn(2, _T("Music"), LVCFMT_LEFT, rect.Width() / 2, 1);
	list->InsertColumn(3, _T("Format"), LVCFMT_LEFT, rect.Width() / 8, 2);

	// process
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS))->SetRange(0, 100);

	// disable button
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);

	// init network
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCacheDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCacheDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCacheDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCacheDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_hThreadDown)
	{
		TerminateThread(m_hThreadDown, 0);
		CloseHandle(m_hThreadDown);
	}
	if (m_hThreadStart)
	{
		TerminateThread(m_hThreadStart, 0);
		CloseHandle(m_hThreadStart);
	}
	WSACleanup();
	CDialogEx::OnClose();
}


void CCacheDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	static BOOL s_Initd = FALSE;
	static BOOL s_bMax = FALSE;
	static CRect s_rcMax;
	static CRect s_rcMin;

	if (!s_Initd)
	{
		s_Initd = TRUE;
		GetWindowRect(s_rcMax);
		GetDlgItem(IDC_EDIT_LOG)->GetWindowRect(s_rcMin);
		s_rcMin.right = s_rcMin.left;
		s_rcMin.top = s_rcMax.top;
		s_rcMin.bottom = s_rcMax.bottom;
		s_rcMin.left = s_rcMax.left;
	}

	CRect rect = s_bMax ? s_rcMax : s_rcMin;
	MoveWindow(rect, TRUE);
	s_bMax = !s_bMax;

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CCacheDlg::OnBnClickedButtonBrowser()
{
	// TODO: Add your control notification handler code here
	CFolderPickerDialog dlg;
	TCHAR szPath[MAX_PATH] = {0};
	if (IDOK == dlg.DoModal())
	{
		GetCurrentDirectory(MAX_PATH-1, szPath);
		SetDlgItemText(IDC_EDIT_PATH, dlg.GetPathName());
		SetDlgItemText(IDC_EDIT_PATH2, CString(szPath)+_T("\\music"));
		PostMessage(WM_LOAD_FILES, 0, NULL);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_PATH, _T(""));
		SetDlgItemText(IDC_EDIT_PATH2, _T(""));
		((CListCtrl*)GetDlgItem(IDC_LIST))->DeleteAllItems();
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);
	}
}

void CCacheDlg::OnBnClickedButtonBrowser2()
{
	CString path;
	GetDlgItemText(IDC_EDIT_PATH2, path);
	CreateDirectory(path, NULL);
	ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWDEFAULT);
}


void CCacheDlg::OnBnClickedButtonDown()
{
	// TODO: Add your control notification handler code here
	if (!m_hThreadDown)
	{
		DWORD dwThreadId;
		m_hThreadDown = CreateThread(NULL, 0, ThreadProcDownload, this, CREATE_SUSPENDED, &dwThreadId);
		ResumeThread(m_hThreadDown);
	}
}


void CCacheDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	if (!m_hThreadStart)
	{
		DWORD dwThreadId;
		m_hThreadStart = CreateThread(NULL, 0, ThreadProcStart, this, CREATE_SUSPENDED, &dwThreadId);
		ResumeThread(m_hThreadStart);
	}
}


LRESULT CCacheDlg::OnMessageLoadFiles(WPARAM wParam, LPARAM lParam)
{
	CString szPath;
	CString szFilter;
	CString szFormat;
	WIN32_FIND_DATA findData;
	HANDLE hFind = NULL;
	CListCtrl* list = NULL;
	int index = 0;

	list = (CListCtrl*)GetDlgItem(IDC_LIST);
	list->DeleteAllItems();
	GetDlgItemText(IDC_EDIT_PATH, szPath);
	if (szPath.Right(1) != _T("\\"))
		szPath.Append(_T("\\"));
	szFilter = szPath + _T("*.uc");

	hFind = FindFirstFile(szFilter, &findData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return 1L;
	}

	do
	{
		if (!CheckCache(szPath + findData.cFileName, szFormat))
			continue;
		list->InsertItem(index, _T(""));
		list->SetItemText(index, 0, findData.cFileName);
		list->SetItemText(index, 2, szFormat);
		list->SetCheck(index, TRUE);
		list->SetItemState(index++, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	} while (FindNextFile(hFind, &findData));
	FindClose(hFind);

	return 0L;
}


LRESULT CCacheDlg::OnMessageUpdateList(WPARAM wParam, LPARAM lParam)
{
	CUpdateListInfo* info = (CUpdateListInfo*)lParam;

	CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST);

	if (info->cbCheck & 0xF0)
	{
		list->SetCheck(info->nItem, info->cbCheck & 0x0F);
	}
	else
	{
		list->SetItemText(info->nItem, info->nSubItem, *info->szText);
	}

	if (info->szText)
		delete info->szText;
	delete info;

	return 0L;
}


LRESULT CCacheDlg::OnMessageUpdateProgress(WPARAM wParam, LPARAM lParam)
{
	int pos = (int)wParam;

	CProgressCtrl* pctrl = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pctrl->SetPos(pos);

	return 0L;
}


LRESULT CCacheDlg::OnMessageEnableButton(WPARAM wParam, LPARAM lParam)
{
	BYTE enable = (BYTE)wParam;

	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(enable & 0x0F);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(enable & 0xF0);

	return 0L;
}


LRESULT CCacheDlg::OnMessageLog(WPARAM wParam, LPARAM lParam)
{
	CString* log = (CString*)lParam;

	CEdit* ctrl = (CEdit*)GetDlgItem(IDC_EDIT_LOG);
	ctrl->SetSel(-1);
	ctrl->ReplaceSel(*log+_T("\r\n"));
	ctrl->SetSel(-1);

	delete log;
	return 0L;
}


BOOL CCacheDlg::CheckCache(CString szUc, CString& szFormat)
{
#define goto_end(x) \
	do \
	{ \
	if (fp) \
	fclose(fp); \
	if (buf) \
	free(buf); \
	return x; \
	} while (FALSE);

	CString szIdx = szUc.Left(szUc.ReverseFind(_T('.'))) + _T(".idx");
	CString szInfo = szUc.Left(szUc.ReverseFind(_T('.'))) + _T(".info");
	FILE* fp = NULL;
	long len = 0;
	char* buf = NULL;
	char* begin = NULL;
	char* end = NULL;

	// check if download completed
	fp = _tfopen(szIdx, _T("rb"));
	if (!fp)
		goto_end(FALSE);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = (char*)malloc(len + 1);
	if (!buf)
		goto_end(FALSE);
	fread(buf, 1, len, fp);
	fclose(fp);
	fp = NULL;
	buf[len] = 0;
	begin = buf;
	begin += strlen("{\"size\":\"");
	end = strstr(begin, "\"");
	begin[end-begin] = 0;

	fp = _tfopen(szUc, _T("rb"));
	if (!fp)
		goto_end(FALSE);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fclose(fp);
	fp = NULL;

	if (len != atoi(begin))
		goto_end(FALSE);

	// Get format
	fp = _tfopen(szInfo, _T("rb"));
	if (!fp)
		goto_end(FALSE);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	begin = (char*)realloc(buf, len + 1);
	if (!begin)
		goto_end(FALSE);
	buf = begin;
	fread(buf, 1, len, fp);
	fclose(fp);
	fp = NULL;
	buf[len] = 0;
	begin = buf;
	begin += strlen("{\"format\":\"");
	end = strstr(begin, "\"");
	begin[end-begin] = 0;
	szFormat = utf8_to_tstr(begin);

	goto_end(TRUE);
}


BOOL CCacheDlg::GetMusicInfo(CString szId, CMusicInfo& info)
{
	std::string response;
	static char _name[256] = {0};

	if (!HttpRequest(szId, response))
		return FALSE;

	// parse response Json
	memset(_name, 0, sizeof(_name));
	const char* ARTISTS_NAME = "\"artists\":[{\"name\":\"";
	const char* artists_name = strstr(response.c_str(), ARTISTS_NAME);
	artists_name += strlen(ARTISTS_NAME);
	const char* artists_name_end = strstr(artists_name, "\"");
	memcpy(_name, artists_name, artists_name_end - artists_name);

	memcpy(_name+strlen(_name), " - ", strlen(" - "));

	const char* SONG_NAME = "{\"songs\":[{\"name\":\"";
	const char* song_name = strstr(response.c_str(), SONG_NAME);
	song_name += strlen(SONG_NAME);
	const char* song_name_end = strstr(song_name, "\"");
	memcpy(_name+strlen(_name), song_name, song_name_end - song_name);

	// covert utf8 to unicode
	info.szName = utf8_to_tstr(_name);

	return TRUE;
}


BOOL CCacheDlg::ConvertMusic(CString szUc, CString szMusic)
{
#define goto_end(x) \
	do \
	{ \
	if (fin) \
	fclose(fin); \
	if (fout) \
	fclose(fout); \
	return x; \
} while (FALSE);

	FILE* fin = NULL;
	FILE* fout = NULL;
	char buf[4096] = {0};
	int size = 0;

	fin = _tfopen(szUc, _T("rb"));
	fout = _tfopen(szMusic, _T("wb"));
	if (!fin || !fout)
		goto_end(FALSE);

	do
	{
		size = fread(buf, 1, 4096, fin);
		for (int i=0; i<size; i++)
		{
			buf[i] ^= 0xA3;
		}
		if (size > 0)
		{
			fwrite(buf, 1, size, fout);
		}
	} while (size > 0);

	goto_end(TRUE);
}


BOOL CCacheDlg::HttpRequest(CString szId, std::string& response)
{
#define goto_end(x,y) \
	do \
	{ \
	if (INVALID_SOCKET != sockfd) \
	closesocket(sockfd); \
	if (_T("") != y) \
	LOG_PRINT(y);\
	return x; \
} while (FALSE);

	//  http://music.163.com/api/song/detail/?id=30987293&ids=%5B30987293%5D

	struct hostent *host = NULL;
	in_addr addr;
	SOCKET sockfd = INVALID_SOCKET;
	int ret = 0;
	struct sockaddr_in sin = {0};
	std::string header;
	char buf[4096] = {0};
	int size = 0;
	int content_length = -1;
	int chunkedsize = 0;

	host = gethostbyname("music.163.com");
	if (!host)
		goto_end(FALSE, _T("ERROR: gethostbyname fail."));
	addr = *(struct in_addr*)host->h_addr_list[0];

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sockfd)
		goto_end(FALSE, _T("ERROR: create socket fail."));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);
	sin.sin_addr = addr;
	ret = connect(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
	if(ret)
	{
		
		goto_end(FALSE, _T("ERROR: connect to music.163.com fail."));
	}

	// send request
	header.append("GET /api/song/detail/?id=");
	header.append(tstr_to_utf8(szId.GetBuffer()));
	header.append("&ids=%5B");
	header.append(tstr_to_utf8(szId.GetBuffer()));
	header.append("%5D");
	header.append(" HTTP/1.1\r\nHost: music.163.com\r\n");
	header.append("User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\r\n");
	header.append("\r\n");
	send(sockfd, header.c_str(), header.size(), 0);

	// read response header
	header.clear();
	while (1)
	{
		ret = recv(sockfd, buf, 1, 0);
		if (ret > 0)
		{
			header.append(buf);
			size += ret;
			if (size >= 4)
			{
				if ('\n' == header[size-1]
				&& '\r' == header[size-2]
				&& '\n' == header[size-3]
				&& '\r' == header[size-4])
				{
					// recv header finished.
					break;
				}
			}
		}
	}

	// parse 'Content-Length'
	transform(header.begin(), header.end(), header.begin(), tolower);
	if (header.find("content-length") != -1)
	{
		header = header.substr(header.find("content-length"),std::string::npos);
		header = header.substr(header.find(":")+1,std::string::npos);
		header = header.substr(0,header.find("\r\n"));
		content_length = atoi(header.c_str());

		// recv body
		response.clear();
		while (1)
		{
			memset(buf, 0, sizeof(buf));
			ret = recv(sockfd, buf, 4096-1, 0);
			if (ret > 0)
			{
				response.append(buf);
				content_length -= ret;
				if (content_length <= 0)
				{
					break;
				}
			}
		}
	}
	else
	{
		//Transfer-Encoding: chunked

		// recv body
		response.clear();
		size = 0;
		header.clear();
		while (1)
		{
			memset(buf, 0, 2);
			ret = recv(sockfd, buf, 1, 0);
			if (ret > 0)
			{
				header.append(buf);
				size += ret;
				if (size >= 2)
				{
					if ('\n' == header[size-1]
					&& '\r' == header[size-2])
					{
						if (size == 2)
						{
							header.clear();
							size = 0;
							continue;
						}

						// chunked size
						header[size-2] = 0;
						sscanf(header.c_str(), "%x\r\n", &chunkedsize);
						if (chunkedsize == 0)
							break;

						header.clear();
						size = 0;

						// recv chunked data
						while (chunkedsize > 0)
						{
							memset(buf, 0, sizeof(buf));
							ret = recv(sockfd, buf, 4096-1 > chunkedsize ? chunkedsize : 4096-1, 0);
							if (ret > 0)
							{
								response.append(buf);
								chunkedsize -= ret;
							}
						}
					}
				}
			}
		}
	}
	
	goto_end(TRUE, _T(""));
}


char* CCacheDlg::tstr_to_utf8(const TCHAR* in)
{
	static char* utf8 = NULL;
	int len = 0;

#ifdef _UNICODE
	len = WideCharToMultiByte(CP_UTF8, 0, in, wcslen(in), NULL, 0, NULL, NULL);
	utf8 = (char*)realloc(utf8, len+1);
	WideCharToMultiByte(CP_UTF8, 0, in, wcslen(in), utf8, len, NULL, NULL);
	utf8[len] = 0;
#else
	
#endif

	return utf8;
}


TCHAR* CCacheDlg::utf8_to_tstr(const char* in)
{
	static TCHAR* tstr = NULL;
	int len = 0;

#ifdef _UNICODE
	len = MultiByteToWideChar(CP_UTF8, 0, in, strlen(in), NULL, 0);
	tstr = (TCHAR*)realloc(tstr, (len+1)*sizeof(TCHAR));
	MultiByteToWideChar(CP_UTF8, 0, in, strlen(in), tstr, len);
	tstr[len] = 0;
#else

#endif

	return tstr;
}


void CCacheDlg::log_print(CString szLog)
{
	CString* log = new CString(szLog);
	PostMessage(WM_LOG, 0, (LPARAM)log);
}


DWORD WINAPI CCacheDlg::ThreadProcDownload(void* param)
{
	CCacheDlg* _this = (CCacheDlg*)param;
	CListCtrl* list = NULL;
	CString szUc;
	CString szFmt;
	CString szName;
	CString szId;
	CMusicInfo info;
	CUpdateListInfo* listInfo = NULL;

	LOG_PRINT_EX(_T(">>> Start get music name by network..."));
	list = (CListCtrl*)_this->GetDlgItem(IDC_LIST);
	_this->PostMessage(WM_ENABLE_BTN, 0x00, NULL);
	_this->PostMessage(WM_UPDATE_PROGRESS, 0, NULL);
	for (int i=0; i<list->GetItemCount(); i++)
	{
		if (!list->GetCheck(i))
		{
			continue;
		}

		listInfo = new CUpdateListInfo;
		listInfo->nItem = i;

		szUc = list->GetItemText(i, 0);
		szFmt = list->GetItemText(i, 2);
		szId = szUc.Left(szUc.Find(_T('-')));
		if (!_this->GetMusicInfo(szId, info))
		{
			LOG_PRINT_EX(_T("Get music name fail. id=")+szId);
			// set check
			listInfo->cbCheck = 0x10;
			_this->PostMessage(WM_UPDATE_LIST, 0, (LPARAM)listInfo);
			continue;
		}

		// set item text
		listInfo->nSubItem = 1;
		listInfo->szText = new CString(info.szName+_T(".")+szFmt);
		LOG_PRINT_EX(_T("Get music name success. name=")+*listInfo->szText);
		_this->PostMessage(WM_UPDATE_LIST, 0, (LPARAM)listInfo);

		// update progress
		_this->PostMessage(WM_UPDATE_PROGRESS, i*100/list->GetItemCount(), NULL);
	}

	_this->PostMessage(WM_ENABLE_BTN, 0x10);
	_this->PostMessage(WM_UPDATE_PROGRESS, 100, NULL);
	LOG_PRINT_EX(_T(">>> Get music name by network completed."));

	// clear
	CloseHandle(_this->m_hThreadDown);
	_this->m_hThreadDown = NULL;
	return 0;
}


DWORD WINAPI CCacheDlg::ThreadProcStart(void* param)
{
	CCacheDlg* _this = (CCacheDlg*)param;
	CListCtrl* list = NULL;
	CString szUc;
	CString szMusic;
	CString szPath;

	LOG_PRINT_EX(_T(">>> Start convert uc to music..."));
	CreateDirectory(/*szPath+*/_T("music"), NULL);
	_this->GetDlgItemText(IDC_EDIT_PATH, szPath);
	if (szPath.Right(1) != _T("\\"))
		szPath.Append(_T("\\"));
	list = (CListCtrl*)_this->GetDlgItem(IDC_LIST);
	_this->PostMessage(WM_ENABLE_BTN, 0x00, NULL);
	_this->PostMessage(WM_UPDATE_PROGRESS, 0, NULL);

	for (int i=0; i<list->GetItemCount(); i++)
	{
		if (!list->GetCheck(i))
		{
			continue;
		}
		szUc = list->GetItemText(i, 0);
		szMusic = list->GetItemText(i, 1);

		if (_this->ConvertMusic(szPath+szUc, /*szPath+*/_T("music\\")+szMusic))
			LOG_PRINT_EX(_T("success convert music "+szMusic));
		else
			LOG_PRINT_EX(_T("fail convert music "+szMusic));
		// update progress
		_this->PostMessage(WM_UPDATE_PROGRESS, i*100/list->GetItemCount(), NULL);
	}

	_this->PostMessage(WM_ENABLE_BTN, 0x01);
	_this->PostMessage(WM_UPDATE_PROGRESS, 100, NULL);
	LOG_PRINT_EX(_T(">>> Convert uc to music completed."));
	if (IDOK == _this->MessageBox(_T("转换成功\r\n点击确认打开mp3目录"), _T("转换成功"), MB_OK))
	{
		_this->OnBnClickedButtonBrowser2();
	}

	// clear
	CloseHandle(_this->m_hThreadStart);
	_this->m_hThreadStart = NULL;
	return 0;
}
