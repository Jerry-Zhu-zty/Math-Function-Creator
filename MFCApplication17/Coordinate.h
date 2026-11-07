#pragma once
#include <cmath>
class CCoordinate
{
public:
	void DrawCoordinate(CDC* pDC)
	{
		CPen penBorder;
		penBorder.CreatePen(PS_SOLID, 2, RGB(180, 180, 180));
		CPen* pOldPen = pDC->SelectObject(&penBorder);
		pDC->Rectangle(&m_borderRc);

		// grid settings
		double dWorldPerPixel = m_fScale;
		const int nTargetPixels = 80;
		double dTargetWorld = nTargetPixels * dWorldPerPixel;

		double dBase = pow(10.0, floor(log10(dTargetWorld)));
		double dCandidates[4] = { dBase, dBase * 2.0, dBase * 5.0, dBase * 10.0 };//nice number
		double dGridWorld = dCandidates[0];
		double dPixelSpacing = dGridWorld / dWorldPerPixel;
		double dBestDiff = fabs(dPixelSpacing - nTargetPixels);
		for (int i = 1; i < 4; ++i)
		{
			double px = dCandidates[i] / dWorldPerPixel;
			double diff = fabs(px - nTargetPixels);
			if (diff < dBestDiff)
			{
				dBestDiff = diff;
				dGridWorld = dCandidates[i];
				dPixelSpacing = px;
			}
		}

		// light grid
		CPen penGrid;
		penGrid.CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
		pDC->SelectObject(&penGrid);

		// world bounds visible
		double dLeftWorld = (m_borderRc.left - m_zeroPt.x) * dWorldPerPixel;
		double dRightWorld = (m_borderRc.right - m_zeroPt.x) * dWorldPerPixel;
		double dTopWorld = (m_zeroPt.y - m_borderRc.top) * dWorldPerPixel;
		double dBottomWorld = (m_zeroPt.y - m_borderRc.bottom) * dWorldPerPixel;

		// vertical grid lines
		double dStartX = floor(dLeftWorld / dGridWorld) * dGridWorld;
		for (double wx = dStartX; wx <= dRightWorld + 1e-9; wx += dGridWorld)
		{
			int sx = (int)round(m_zeroPt.x + wx / dWorldPerPixel);
			
			if (sx < m_borderRc.left || sx > m_borderRc.right)
				continue;
			pDC->MoveTo(sx, m_borderRc.top);
			pDC->LineTo(sx, m_borderRc.bottom);
		}

		// horizontal grid lines
		double dStartY = floor(dBottomWorld / dGridWorld) * dGridWorld;
		for (double wy = dStartY; wy <= dTopWorld + 1e-9; wy += dGridWorld)
		{
			int sy = (int)round(m_zeroPt.y - wy / dWorldPerPixel);
			if (sy < m_borderRc.top || sy > m_borderRc.bottom)
				continue;
			pDC->MoveTo(m_borderRc.left, sy);
			pDC->LineTo(m_borderRc.right, sy);
		}

		// draw axes
		CPen penAxis;
		penAxis.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		pDC->SelectObject(&penAxis);
		// X axis
		if (m_zeroPt.y >= m_borderRc.top && m_zeroPt.y <= m_borderRc.bottom)
		{
			pDC->MoveTo(m_borderRc.left, m_zeroPt.y);
			pDC->LineTo(m_borderRc.right, m_zeroPt.y);
		}
		// Y axis
		if (m_zeroPt.x >= m_borderRc.left && m_zeroPt.x <= m_borderRc.right)
		{
			pDC->MoveTo(m_zeroPt.x, m_borderRc.top);
			pDC->LineTo(m_zeroPt.x, m_borderRc.bottom);
		}

		// draw labels
		// decimal precision
		int decimals = 0;
		if (dGridWorld < 1.0)
			decimals = (int)ceil(-log10(dGridWorld));
		if (decimals > 6)
			decimals = 6;

		// vertical labels
		if (m_zeroPt.y >= m_borderRc.top && m_zeroPt.y <= m_borderRc.bottom)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			for (double wx = dStartX; wx <= dRightWorld + 1e-9; wx += dGridWorld)
			{
				int sx = (int)round(m_zeroPt.x + wx / dWorldPerPixel);
				if (sx < m_borderRc.left || sx > m_borderRc.right)
					continue;
				CString label;
				wchar_t buf[64];
				swprintf_s(buf, L"%.*f", decimals, wx);
				label = buf;
				pDC->TextOutW(sx + 2, m_zeroPt.y + 2, label);
			}
		}

		// horizontal labels
		if (m_zeroPt.x >= m_borderRc.left && m_zeroPt.x <= m_borderRc.right)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			for (double wy = dStartY; wy <= dTopWorld + 1e-9; wy += dGridWorld)
			{
				int sy = (int)round(m_zeroPt.y - wy / dWorldPerPixel);
				if (sy < m_borderRc.top || sy > m_borderRc.bottom)
					continue;
				CString label;
				wchar_t buf[64];
				swprintf_s(buf, L"%.*f", decimals, wy);
				label = buf;
				pDC->TextOutW(m_zeroPt.x + 2, sy + 2, label);
			}
		}

		pDC->SelectObject(pOldPen);
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

