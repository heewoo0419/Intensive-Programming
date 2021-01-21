
// EditorView.cpp: CEditorView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Editor.h"
#endif

#include "EditorDoc.h"
#include "EditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEditorView

IMPLEMENT_DYNCREATE(CEditorView, CView)

BEGIN_MESSAGE_MAP(CEditorView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CEditorView 생성/소멸

CEditorView::CEditorView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	pntCur.x = 0;
	pntCur.y = 0;
}

CEditorView::~CEditorView()
{
}

BOOL CEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CEditorView 그리기

void CEditorView::OnDraw(CDC* /*pDC*/)
{
	CEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CEditorView 인쇄

BOOL CEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CEditorView 진단

#ifdef _DEBUG
void CEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEditorDoc* CEditorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEditorDoc)));
	return (CEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CEditorView 메시지 처리기


int CEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	pDoc = GetDocument();

	TEXTMETRIC txtKey;

	CClientDC dc(this);

	dc.GetTextMetrics(&txtKey);	// 현 dc(View)의 폰트정보 얻기

	CreateSolidCaret(txtKey.tmAveCharWidth / 2, txtKey.tmHeight);

	CPoint pntTmp(0, 0);

	SetCaretPos(pntTmp);

	ShowCaret();

	return 0;
}


void CEditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CClientDC dc(this);

	TEXTMETRIC txtKey;

	dc.GetTextMetrics(&txtKey);

	HideCaret();	// 입력 동안 캐럿 숨기기

	// 백스페이스 입력 시
	if (nChar == VK_BACK) {
		if (!pDoc->strDB[pDoc->pline].IsEmpty()) {
			pDoc->strDB[pDoc->pline] = pDoc->strDB[pDoc->pline].Left(pDoc->strDB[pDoc->pline].GetLength() - 1);
			dc.TextOut(0, pntCur.y, pDoc->strDB[pDoc->pline] + _T("  "));
		}
	}

	// 엔터키 입력 시
	else if (nChar == VK_RETURN) {
		pDoc->pline++;
	}

	// 윗방향키 입력 시
	else if (nChar == VK_UP) {
		pDoc->pline--;
	}

	// 일반 문자 입력 시
	else {
		// 입력 문자 strDB에 추가
		pDoc->strDB[pDoc->pline] += (TCHAR)nChar;
		// 화면에 출력
		dc.TextOut(0, (int)pDoc->pline * txtKey.tmHeight, pDoc->strDB[pDoc->pline]);
	}

	// 현재 문자열의 끝나는 위치 계산
	CSize strSize;
	strSize = dc.GetTextExtent(pDoc->strDB[pDoc->pline]);

	// 캐럿 위치 계산
	pntCur.x = strSize.cx;
	pntCur.y = int(pDoc->pline) * txtKey.tmHeight;

	// 캐럿 위치 지정
	SetCaretPos(pntCur);
	
	// 캐럿 보여주기
	ShowCaret();

	CView::OnChar(nChar, nRepCnt, nFlags);
}


void CEditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 맨 윗줄일 경우
	if (pDoc->pline == 0) return;

	// 윗 방향키일 경우
	if (nChar == VK_UP) OnChar(nChar, nRepCnt, nFlags);


	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CEditorView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.
	TEXTMETRIC txtKey;

	dc.GetTextMetrics(&txtKey);

	for (int i = 0; i <= pDoc->pline; i++)
		dc.TextOut(0, (int)i * txtKey.tmHeight, pDoc->strDB[i]);
}
