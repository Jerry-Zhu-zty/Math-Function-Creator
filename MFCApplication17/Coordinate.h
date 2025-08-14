#pragma once
class CCoordinate
{
public:
	void DrawCoordinate(CDC* pDC)
	{
		CPen newPen;
		CString stmp;
		newPen.CreatePen(1, 2, RGB(255, 0, 0));
		pDC->SelectObject(&newPen);
		pDC->Rectangle(&m_borderRc);

		//stmp.Format(L"%f %f", m_borderRc.left * m_fScale, m_zeroPt.y * m_fScale);
		//pDC->TextOutW(m_borderRc.left, m_zeroPt.y, stmp);
		//int nNegative = (m_borderRc.left - m_zeroPt.x) * m_fScale < 0 ? -1 : 1;
		//for (float i = (int)(
		//	((m_borderRc.left - m_zeroPt.x)* m_fScale)
		//	/ pow(10, (int)log10(abs((m_borderRc.left - m_zeroPt.x))))) * 
		//	pow(10, (int)log10(abs((m_borderRc.left - m_zeroPt.x) * m_fScale)));
		//i < (m_borderRc.right - m_zeroPt.x) * m_fScale; 
		//i += pow(10, (int)log10(abs((m_borderRc.left - m_zeroPt.x) * m_fScale))))
		//{
		//	stmp.Format(L"%f", i);
		//	pDC->TextOutW(i / m_fScale + m_zeroPt.x, m_zeroPt.y, stmp);
		//	//pDC->MoveTo(i / m_fScale + m_zeroPt.x, m_borderRc.top);
		//	//pDC->LineTo(i / m_fScale + m_zeroPt.x, m_borderRc.bottom);
		//}
		pDC->MoveTo(m_borderRc.left, m_zeroPt.y);
		pDC->LineTo(m_borderRc.right, m_zeroPt.y);

		pDC->MoveTo(m_zeroPt.x, m_borderRc.top);
		pDC->LineTo(m_zeroPt.x, m_borderRc.bottom);

	}
	void SetZeroPt(int x, int y)
	{
		m_zeroPt.SetPoint(x, y);
	}
	CPoint GetZeroPt()
	{
		return m_zeroPt;
	}
	void SetBorder(CRect rc)
	{
		m_borderRc = rc;
	}
	void SetScale(float fScale)
	{
		m_fScale = fScale;
	}
	float GetScale()
	{
		return m_fScale;
	}
	CRect GetBorder()
	{
		return m_borderRc;
	}
protected:
	CPoint m_zeroPt;
	CRect m_borderRc;
	float m_fScale = 1;
};

