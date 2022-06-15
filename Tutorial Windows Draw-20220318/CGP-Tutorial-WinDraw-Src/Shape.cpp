#include <iostream>
#include "Shape.h"

using namespace std;

CShape::CShape()
{
}

CShape::CShape(float x, float y)
{
	m_x = x;
	m_y = y;
	rect = {0, 0, 0, 0};
}

CShape::~CShape()
{

}

void CShape::Draw(HDC pDC)
{
	cout << "[SHAPE] position = ( " << m_x << " , " << m_y << " )" << endl;
}