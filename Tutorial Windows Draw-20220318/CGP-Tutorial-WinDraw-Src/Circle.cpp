#include <windows.h>
#include "Circle.h"

CCircle::CCircle(float x, float y, float r)
{
	m_x = x;
	m_y = y;
	m_r = r;

	rect.bottom = (LONG)(m_y - m_r);
	rect.left = (LONG)m_x;// -_radius;
	rect.right = (LONG)(m_x + m_r);
	rect.top = (LONG)(m_y + m_r);
}

CCircle::~CCircle()
{

}

void CCircle::Draw(HDC pDC)
{	
//	cout << "[CIRCLE] position = ( " << _x << " , " << _y << " ), radius = " << _radius << endl;

//	TCHAR st[1024]; // A Win32 character string that can be used to describe ANSI, DBCS, or Unicode strings.
//	wsprintf(st, TEXT("[CIRCLE]position=(%d,%d),radius=%d"), (int)m_x, (int)m_y, (int)m_r);
//	DrawText(pDC, st, -1, &rect, DT_LEFT | DT_VCENTER | DT_NOCLIP);

	Ellipse(pDC, (int)(m_x - m_r), (int)(m_y - m_r), (int)(m_x + m_r), (int)(m_y + m_r));
}


