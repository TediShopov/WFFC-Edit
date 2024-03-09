// MFCTransformView.cpp : implementation file
//

#include "resource.h"
#include "MFCTransformView.h"

// MFCTransformView

IMPLEMENT_DYNCREATE(MFCTransformView, CFormView)

MFCTransformView::MFCTransformView()
	: CFormView(IDD_FORMVIEW)
{
}

MFCTransformView::~MFCTransformView()
{
}

void MFCTransformView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MFCTransformView, CFormView)
END_MESSAGE_MAP()

// MFCTransformView diagnostics

#ifdef _DEBUG
void MFCTransformView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void MFCTransformView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// MFCTransformView message handlers