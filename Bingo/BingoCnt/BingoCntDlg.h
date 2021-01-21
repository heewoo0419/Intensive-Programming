
// BingoCntDlg.h: 헤더 파일
//

#pragma once

// CBingoCntDlg 대화 상자
class CBingoCntDlg : public CDialogEx
{
// 생성입니다.
public:
	CBingoCntDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CSocCom m_socCom;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BINGOCNT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LPARAM OnReceive(UINT wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedButtonConnect();
	CString m_strConnect;
	CString m_strMe;
	CString m_strStatus;
	CListBox m_list;
	CString m_strSend;

	int m_iOrder;

	BOOL m_bConnect;	// 다른쪽과 접속했을때 TRUE
	BOOL m_bStartSvr;	// 서버가 준비 끝나면 TRUE
	BOOL m_bStart;		// 1~25를 다 채우면 TRUE
	BOOL m_bMe;			// 내가 선택할 차예이면 TRUE
	BOOL m_bCntEnd;		// 클라이언트가 끝났는지 검사
	BOOL m_bSvrEnd;		// 서버가 끝났는지 검사
	
	int m_iGame[5][5];
	BOOL m_bGame[5][5];


	void SendGame(int iType, CString strTmp);
	void DrawRec();
	void DrawLine();
	void DrawNum(int iRow, int iCol, int iNum);
	void DrawCheck(int iRow, int iCol);
	void InitGame();

	void OrderNum(int iRow, int iCol);
	void PosToIndex(CPoint pnt, int& iRow, int& iCol);
	void NumToIndex(int iNum, int& iRow, int& iCol);
	BOOL IsGameEnd();
	void SetGameEnd();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonSend();
};
