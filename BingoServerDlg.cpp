
// BingoServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "BingoServer.h"
#include "BingoServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBingoServerDlg 대화 상자



CBingoServerDlg::CBingoServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BINGOSERVER_DIALOG, pParent)
	, m_strStatus(_T("접속 대기중 입니다."))
	, m_strMe(_T("대기중"))
	, m_strConnect(_T("대기중"))
	, m_strSend(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBingoServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_STATIC_ME, m_strMe);
	DDX_Text(pDX, IDC_STATIC_CONNECT, m_strConnect);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
}

BEGIN_MESSAGE_MAP(CBingoServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CBingoServerDlg::OnBnClickedButtonSend)
	ON_MESSAGE(UM_ACCEPT, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnAccept)
	ON_MESSAGE(UM_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnReceive)
END_MESSAGE_MAP()


// CBingoServerDlg 메시지 처리기

BOOL CBingoServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_socCom = NULL;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// 서버 소켓 생성(포트번호 : 5000)
	if (m_socServer.Create(5000) == FALSE) {
		int err = m_socServer.GetLastError();
		CString error = _T("ERROR : Fail to create Server (ERROR CODE : ");
		CString strErr = _T("");
		strErr.Format(_T("%d)"), err);
		AfxMessageBox(error + strErr);
	}
	else {
		MessageBox(_T("Success to Create Socket Server"));
	}
	// 클라이언트 접속 대기
	m_socServer.Listen();

	// 소켓 클래스와 메인 윈도우 (여기서는 CChatServerDlg)를 연결
	m_socServer.Init(this->m_hWnd);

	InitGame();
	m_bConnect = FALSE;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CBingoServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CBingoServerDlg::OnPaint()
{
	if (IsIconic())
	{
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
	else
	{
		CDialogEx::OnPaint();
	}

	DrawRec();
	DrawLine();

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			DrawNum(i, j, m_iGame[i][j]);
			if (m_bGame[i][j])	DrawCheck(i, j);
		}
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CBingoServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBingoServerDlg::SendGame(int iType, CString strTmp)
{
	// 데이터 전송
	UpdateData(TRUE);
	char pTmp[256];
	memset(pTmp, '\0', 256);
	sprintf(pTmp, "%d%s", iType, strTmp);

	m_socCom->Send(pTmp, 256);
}

LPARAM CBingoServerDlg::OnAccept(UINT wParam, LPARAM lParam) {
	MessageBox(_T("접속 요청 도착"));
	// 클라이언트에서 접속 요청이 왔을 떄


	// 통신용 소켓을 생성한 뒤
	m_socCom = new CSocCom;

	// 서버소켓과 통신소켓 연결
	m_socCom = m_socServer.GetAcceptSocCom();

	m_socCom->Init(this->m_hWnd);
	m_strConnect = _T("접속성공");
	m_strStatus = _T("게임을 초기화 하십시오.");

	SendGame(SOC_TEXT, "접속성공");
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	m_bConnect = TRUE;
	//UpdateData(FALSE);
	return TRUE;
}

LPARAM CBingoServerDlg::OnReceive(UINT wParam, LPARAM lParam) {
	// 접속된 곳에서 데이터가 도착했을 때

	char pTmp[256];
	CString strTmp, str;
	memset(pTmp, '\0', 256);

	// 데이터를 pTmp에 받는다
	m_socCom->Receive(pTmp, 256);

	// strTmp에 헤더 저장
	strTmp.Format(_T("%c"), pTmp[0]);

	int iType = atoi((char*)(LPCTSTR)strTmp);

	if (iType == SOC_GAMESTART) {
		m_bStartSvr = TRUE;

		if (m_bStart) {
			m_strMe = _T("상대방의 차례입니다.");
			m_strStatus = _T("대기 하세요.");
			m_bMe = FALSE;
			UpdateData(FALSE);
		}

	}

	else if (iType == SOC_TEXT) {
		str.Format(_T("%s"), pTmp + 1);
		m_list.AddString(str);
	}

	else if (iType == SOC_CHECK) {
		str.Format(_T("%s"), pTmp + 1);
		int iRow = -1, iCol = -1;
		NumToIndex(atoi((char*)(LPCTSTR)str), iRow, iCol);

		DrawCheck(iRow, iCol);

		// 차례 변경
		m_bMe = TRUE;
		m_strMe = _T("당신의 차례입니다.");
		m_strStatus = _T("원하는 곳을 선택 하세요.");
		UpdateData(FALSE);

		if (IsGameEnd()) {
			m_bCntEnd = TRUE;
			SendGame(SOC_GAMEEND, "");
			Sleep(1000);
			SetGameEnd();
		}
	}

	else if (iType == SOC_GAMEEND) {
		m_bSvrEnd = TRUE;
		Sleep(1000);
		SetGameEnd();
	}
	return TRUE;
}


void CBingoServerDlg::DrawRec()
{
	// 사각형 그리기 (250 *250 시작은 (10, 10))
	CClientDC dc(this);
	CBrush br;

	br.CreateSolidBrush(RGB(62, 62, 124));

	CBrush* lbr = dc.SelectObject(&br);

	dc.Rectangle(10, 10, 10 + 250, 10 + 250);

	dc.SelectObject(lbr);

}
void CBingoServerDlg::DrawLine()
{
	// TODO: 여기에 구현 코드 추가.

	CClientDC dc(this);

	CPen pen;

	pen.CreatePen(PS_SOLID, 2, RGB(128, 128, 128));

	CPen* lodp = dc.SelectObject(&pen);

	for (int i = 0; i < 6; i++) {
		dc.MoveTo(10, 10 + i * 50);
		dc.LineTo(50 * 5 + 10, 10 + i * 50);

	}

	for (int i = 0; i < 6; i++) {
		dc.MoveTo(10 + i * 50, 10);
		dc.LineTo(10 + i * 50, 50 * 5 + 10);
	}

	dc.SelectObject(pen);
}


void CBingoServerDlg::DrawNum(int iRow, int iCol, int iNum)
{
	// TODO: 여기에 구현 코드 추가.

	CString str;
	str.Format(_T("%d"), iNum);

	CClientDC dc(this);

	// 원래 있던 숫자 0을 지운다
	CBrush br;
	CPen pen;

	pen.CreatePen(PS_SOLID, 1, RGB(62, 62, 124));
	CPen* lodp = dc.SelectObject(&pen);

	br.CreateSolidBrush(RGB(62, 62, 124));
	CBrush* lbr = dc.SelectObject(&br);
	dc.Rectangle(12 + iCol * 50, 12 + iRow * 50, 12 + iCol * 50 + 45, 12 + iRow * 50 + 45);

	dc.SetTextColor(RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);

	if (str.GetLength() > 1)	dc.TextOut(27 + iCol * 50, 30 + iRow * 50, str);
	else dc.TextOut(30 + iCol * 50, 30 + iRow * 50, str);

	dc.SelectObject(lbr);
	dc.SelectObject(lodp);

}


void CBingoServerDlg::DrawCheck(int iRow, int iCol)
{
	// TODO: 여기에 구현 코드 추가.

	// 화면과 m_bGame 배열을 함께 체크한다
	m_bGame[iRow][iCol] = TRUE;

	CString str;
	str.Format(_T("%d"), m_iGame[iRow][iCol]);

	CClientDC dc(this);

	// 다른색으로 칠하기
	CBrush br;
	br.CreateSolidBrush(RGB(124, 0, 0));
	CBrush* lbr = dc.SelectObject(&br);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(124, 0, 0));
	CPen* lodp = dc.SelectObject(&pen);

	dc.Rectangle(11 + iCol * 50, 11 + iRow * 50, 11 + iCol * 50 + 48, 11 + iRow * 50 + 48);

	// 원하는 숫자를 그려준다.
	dc.SetTextColor(RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);


	if (str.GetLength() > 1)	dc.TextOut(27 + iCol * 50, 30 + iRow * 50, str);
	else dc.TextOut(30 + iCol * 50, 30 + iRow * 50, str);

	dc.SelectObject(lbr);
	dc.SelectObject(lodp);


}


