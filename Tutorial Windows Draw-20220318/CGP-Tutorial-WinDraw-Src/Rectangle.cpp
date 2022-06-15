#include "Rectangle.h"

CRectangle::CRectangle(float x, float y, float w, float h)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
	rect.bottom = (LONG)(m_y - m_h / 2);
	rect.left = (LONG)(m_x);// -_width / 2;
	rect.right = (LONG)(m_x + m_w / 2);
	rect.top = (LONG)(m_y + m_h / 2);
}

CRectangle::~CRectangle()
{

}

void CRectangle::Draw(HDC pDC)
{
//	cout << "[RECTANGLE] position = ( " << _x << " , " << _y << " ), size = ( " << _width << " , " << _height << " )" << endl;

//	TCHAR st[1024];
//	wsprintf(st, TEXT("[RECTANGLE]position=(%d,%d),size=(%d,%d)"), (int)m_x, (int)m_y, (int)m_w,(int)m_h);
//	DrawText(pDC, st, -1, &rect, DT_LEFT | DT_VCENTER | DT_NOCLIP);

	Rectangle(pDC, (int)m_x, (int)m_y, (int)m_w, (int)m_h);
}


