#include "pch.h"
#include "Solver.h"
#include <fstream>

Solver::Solver()
{
}


Solver::~Solver()
{
}

void Solver::Clear()
{
	nOrig = 0;
	positions.clear();
	hOrig.clear();
	source = sink = -1;
}

bool Solver::Load(const std::wstring& fileName) {
	std::ifstream in(fileName);
	Clear();
	if (!in.good()) {
		MessageBox(NULL, _wcserror(errno), L"Error", MB_OK);
		return false;
	}
	if (!(in >> nOrig)) {
		return false;
	}
	positions.resize(nOrig);
	for (int i = 0; i < nOrig; i++) {
		double x, y;
		if (!(in >> x >> y))
			return false;
		positions[i] = std::make_pair(x, y);
	}

	hOrig.resize(nOrig);
	int k;
	if (!(in >> k))
		return false;
	for (int i = 0; i < k; i++) {
		int from, to;
		if (!(in >> from >> to))
			return false;
		hOrig[from].push_back(to);
	}

	return true;
}

void Solver::PrepareGraph()
{
	n = nOrig * 2;

	f.clear();
	c.clear();
	h.clear();
	origIdxToSource.clear();
	origIdxToSink.clear();
	idxToOrigIdx.clear();

	f.resize(n, std::vector<int>(n, 0));
	c.resize(n, std::vector<int>(n, 0));
	h.resize(n);
	origIdxToSource.resize(nOrig);
	origIdxToSink.resize(nOrig);
	idxToOrigIdx.resize(n);

	for (int i = 0; i < nOrig; i++) {
		int source = i * 2;
		int sink = i * 2 + 1;
		h[sink].push_back(source);
		h[source].push_back(sink);
		f[sink][source] = 1;
		f[source][sink] = 0;
		origIdxToSource[i] = source;
		origIdxToSink[i] = sink;
	}
	for (int v1 = 0; v1 < nOrig; v1++) {
		for (int i = 0; i < hOrig[v1].size(); i++) {
			int v2 = hOrig[v1][i];

			int source1 = origIdxToSource[v1];
			int source2 = origIdxToSource[v2];
			int sink1 = origIdxToSink[v1];
			int sink2 = origIdxToSink[v2];

			f[source1][sink2] = f[source2][sink1] = 1;
			h[source1].push_back(sink2);
			h[sink2].push_back(source1);
			h[source2].push_back(sink1);
			h[sink1].push_back(source2);
		}
	}
}

int Solver::GetAnswer(int vS, int vT) {
	source = vS;
	sink = vT;

	for (int i = 0; i < hOrig[vS].size(); i++)
		if (hOrig[vS][i] == vT)
			return -1;
	for (int i = 0; i < hOrig[vT].size(); i++)
		if (hOrig[vT][i] == vS)
			return -1;

	PrepareGraph();
	
	int s = origIdxToSource[vS];
	int t = origIdxToSink[vT];

	used.resize(n);
	prev.resize(n);

	do {
		std::fill(used.begin(), used.end(), false);
		std::fill(prev.begin(), prev.end(), -1);

		Dfs(s);
		if (used[t]) {
			int v = t;
			while (v != s) {
				int from = prev[v];
				int to = v;
				c[from][to]++;
				c[to][from]--;
				v = prev[v];
			}
		}
		else {
			break;
		}
	} while (true);

	int ans = 0;
	for (int i = 0; i < h[s].size(); i++) {
		int to = h[s][i];
		if (c[s][to] > 0)
			ans += c[s][to];
	}

	return ans;
}

void Solver::Dfs(int v) {
	used[v] = true;
	for (int i = 0; i < h[v].size(); i++) {
		int to = h[v][i];
		if (!used[to] && (f[v][to] - c[v][to]) > 0) {
			prev[to] = v;
			Dfs(to);
		}
	}
}