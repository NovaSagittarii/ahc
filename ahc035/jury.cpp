#include <stdlib.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#define AHC035_JURY_MODE_  // so no duplicated main
// i don't know how to do I/O via stdin/stdout from another program

#include "main.cpp"

typedef std::vector<int> vi;
typedef std::vector<vi> vvi;
typedef std::vector<std::string> vstr;
typedef std::vector<vstr> vvstr;

int32_t main() {
  int n;  // grid width & height
  int m;  // characteristics
  int t;  // iterations
  std::cin >> n >> m >> t;
  int seed_count = 2 * n * (n - 1);

  vvi a(seed_count, vi(m));  // seeds
  for (auto &seed : a) {
    for (auto &x : seed) std::cin >> x;
  }

  std::stringstream in, out;
  in << n << " " << m << " " << t << "\n";
  auto PrintSeeds = [&]() -> void {
    for (auto &seed : a) {
      for (auto x : seed) in << x << " ";
      in << "\n";
    }
    in << std::flush;
  };

  Solution sol(in, out);
  for (int iter = 0; iter < t; ++iter) {
    std::cout << "# s00 = ";
    for (auto x : a[0]) std::cout << x << " ";
    std::cout << "\n";

    PrintSeeds();
    sol.ReadSeeds();
    sol.Breed();

    // do breeding based on how they got sown
    vvstr u(n, vstr(n - 1));  // horizontal
    vvstr v(n - 1, vstr(n));  // verticals
    for (auto &row : u)
      for (auto &x : row) std::cin >> x;
    for (auto &row : v)
      for (auto &x : row) std::cin >> x;

    vvi g(n, vi(n));
    for (auto &row : g) {
      for (auto &x : row) out >> x;
      for (auto x : row) std::cout << x << " ";
      std::cout << "\n";
    }
    std::cout << "\n";

    vvi b = a;
    int bi = 0;

    // [u] horizontals
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n - 1; ++j) {
        for (int k = 0; k < m; ++k) {
          b[bi][k] = u[i][j][k] == '0' ? a[g[i][j]][k] : a[g[i][j + 1]][k];
        }
        ++bi;
      }
    }

    // [v] verticals
    for (int i = 0; i < n - 1; ++i) {
      for (int j = 0; j < n; ++j) {
        for (int k = 0; k < m; ++k) {
          b[bi][k] = v[i][j][k] == '0' ? a[g[i][j]][k] : a[g[i + 1][j]][k];
        }
        ++bi;
      }
    }

    a = std::move(b);
  }

  double score = 0;
  std::vector<int> attr_max(m);
  int W = 0;
  for (auto &seed : a) {
    int sum = 0;
    for (int i = 0; i < m; ++i) {
      attr_max[i] = std::max(attr_max[i], seed[i]);
      sum += seed[i];
    }
    W = std::max(W, sum);
  }
  int attr_max_sum = 0;
  for (auto x : attr_max) attr_max_sum += x;
  score = std::round(1000000.0 * W / attr_max_sum);

  std::cerr << "score = " << score << std::endl;
}
