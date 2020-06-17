#pragma once

#include <string>
#include <vector>

class Solver
{
	int n; // Количество вершин после разбиения
	std::vector<std::vector<int>> f; // Поток
	std::vector<std::vector<int>> c; // Пропускная способность
	std::vector<std::vector<int>> h; // Смежные вершины
	std::vector<int> origIdxToSource; // Набор истоков, которые соответствуют исходным точкам
	std::vector<int> origIdxToSink; // Набор стоков, которые соответствую исходным точкам
	std::vector<int> idxToOrigIdx; // Обратное преобразование
	
	// Для поиска в ширину:
	std::vector<bool> used;
	std::vector<int> prev;
public:
	int nOrig; // Количество исходных вершин
	std::vector<std::pair<double, double>> positions; // Позиции вершин
	std::vector<std::vector<int>> hOrig; // Исходные ребра (неориентированные)
	int source = -1; // Исток
	int sink = -1; // Сток

	Solver();
	~Solver();

	bool Load(const std::wstring& fileName);
	int GetAnswer(int vS, int vT);
	void Clear();

private:
	void PrepareGraph();
	void Dfs(int v);
};

