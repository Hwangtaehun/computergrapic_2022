#pragma once

#include "Shape.h"

class CCircle :	public CShape
{
	private:
		float m_r;

	public:
		CCircle(float x, float y, float r);
		~CCircle();

		void Draw(HDC pDC);
};