void CBingoServerDlg::InitGame()
{
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			m_iGame[i][j] = 0;
			m_bGame[i][j] = FALSE;
		}
	}

	m_bStart = FALSE;
	m_bMe = FALSE;
	m_bSvrEnd = FALSE;
	m_bCntEnd = FALSE;
	m_iOrder = 1;
}

void CBingoServerDlg::OrderNum(int iRow, int iCol)
{
	if (m_bConnect && m_bStart)	return;

	// 마우스를 클릭하는 순서대로 번호를 메긴다
	if (!m_iGame[iRow][iCol]) {
		m_iGame[iRow][iCol] = m_iOrder++;
		DrawNum(iRow, iCol, m_iGame[iRow][iCol]);
	}

	if (m_iOrder > 25) {
		// 게임 시작을 보낸다.
		SendGame(SOC_GAMESTART, "");
		m_bStart = TRUE;

		if (m_bStartSvr) {
			m_strMe = _T("상대방의 차례 입니다.");
			m_strStatus = _T("대기하세요.");
			m_bMe = FALSE;
			UpdateData(FALSE);
		}
	}
}

void CBingoServerDlg::PosToIndex(CPoint pnt, int& iRow, int& iCol) {
	int i, j;


	// 행 결정
	for (i = 0; i < 5; i++) {
		if (pnt.y > 10 + (i * 50) && pnt.y <= (10 + ((i + 1) * 50)))
			break;
	}
	// 행 결정
	for (j = 0; j < 5; j++) {
		if ((pnt.x > 10 + j * 50) && (pnt.x <= (10 + (j + 1) * 50)))
			break;
	}
	iRow = i;
	iCol = j;
}


