
// SearchFileDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "SearchFile.h"
#include "SearchFileDlg.h"
#include "afxdialogex.h"
#include "FileFindEx.h"
#include <iostream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) { }

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSearchFileDlg 대화 상자



CSearchFileDlg::CSearchFileDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SEARCHFILE_DIALOG, pParent)
	, m_bSub(FALSE)
	, m_strFileLocation(_T(""))
	, m_strFileName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strToken = _T("");
}

void CSearchFileDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btn_Search);
	DDX_Control(pDX, IDC_LIST_RESULT, m_lstResult);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_stcStatus);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Text(pDX, IDC_EDIT_FILELOCATION, m_strFileLocation);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Check(pDX, IDC_CHECK_SUB, m_bSub);
}

BEGIN_MESSAGE_MAP(CSearchFileDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CSearchFileDlg::OnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CSearchFileDlg::OnClickedButtonStop)
END_MESSAGE_MAP()


// CSearchFileDlg 메시지 처리기

BOOL CSearchFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 리스트 컨트롤에 이미지 연결
	m_img.Create(IDB_LIST, 16, 2, RGB(255, 255, 255));
	m_lstResult.SetImageList(&m_img, LVSIL_SMALL);


	// 리스트 컨트롤 헤더 설정
	m_lstResult.InsertColumn(0, _T("이름"), LVCFMT_LEFT, 150);
	m_lstResult.InsertColumn(1, _T("위치"), LVCFMT_LEFT, 150);
	m_lstResult.InsertColumn(2, _T("크기(byte)"), LVCFMT_RIGHT, 80);
	m_lstResult.InsertColumn(3, _T("생성날짜"), LVCFMT_CENTER, 80);
	m_tab.InsertItem(0,_T("이름 및 위치"));


	TCHAR pBuf[256];
	GetCurrentDirectory(256, pBuf);
	m_strFileLocation.Format(_T("%s"), pBuf);
	UpdateData(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CSearchFileDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSearchFileDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSearchFileDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}



void CSearchFileDlg::SearFileNotSub() {
	// 하위폴더를 제외한 검색 구현
	UpdateData(TRUE);
	
	CString strTmp, strFolder, strName;
	CFileFindEx cfile;
	int i;
	BOOL b;
	MSG msg;

	// 시작 위치 지정, 검색 조건은 모든 파일
	if (m_strFileLocation.Right(1) == "\\")
		strTmp = m_strFileLocation + _T("*.*");
	else {
		strTmp = m_strFileLocation + _T("\\");
		strTmp += "*.*";
	}

	b = cfile.FindFile(strTmp);


	while (b) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 중지 버튼 클릭 시
		if (m_bStop) return;

		b = cfile.FindNextFile();

		// . .. 일 경우
		if (cfile.IsDots())	continue;

		i = m_lstResult.GetItemCount();

		// 현재 검색하는 폴더 보여주기
		

		// 검색 결과가 위치하는 폴더 얻기
		strFolder = cfile.GetFilePath().Left(cfile.GetFilePath().ReverseFind('\\') + 1);

		// 파일 (폴더) 이름 얻기
		strName = cfile.GetFileName();

		// 얻어진 결과를 대문자로 변경
		strName.MakeUpper();
		if (strName.Find(m_strToken) != -1) {
			// 리스트 컨트롤에 데이터 입력
			// 폴더이면
			if (cfile.IsDirectory()) {
				// 0번 그림과 함께 출력
				m_lstResult.AddItem(cfile.GetFileName(),i,0,-1,0);
				m_lstResult.AddItem(strFolder, i, 1);
				m_lstResult.AddItem(_T("파일폴더"), i, 2);
				m_lstResult.AddItem(cfile.GetCreationTimeString(), i, 3);
			}
			// 파일이면
			else {
				// 1번 그림과 함께 출력
				m_lstResult.AddItem(cfile.GetFileName(), i, 0, -1, 1);
				m_lstResult.AddItem(strFolder, i, 1);
				m_lstResult.AddItem(cfile.GetLengthString(), i, 2);
				m_lstResult.AddItem(cfile.GetCreationTimeString(), i, 3);
			}
		}
	}
}


void CSearchFileDlg::SearFile(CString strStartFolder) {
	UpdateData(TRUE);

	CString strTmp, strFolder, strName;
	CFileFindEx cfile;
	int i;
	BOOL b;
	MSG msg;

	if (strStartFolder.Right(1) == "\\")
		strTmp = strStartFolder + _T("*.*");
	else {
		strTmp = strStartFolder + _T("\\");
		strTmp += "*.*";
	}

	b = cfile.FindFile(strTmp);

	while (b) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (m_bStop) return;

		b = cfile.FindNextFile();

		if (cfile.IsDots()) continue;

		i = m_lstResult.GetItemCount();
		m_stcStatus.SetWindowText(cfile.GetFilePath());
		strFolder = cfile.GetFilePath().Left(cfile.GetFilePath().ReverseFind('\\') + 1);

		strName = cfile.GetFileName();
		strName.MakeUpper();

		if (cfile.IsDirectory()) {
			if (strName.Find(m_strToken) != -1) {
				m_lstResult.AddItem(cfile.GetFileName(), i, 0, -1, 0);
				m_lstResult.AddItem(strFolder, i, 1);
				m_lstResult.AddItem(_T("파일폴더"), i, 2);
				m_lstResult.AddItem(cfile.GetCreationTimeString(), i, 3);
			}
			SearFile(cfile.GetFilePath());
		}
		else {
			if (strName.Find(m_strToken) != -1) {
				m_lstResult.AddItem(cfile.GetFileName(), i, 0, -1, 1);
				m_lstResult.AddItem(strFolder, i, 1);
				m_lstResult.AddItem(cfile.GetLengthString(), i, 2);
				m_lstResult.AddItem(cfile.GetCreationTimeString(), i, 3);
			}
		}
	}
}


void CSearchFileDlg::OnClickedButtonStart() {
	UpdateData(TRUE);
	m_bStop = FALSE;											// 중지 초기화
	m_lstResult.DeleteAllItems();								// 리스트 초기화
	m_strToken = m_strFileName.MakeUpper();						// 비교 문자열 대문자 변환
	m_bSub ? SearFile(m_strFileLocation) : SearFileNotSub();	// 하위 폴더 검색 체크 확인

}

void CSearchFileDlg::OnClickedButtonStop() {
	UpdateData(TRUE);
	m_bStop = TRUE;		// 검색 중지
}
