#pragma once
#include <cmath>
class CCoordinate
{
public:
	void DrawCoordinate(CDC* pDC)
	{
		// border
		CPen penBorder;
		penBorder.CreatePen(PS_SOLID, 2, RGB(180, 180, 180));
		CPen* pOldPen = pDC->SelectObject(&penBorder);
		pDC->Rectangle(&m_borderRc);

		// grid settings
		double worldPerPixel = m_fScale; // world units per screen pixel
		const int targetPixels = 80; // desired pixels between grid lines
		double targetWorld = targetPixels * worldPerPixel;

		double base = pow(10.0, floor(log10(targetWorld)));
		double candidates[4] = { base, base * 2.0, base * 5.0, base * 10.0 };
		double gridWorld = candidates[0];
		double pixelSpacing = gridWorld / worldPerPixel;
		double bestDiff = fabs(pixelSpacing - targetPixels);
		for (int i = 1; i < 4; ++i)
		{
			double px = candidates[i] / worldPerPixel;
			double diff = fabs(px - targetPixels);
			if (diff < bestDiff)
			{
				bestDiff = diff;
				gridWorld = candidates[i];
				pixelSpacing = px;
			}
		}

		// light grid pen
		CPen penGrid;
		penGrid.CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
		pDC->SelectObject(&penGrid);

		// compute world bounds visible
		double leftWorld = (m_borderRc.left - m_zeroPt.x) * worldPerPixel;
		double rightWorld = (m_borderRc.right - m_zeroPt.x) * worldPerPixel;
		double topWorld = (m_zeroPt.y - m_borderRc.top) * worldPerPixel;
		double bottomWorld = (m_zeroPt.y - m_borderRc.bottom) * worldPerPixel;

		// vertical grid lines
		double startX = floor(leftWorld / gridWorld) * gridWorld;
		for (double wx = startX; wx <= rightWorld + 1e-9; wx += gridWorld)
		{
			int sx = (int)round(m_zeroPt.x + wx / worldPerPixel);
			// skip drawing over border area outside
			if (sx < m_borderRc.left || sx > m_borderRc.right)
				continue;
			pDC->MoveTo(sx, m_borderRc.top);
			pDC->LineTo(sx, m_borderRc.bottom);
		}

		// horizontal grid lines
		double startY = floor(bottomWorld / gridWorld) * gridWorld;
		for (double wy = startY; wy <= topWorld + 1e-9; wy += gridWorld)
		{
			int sy = (int)round(m_zeroPt.y - wy / worldPerPixel);
			if (sy < m_borderRc.top || sy > m_borderRc.bottom)
				continue;
			pDC->MoveTo(m_borderRc.left, sy);
			pDC->LineTo(m_borderRc.right, sy);
		}

		// draw axes on top (darker)
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

		// draw labels on axes where grid lines cross axes
		// determine decimal precision
		int decimals = 0;
		if (gridWorld < 1.0)
			decimals = (int)ceil(-log10(gridWorld));
		if (decimals > 6)
			decimals = 6;

		// vertical labels (x values) along X axis
		if (m_zeroPt.y >= m_borderRc.top && m_zeroPt.y <= m_borderRc.bottom)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			for (double wx = startX; wx <= rightWorld + 1e-9; wx += gridWorld)
			{
				int sx = (int)round(m_zeroPt.x + wx / worldPerPixel);
				if (sx < m_borderRc.left || sx > m_borderRc.right)
					continue;
				CString label;
				wchar_t buf[64];
				swprintf_s(buf, L"%.*f", decimals, wx);
				label = buf;
				// place label slightly below axis
				pDC->TextOutW(sx + 2, m_zeroPt.y + 2, label);
			}
		}

		// horizontal labels (y values) along Y axis
		if (m_zeroPt.x >= m_borderRc.left && m_zeroPt.x <= m_borderRc.right)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			for (double wy = startY; wy <= topWorld + 1e-9; wy += gridWorld)
			{
				int sy = (int)round(m_zeroPt.y - wy / worldPerPixel);
				if (sy < m_borderRc.top || sy > m_borderRc.bottom)
					continue;
				CString label;
				wchar_t buf[64];
				swprintf_s(buf, L"%.*f", decimals, wy);
				label = buf;
				// place label slightly right of Y axis
				pDC->TextOutW(m_zeroPt.x + 2, sy + 2, label);
			}
		}

		// restore pen
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