void CBingoServerDlg::NumToIndex(int iNum, int& iRow, int& iCol) {
	// 숫자를 맞는 인덱스로 변환
	int i, j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (iNum = m_iGame[i][j]) {
				iRow = i;
				iCol = j;
				break;
			}
		}
	}
}


BOOL CBingoServerDlg::IsGameEnd()
{
	int iLine = 0;
	int i, j;

	// 가로 검사
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (!m_bGame[i][j])
				break;
		}
		if (j == 5)	iLine++;
	}

	// 세로 검사
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (!m_bGame[j][j])
				break;
		}
		if (j == 5) iLine++;
	}


	// 대각선 검사 (\ 방향)
	for (i = 0; i < 5; i++) {
		if (!m_bGame[i][i])
			break;
	}
	if (i == 5) iLine++;

	// 대각선 검사 (/ 방향)
	for (i = 0, j = 4; i < 5; i++, j--) {
		if (!m_bGame[i][j])
			break;
	}
	if (i == 5) iLine++;

	if (iLine >= 5)	return TRUE;
	else return FALSE;
}


void CBingoServerDlg::SetGameEnd()
{
	if (!m_bStart)	return;

	if (m_bCntEnd && m_bSvrEnd) {
		MessageBox(_T("무승부"));
		m_strStatus = _T("게임 끝 !! 무승부 입니다");
	}
	else if (!m_bCntEnd && m_bSvrEnd) {
		MessageBox(_T("패"));
		m_strStatus = _T("게임 끝 !! 졌습니다");
	}
	else {
		MessageBox(_T("승"));
		m_strStatus = _T("게임 끝 !! 이겼습니다");
	}
	m_bStart = FALSE;
}



void CBingoServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 게임과 관련 없는 곳 클릭 시
	if (point.x > 260 || point.y > 260)	return;
	if (point.x < 10 || point.y < 10)	return;
	if (!m_bConnect)	return;

	int iRow = -1, iCol = -1;
	PosToIndex(point, iRow, iCol);


	if (m_bStart && m_bStartSvr && m_bMe) {
		if (!m_bGame[iRow][iCol]) {
			DrawCheck(iRow, iCol);
			// 선택한 숫자를 전송한다.
			CString str;
			str.Format(_T("%02d", m_iGame[iRow][iCol]));
			SendGame(SOC_CHECK, str);

			// 차례 변경
			m_bMe = FALSE;
			m_strMe = _T("상대방의 차례 입니다.");
			m_strStatus = _T("대기하세요.");
			UpdateData(FALSE);

			if (IsGameEnd()) {
				m_bCntEnd = TRUE;
				SendGame(SOC_GAMEEND, "");
				Sleep(1000);
				SetGameEnd();
				InitGame();
				Invalidate(TRUE);
			}
		}
	}
	else {
		OrderNum(iRow, iCol);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CBingoServerDlg::OnBnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	char pTmp[256];
	CString strTmp;
	memset(pTmp, '\0', 256);
	memcpy(pTmp, (unsigned char*)(LPCTSTR)m_strSend, 256);
	//strcpy(pTmp, LPSTR(LPCTSTR(m_strSend)));

	// 전송
	m_socCom->Send(pTmp, 256);
	SendGame(SOC_TEXT, m_strSend);
	// 전송한 데이터도 리스트박스에 보여준다
	strTmp.Format(_T("%s"), pTmp);
	int i = m_list.GetCount();
	m_list.InsertString(i, strTmp);

}
