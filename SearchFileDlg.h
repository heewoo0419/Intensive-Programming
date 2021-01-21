
// SearchFileDlg.h: 헤더 파일
//

#pragma once
#include "ListCtrlEx.h"


// CSearchFileDlg 대화 상자
class CSearchFileDlg : public CDialogEx
{
// 생성입니다.
public:
	CSearchFileDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEARCHFILE_DIALOG };
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
	DECLARE_MESSAGE_MAP()

public:
	CImageList m_img;
	CListCtrlEx m_lstResult;
	void SearFileNotSub();
	BOOL m_bStop;
	CString m_strToken;
	void SearFile(CString strStartFolder);
	CButton m_btnStart;
	CButton m_btn_Search;
	CStatic m_stcStatus;
	CTabCtrl m_tab;
	CString m_strFileLocation;
	CString m_strFileName;
	BOOL m_bSub;
	afx_msg void OnClickedButtonStart();
	afx_msg void OnClickedButtonStop();
};
