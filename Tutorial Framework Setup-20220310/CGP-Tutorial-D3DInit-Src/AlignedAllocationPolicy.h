#pragma once

// for warning C4316
// �� ���� __declspec(align(byte))�� ���ĵǾ�� �ϴ� �����͸� member�� �����ų�, 
// �Ҵ��� �� ����� �����. For example, using XMMATRIX as a class member.(���� ����)
template<size_t T>			// �����޸� �Ҵ� -> �ڷ����� ������� ����ϴ� ���ø�
class AlignedAllocationPolicy
{
public:
	static void* operator new(size_t size)	//�ڷ����� ���� �Ҵ����
	{
		return _aligned_malloc(size, T);
	}

	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};

// �޸� ���� �Լ�, �����ڵ�, D3D�ڵ� ������ ���ø� ���, 128��Ʈ�� ����ϱ� ���ؼ� ���