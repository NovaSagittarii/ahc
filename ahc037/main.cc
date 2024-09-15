#include <bits/stdc++.h>

class Solution {
 public:
  Solution(int N, std::vector<std::array<int, 2>> a) : N(N), a(a) {}
  std::array<int, 2> ComputeGridSpacing(int n, int m) {
    int dx = (int)1e9 / n;
    int dy = (int)1e9 / m;
    return {dx, dy};
  }

  std::set<std::array<int, 3>> ComputeGrid(int n, int m) {
    const auto [dx, dy] = ComputeGridSpacing(n, m);
    std::set<std::array<int, 3>> used;
    for (auto [x, y] : a) {
      int i = x / dx;
      int j = y / dy;
      used.insert({i + j * N, i, j});
    }
    return used;
  }

  std::vector<std::array<int, 4>> Solve(int n, int m) {
    const auto [dx, dy] = ComputeGridSpacing(n, m);
    std::vector<std::array<int, 4>> ans;
    std::set<std::array<int, 2>> exist;
    exist.insert({0, 0});
    auto used = ComputeGrid(n, m);
    for (auto [_, i, j] : used) {
      std::array<int, 2> prev;
      int best = 1e9;
      for (auto [i2, j2] : exist) {
        if (i2 <= i && j2 <= j) {
          int score = i - i2 + j - j2;
          if (score < best) {
            best = score;
            prev = {i2, j2};
          }
        }
      }
      auto [pi, pj] = prev;
      ans.push_back({pi * dx, pj * dy, i * dx, j * dy});
      exist.insert({i, j});
    }

    for (auto [x, y] : a) {
      ans.push_back({(x / dx) * dx, (y / dy) * dy, x, y});
    }

    return ans;
  }

 private:
  int N;
  std::vector<std::array<int, 2>> a;
};

int32_t main() {
  int N;
  std::cin >> N;
  std::vector<std::array<int, 2>> a(N);
  for (auto &[x, y] : a) std::cin >> x >> y;

  Solution sol(N, a);

  // N = 1000
  // with merging that means 2000 in total
  // i have 3000 to spare
  int n = 63;
  int m = 62;

  auto ans = sol.Solve(n, m);

  std::cout << ans.size() << "\n";
  for (auto &row : ans) {
    for (auto x : row) std::cout << x << " ";
    std::cout << "\n";
  }

  return 0;
}