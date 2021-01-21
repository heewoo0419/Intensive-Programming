
// MyPaintView.h: CMyPaintView 클래스의 인터페이스
//

#pragma once


class CMyPaintView : public CView
{
protected: // serialization에서만 만들어집니다.
	CMyPaintView() noexcept;
	DECLARE_DYNCREATE(CMyPaintView)

// 특성입니다.
public:
	CMyPaintDoc* GetDocument() const;

	// 선 색상
	COLORREF m_colLine = RGB(0, 0, 0);	

	// 채우기 색상
	COLORREF m_colFill = RGB(0, 0, 0);	

	// XOR 선 색상
	COLORREF m_colLineXor = (RGB(GetRValue(m_colLine) ^ 255, GetGValue(m_colLine) ^ 255, GetBValue(m_colLine) ^ 255));		
	// 그리기 타입
	int m_nType;		
	// 그리기 좌표
	CPoint m_pntOld, m_pntCur;					

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMyPaintView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnChangeTool(UINT wParam);
	afx_msg void OnUpdateChangeTool(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLinecolor();
	afx_msg void OnFillcolor();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MyPaintView.cpp의 디버그 버전
inline CMyPaintDoc* CMyPaintView::GetDocument() const
   { return reinterpret_cast<CMyPaintDoc*>(m_pDocument); }
#endif

