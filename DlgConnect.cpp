// DlgConnect.cpp: 구현 파일
//

#include "pch.h"
#include "BingoCnt.h"
#include "DlgConnect.h"
#include "afxdialogex.h"


// CDlgConnect 대화 상자

IMPLEMENT_DYNAMIC(CDlgConnect, CDialog)

CDlgConnect::CDlgConnect(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_CONNECT, pParent)
	, m_strIP(_T(""))
{

}

CDlgConnect::~CDlgConnect()
{
}

void CDlgConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IP, m_strIP);
}


BEGIN_MESSAGE_MAP(CDlgConnect, CDialog)
END_MESSAGE_MAP()


// CDlgConnect 메시지 처리기
