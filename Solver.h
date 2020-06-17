#pragma once

#include <string>
#include <vector>

class Solver
{
	int n; // ���������� ������ ����� ���������
	std::vector<std::vector<int>> f; // �����
	std::vector<std::vector<int>> c; // ���������� �����������
	std::vector<std::vector<int>> h; // ������� �������
	std::vector<int> origIdxToSource; // ����� �������, ������� ������������� �������� ������
	std::vector<int> origIdxToSink; // ����� ������, ������� ������������ �������� ������
	std::vector<int> idxToOrigIdx; // �������� ��������������
	
	// ��� ������ � ������:
	std::vector<bool> used;
	std::vector<int> prev;
public:
	int nOrig; // ���������� �������� ������
	std::vector<std::pair<double, double>> positions; // ������� ������
	std::vector<std::vector<int>> hOrig; // �������� ����� (�����������������)
	int source = -1; // �����
	int sink = -1; // ����

	Solver();
	~Solver();

	bool Load(const std::wstring& fileName);
	int GetAnswer(int vS, int vT);
	void Clear();

private:
	void PrepareGraph();
	void Dfs(int v);
};

