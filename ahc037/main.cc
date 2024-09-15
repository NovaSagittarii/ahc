#include <bits/stdc++.h>

int32_t main() {
  int N;
  std::cin >> N;
  std::vector<std::array<int, 2>> a(N);
  for (auto &[x, y] : a) std::cin >> x >> y;

  std::vector<std::array<int, 4>> ans;
  // N = 1000
  // with merging that means 2000 in total
  // i have 3000 to spare
  int di = (int)1e9 / 63;
  int dj = (int)1e9 / 62;
  for (int i = 0; i < 63; ++i) {
    ans.push_back({0, i * di, 0, (i + 1) * di});
    for (int j = 0; j < 62; ++j) {
      ans.push_back({j * dj, i * di, (j + 1) * dj, i * di});
    }
  }

  for (auto [x, y] : a) {
    ans.push_back({(x / dj) * dj, (y / di) * di, x, y});
  }

  std::cout << ans.size() << "\n";
  for (auto &row : ans) {
    for (auto x : row) std::cout << x << " ";
    std::cout << "\n";
  }

  return 0;
}