#pragma once

#include <windows.h>

class CShape
{
	protected:
		float m_x, m_y;
		RECT rect;

	public:
		CShape();
		CShape(float x, float y);
		~CShape();
		
		virtual void Draw(HDC pDC);
};
