#include <bits/stdc++.h>

const int kSubdivisions = 5;

class Solution {
 public:
  Solution(int N, std::vector<std::array<int, 2>> a) : N(N), a(a) {}
  std::array<int, 2> ComputeGridSpacing(int n, int m) {
    int dx = (int)1e9 / n;
    int dy = (int)1e9 / m;
    return {dx, dy};
  }

  /**
   * @brief Generates the mesh used by the existing points.
   *
   * @param n
   * @param m
   * @return std::set<std::array<int, 3>>
   */
  std::set<std::array<int, 3>> ComputeGrid(int n, int m) {
    const auto [dx, dy] = ComputeGridSpacing(n, m);
    std::set<std::array<int, 3>> used;
    for (auto [x, y] : a) {
      int i = x / dx;
      int j = y / dy;
      used.insert({i + j * N, i, j});
      if (std::min(x, y) < 1e8) {
        if (x > y) {
          used.insert({i, i, 0});
        } else {
          used.insert({j * N, 0, j});
        }
      }
    }
    return used;
  }

  /**
   * @brief Adds in extra points whenever you would've had two long vertical
   * lines parallel to each other.
   *
   * @param mesh list of points you'll connect later
   */
  void PatchMesh(int n, int m, std::set<std::array<int, 3>> &mesh) {
    const auto [dx, dy] = ComputeGridSpacing(n, m);
    std::set<std::array<int, 2>> exist;
    exist.insert({0, 0});
    auto used = ComputeGrid(n, m);
    std::vector<std::array<int, 2>> additions;
    for (auto [_, i, j] : mesh) {
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
      int mi2 = (i + pi) / 2;
      auto Insert = [&](int pi, int pj, int i, int j) -> void {
        if (exist.count({i, j}) == 0) {
          exist.insert({i, j});
        }
      };

      std::vector<int> mi;
      for (int k = 0; k < kSubdivisions; ++k) {
        mi.push_back(pi + (i - pi) / kSubdivisions * k);
      }
      mi.push_back(i);
      if (j) {
        if (j - pj > 3 && i) {
          int dw = 4;
          for (int w = pj; w < j; w += dw+(std::rand()&1)) {
            additions.push_back({pi, w});
            dw ^= 1;
          }
        } else {
          Insert(pi, pj, pi, j);
        }
        for (int i = 1; i < mi.size(); ++i) {
          Insert(mi[i - 1], j, mi[i], j);
        }
      } else {  // mesh x-axis
        Insert(pi, pj, i, j);
      }
    }
    for (auto [i, j] : additions) mesh.insert({i + j * N, i, j});
  }

  std::vector<std::array<int, 4>> Solve(int n, int m) {
    const auto [dx, dy] = ComputeGridSpacing(n, m);
    std::vector<std::array<int, 4>> ans;
    std::set<std::array<int, 2>> exist;
    exist.insert({0, 0});
    auto used = ComputeGrid(n, m);
    PatchMesh(n, m, used);
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
      int mi2 = (i + pi) / 2;
      auto Insert = [&](int pi, int pj, int i, int j) -> void {
        if (exist.count({i, j}) == 0) {
          ans.push_back({pi * dx, pj * dy, i * dx, j * dy});
          exist.insert({i, j});
        }
      };

      std::vector<int> mi;
      for (int k = 0; k < kSubdivisions; ++k) {
        mi.push_back(pi + (i - pi) / kSubdivisions * k);
      }
      mi.push_back(i);
      if (j) {
        Insert(pi, pj, pi, j);
        for (int i = 1; i < mi.size(); ++i) {
          Insert(mi[i - 1], j, mi[i], j);
        }
      } else {  // mesh x-axis
        Insert(pi, pj, i, j);
      }
    }

    for (auto [x, y] : a) {
      ans.push_back({(x / dx) * dx, (y / dy) * dy, x, y});
    }

    return Cleanup(ans);
  }

  /**
   * @brief Reduces unnecessary nodes.
   *
   * @param edges
   */
  std::vector<std::array<int, 4>> Cleanup(
      const std::vector<std::array<int, 4>> &edges) {
    std::vector<std::vector<int>> adj;
    std::vector<std::array<int, 2>> coords;
    std::map<std::array<int, 2>, int> to_index;
    auto CreateNode = [&](int x, int y) -> std::array<int, 2> {
      std::array<int, 2> p = {x, y};
      if (to_index.count(p) == 0) {
        to_index[p] = coords.size();
        coords.push_back(p);
        adj.push_back({});
      }
      return std::move(p);
    };

    for (auto [ux, uy, vx, vy] : edges) {
      auto u = CreateNode(ux, uy);
      auto v = CreateNode(vx, vy);
      adj[to_index[u]].push_back(to_index[v]);
    }

    // for (int i = 0; i < adj.size(); ++i) {
    //   std::cout << i << " (";
    //   for (auto x : coords[i]) std::cout << x << " ";
    //   std::cout << ") ";
    //   for (auto x : adj[i]) std::cout << x << " ";
    //   std::cout << "\n";
    // }

    for (int u = 0; u < adj.size(); ++u) {
      // want to transform
      // u---v---w    =>    u----w
      std::vector<int> append;
      for (int& v : adj[u]) {
        if (adj[v].size() == 1) {
          append.push_back(adj[v].front());
          adj[v].clear();
          v = -1;
        }
      }
      for (auto w : append) adj[u].push_back(w);
    }

    std::vector<std::array<int, 4>> output;
    std::vector<int> vis(adj.size());
    auto dfs = [&](int u, auto&& dfs) -> void {
      if (vis[u]) return;
      vis[u] = 1;
      
      auto [ux, uy] = coords[u];
      for (auto v : adj[u]) {
        if (v == -1) continue;
        auto [vx, vy] = coords[v];
        output.push_back({ux, uy, vx, vy});
        dfs(v, dfs);
      }
    };
    for (int u = 0; u < adj.size(); ++u) dfs(u, dfs);
    return output;
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

  int n = 500;
  int m = 500;

  // int best = 0;
  // for (int i = 250; i < 300; ++i) {
  //   for (int j = 250; j < 300; ++j) {
  //     int res = sol.ComputeGrid(i, j).size();
  //     if (res < 4000 && res > best) {
  //       n = i;
  //       m = j;
  //     }
  //   }
  // }
  // std::cerr << n << " " << m << std::endl;

  auto ans = sol.Solve(n, m);

  std::cout << ans.size() << "\n";
  for (auto &row : ans) {
    for (auto x : row) std::cout << x << " ";
    std::cout << "\n";
  }

  return 0;
}