#pragma once

// for warning C4316
// 위 경고는 __declspec(align(byte))로 정렬되어야 하는 데이터를 member로 가지거나, 
// 할당할 때 생기는 경고임. For example, using XMMATRIX as a class member.(정렬 위혐)
template<size_t T>			// 일정메모리 할당 -> 자료형에 상관없이 사용하는 템플릿
class AlignedAllocationPolicy
{
public:
	static void* operator new(size_t size)	//자료형에 따라서 할당받음
	{
		return _aligned_malloc(size, T);
	}

	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};

// 메모리 정리 함수, 무한코드, D3D코드 때문에 탬플릿 사용, 128비트를 사용하기 위해서 사용