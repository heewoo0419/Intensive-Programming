
// MyPaintView.cpp: CMyPaintView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MyPaint.h"
#endif

#include "MyPaintDoc.h"
#include "MyPaintView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyPaintView

IMPLEMENT_DYNCREATE(CMyPaintView, CView)

BEGIN_MESSAGE_MAP(CMyPaintView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)

	ON_COMMAND_RANGE(ID_FREELINE, ID_ELLIPSE, OnChangeTool)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FREELINE, ID_ELLIPSE, OnUpdateChangeTool)

	ON_COMMAND(ID_LINECOLOR, &CMyPaintView::OnLinecolor)
	ON_COMMAND(ID_FILLCOLOR, &CMyPaintView::OnFillcolor)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CMyPaintView 생성/소멸

CMyPaintView::CMyPaintView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMyPaintView::~CMyPaintView()
{
}

BOOL CMyPaintView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMyPaintView 그리기

void CMyPaintView::OnDraw(CDC* /*pDC*/)
{
	CMyPaintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMyPaintView 인쇄

BOOL CMyPaintView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMyPaintView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMyPaintView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CMyPaintView 진단

#ifdef _DEBUG
void CMyPaintView::AssertValid() const
{
	CView::AssertValid();
}

void CMyPaintView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyPaintDoc* CMyPaintView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyPaintDoc)));
	return (CMyPaintDoc*)m_pDocument;
}
#endif //_DEBUG

// 툴바 항목이 선택될 때 호출
void CMyPaintView::OnChangeTool(UINT wParam) {
	m_nType = GetCurrentMessage()->wParam;	// 클릭한 툴바 항목의 아이디를 리턴
}

// 현재 선택한 툴바항목과 일치여부에 따라 체크 표시
void CMyPaintView::OnUpdateChangeTool(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_nType == (int)pCmdUI->m_nID);
}


// CMyPaintView 메시지 처리기


void CMyPaintView::OnLinecolor() {
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK) {
		m_colLine = dlg.GetColor();
		m_colLineXor = (RGB(GetRValue(m_colLine) ^ 255, GetGValue(m_colLine) ^ 255, GetBValue(m_colLine) ^ 255));
	}
}


void CMyPaintView::OnFillcolor() {
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK) {
		m_colFill = dlg.GetColor();
	}
}


void CMyPaintView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pntOld = point;
	m_pntCur = point;

	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}


void CMyPaintView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nFlags & MK_LBUTTON) {
		CClientDC dc(this);

		// 펜과 브러시 속성을 변경
		CPen pen, * pOldPen;
		CBrush brush, * pOldBrush;

		// 자유 곡선 그리기
		if (m_nType == ID_FREELINE) {
			// 선택한 선색으로 펜을 생성
			pen.CreatePen(PS_SOLID, 1, m_colLine);
			// 생성한 펜을 선택
			pOldPen = (CPen*)dc.SelectObject(&pen);

			// 직전 좌표에서 현재좌표까지 선을 그려준다
			dc.MoveTo(m_pntOld.x, m_pntOld.y);
			dc.LineTo(point.x, point.y);

			// 현재 좌표 저장
			m_pntOld = point;
			dc.SelectObject(pOldPen);
		}


		// 직선 그리기
		else if (m_nType == ID_LINE) {
			// Xor 된 색으로 펜 생성
			pen.CreatePen(PS_SOLID, 1, m_colLineXor);

			// NULL 브러쉬(투명 브러쉬) 선택
			dc.SelectObject(GetStockObject(NULL_BRUSH));

			// 그리기 모드를 XOR 모드로 변경
			dc.SetROP2(R2_XORPEN);

			// 선색으로 펜 생성
			pOldPen = (CPen*)dc.SelectObject(&pen);

			// 이전 그림 지우기
			dc.MoveTo(m_pntOld.x, m_pntOld.y);
			dc.LineTo(m_pntCur.x, m_pntCur.y);

			// 새 그림 그리기
			dc.MoveTo(m_pntOld.x, m_pntOld.y);
			dc.LineTo(point.x, point.y);

			dc.SelectObject(pOldPen);
		}


		// 직사각형 그리기
		else if (m_nType == ID_RECTANGLE) {
			// Xor 된 색으로 펜 생성
			pen.CreatePen(PS_SOLID, 1, m_colLineXor);

			// NULL 브러쉬(투명 브러쉬) 선택
			dc.SelectObject(GetStockObject(NULL_BRUSH));

			// 그리기 모드를 XOR 모드로 변경
			dc.SetROP2(R2_XORPEN);

			// 선색으로 펜 생성
			pOldPen = (CPen*)dc.SelectObject(&pen);

			// 이전 그림 지우기
			dc.Rectangle(m_pntOld.x, m_pntOld.y, m_pntCur.x, m_pntCur.y);

			// 새 그림 그리기
			dc.Rectangle(m_pntOld.x, m_pntOld.y, point.x, point.y);

			dc.SelectObject(pOldPen);
		}

		// 타원 그리기
		else if (m_nType == ID_ELLIPSE) {
			// Xor 된 색으로 펜 생성
			pen.CreatePen(PS_SOLID, 1, m_colLineXor);

			// NULL 브러쉬(투명 브러쉬) 선택
			dc.SelectObject(GetStockObject(NULL_BRUSH));

			// 그리기 모드를 XOR 모드로 변경
			dc.SetROP2(R2_XORPEN);

			// 선색으로 펜 생성
			pOldPen = (CPen*)dc.SelectObject(&pen);

			// 이전 그림 지우기
			dc.Ellipse(m_pntOld.x, m_pntOld.y, m_pntCur.x, m_pntCur.y);

			// 새 그림 그리기
			dc.Ellipse(m_pntOld.x, m_pntOld.y, point.x, point.y);

			dc.SelectObject(pOldPen);
		}

		m_pntCur = point;
	}
	CView::OnMouseMove(nFlags, point);
}


void CMyPaintView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();

	CClientDC dc(this);

	CBrush brush, * pOldBrush;


	// 직사각형 그리기
	if (m_nType == ID_RECTANGLE) {
		brush.CreateSolidBrush(m_colFill);
		// 생성된 브러쉬 선택
		pOldBrush = (CBrush*)dc.SelectObject(brush);

		// 브러쉬로 사각형 그리기
		dc.Rectangle(m_pntOld.x, m_pntOld.y, point.x, point.y);

	}

	// 타원 그리기
	else if (m_nType == ID_ELLIPSE) {
		brush.CreateSolidBrush(m_colFill);
		// 생성된 브러쉬 선택
		pOldBrush = (CBrush*)dc.SelectObject(brush);

		// 브러쉬로 타원 그리기
		dc.Ellipse(m_pntOld.x, m_pntOld.y, point.x, point.y);

	}
	CView::OnLButtonUp(nFlags, point);
}
