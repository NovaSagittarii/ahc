#include <bits/stdc++.h>

// template from kactl
using namespace std;

#define rep(i, a, b) for (int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
typedef long long ll;
typedef pair<int, int> pii;
typedef vector<int> vi;

struct UF {
  vi e;
  UF(int n) : e(n, -1) {}
  bool sameSet(int a, int b) { return find(a) == find(b); }
  int size(int x) { return -e[find(x)]; }
  int find(int x) { return e[x] < 0 ? x : e[x] = find(e[x]); }
  bool join(int a, int b) {
    a = find(a), b = find(b);
    if (a == b) return false;
    if (e[a] > e[b]) swap(a, b);
    e[a] += e[b];
    e[b] = a;
    return true;
  }
};

int32_t main() {
  int N;
  std::cin >> N;
  std::vector<std::array<int, 2>> a(N);
  for (auto &[x, y] : a) std::cin >> x >> y;

  std::vector<std::array<int, 4>> edges;  // <weight, i, j, type>
  for (int i = 0; i < N; ++i) {
    for (int j = i + 1; j < N; ++j) {
      int ai = i;
      int aj = j;
      auto [x1, y1] = a[i];
      auto [x2, y2] = a[j];
      if (x1 > x2 || (x1 == x2 && y1 > y2)) {
        // would like x1 < x2 and y1 < y2
        std::swap(x1, x2);
        std::swap(y1, y2);
        std::swap(ai, aj);
      }
      if (y1 <= y2) {
        // just go directly from P1 to P2
        edges.push_back({y2 - y1 + x2 - x1, ai, aj, 0});
      } else if (y1 > y2) {  // oh no its a funny case
        edges.push_back({std::abs(y1-y2) + std::abs(x2-x1), ai, aj, 1});
      }
    }
  }
  std::sort(edges.begin(), edges.end());
  
  UF dsu(N);
  std::vector<std::vector<int>> req(N);
  for (auto [w, i, j, insert] : edges) {
    if (dsu.sameSet(i, j)) continue;
    dsu.join(i, j);
    if (!insert) {
      req[j].push_back(i);
    } else {
      int k = req.size();
      auto [x1, y1] = a[i];
      auto [x2, y2] = a[j];
      a.push_back({std::min(x1, x2), std::min(y1, y2)});
      // req.push_back({i, j}); // o this is where things get weird
      req[i].push_back(k);
      req[j].push_back(k);
    }
  }

  std::vector<std::array<int, 4>> ans;
  std::vector<int> vis(req.size());
  auto dfs = [&](int u, int prev, auto&& dfs) -> void {
    if (vis[u]) return;
    vis[u] = 1;
    for (auto v : req[u]) dfs(v, u, dfs);
    auto [px, py] = a[prev];
    auto [nx, ny] = a[u];
    ans.push_back({px, py, nx, ny});
  };
  for (int i = 0; i < req.size(); ++i) {
    if (req[i].empty()) dfs(i, -1, dfs);
  }
  
  std::cout << ans.size() << "\n";
  for (auto &row : ans) {
    for (auto x : row) std::cout << x << " ";
    std::cout << "\n";
  }

  return 0;
}