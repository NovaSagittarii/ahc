#include <bits/stdc++.h>

typedef std::vector<int> vi;
typedef std::vector<vi> v2i;

int rnd(int l, int r) { return l + rand() % (r - l + 1); }

typedef std::vector<std::array<int16_t, 2>> offsets;
const offsets kAdj = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
const offsets kSur = {
    {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0},
    {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0},
};
// 0 1 2
// 7   3
// 6 5 4  [mod 8]

class Solver {
 public:
  // uint16_t
  typedef std::array<int, 2> pt;
  typedef std::vector<pt> vpt;
  typedef std::vector<vpt> v2pt;
  Solver(std::istream &in) {
    std::cin >> N;
    ap.assign(N, {});
    an.assign(N, {});
    for (auto &[x, y] : ap) std::cin >> x >> y;
    for (auto &[x, y] : an) std::cin >> x >> y;
  }

  void Solve(std::ostream &os) {
    int best_score = 0;
    vpt best;
    const int LAST = 1000;
    const double MAX_T = 30;
    for (int attempt = 0; attempt < 20; ++attempt) {
      // std::cerr << attempt << std::endl;
      v2i psel(1, vi(1, 0));
      // grid resolution
      for (const int n : vi{10, 20, LAST}) {
        // std::cerr << n << std::endl;
        int S = 100'000 / n;  // grid square size
        v2i g(n, vi(n));
        for (auto [x, y] : ap) g[y / S][x / S] += 9;
        for (auto [x, y] : an) g[y / S][x / S] -= 10;

        v2i sel(n, vi(n));
        const int pk = n / psel.size();
        int score = 0;
        for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
            sel[i][j] = psel[i / pk][j / pk];
            score += g[i][j] * sel[i][j];
          }
        }
        v2i g_perimeter(n, vi(n));

        int perimeter = 0;
        auto Eval = [&](double T) -> double {
          // double LT = 2.0;  // std::min(2.0, 3 * T);
          double LIMIT = 400'000;
          double e = perimeter * S / LIMIT;
          double penalty = std::pow(std::max(0.0, e - 0.8)*5, 2);
          double pscore = std::max(0, score / (n * 10));
          if (perimeter * S > LIMIT) {  // exceed limit (bad)
            return 1e9;
          }
          return penalty - pscore;
        };
        auto UpdatePerimeter = [&](int i, int j) -> void {
          if (!sel[i][j]) return;
          int p = -g_perimeter[i][j];
          for (auto [di, dj] : kAdj) {
            // if it aint selected, its perimeter
            p += sel[i + di][j + dj] ^ 1;
          }
          perimeter += p;
          g_perimeter[i][j] += p;
        };
        auto CanModify = [&](int i, int j) -> bool {
          bool ok = false;

          int prev = 1;
          int change = 0;
          int ds = 1;  // ensure connected adjacent
          while (ds < 8 && !sel[i + kSur[ds][0]][j + kSur[ds][1]]) ds += 2;
          if (ds < 8) {
            for (int d = 0; d <= 8; ++d) {
              auto [di, dj] = kSur[d + ds];
              int ni = i + di, nj = j + dj;
              int w = sel[ni][nj];
              if (!w && prev == -1) continue;
              if (w != prev) ++change;
              prev = w;
            }
          }
          if (change == 2) ok = true;  // 11110001111
          bool at_least_one_empty = false;
          for (auto [di, dj] : kAdj) {
            int ni = i + di, nj = j + dj;
            int w = sel[ni][nj];
            if (w == 0) at_least_one_empty = true;
          }
          ok &= at_least_one_empty;
          return ok;
        };
        auto Set = [&](int i, int j) -> bool {
          if (i == 0 || i == n - 1 || j == 0 || j == n - 1) return false;
          // you cannot remove internal squares!!
          // so a square can only be added when its on the boundary
          // (aka its touching in and out!!)
          if (sel[i][j]) return false;
          bool ok = CanModify(i, j);
          if (ok) {
            sel[i][j] = 1;
            score += g[i][j];
            UpdatePerimeter(i, j);
            for (auto [di, dj] : kAdj) UpdatePerimeter(i + di, j + dj);
          }
          return ok;
        };
        auto Unset = [&](int i, int j) -> bool {
          if (i == 0 || i == n - 1 || j == 0 || j == n - 1) return false;
          if (!sel[i][j]) return false;
          // oh hold up, how do you deal with not getting disconnected...
          //  .
          // ##.
          // ##.
          // if at least any three in a row in the adjacent, you're good
          // (you can remove the center)
          bool ok = CanModify(i, j);
          if (ok) {
            sel[i][j] = 0;
            score -= g[i][j];
            UpdatePerimeter(i, j);
            for (auto [di, dj] : kAdj) UpdatePerimeter(i + di, j + dj);
          }
          return ok;
        };
        auto Toggle = [&](int i, int j) -> bool {
          if (sel[i][j]) return Unset(i, j);
          return Set(i, j);
        };

        if (psel.size() == 1) {
          int bi, bj, bv = 0;
          for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
              if (g[i][j] > bv) {
                bv = g[bi = i][bj = j];
              }
            }
          }
          // bi = rnd(0, n-1);
          // bj = rnd(0, n-1);
          sel[bi][bj] = 1;
          score += g[bi][bj];
          // for (int i = 1; i < 15; ++i)
          //   if (!Set(bi - i, bj)) break;
          // for (int i = 1; i < 15; ++i)
          //   if (!Set(bi + i, bj)) break;
          // for (int i = 1; i < 15; ++i)
          //   if (!Set(bi, bj - i)) break;
          // for (int i = 1; i < 15; ++i)
          //   if (!Set(bi, bj + i)) break;
        } else {
          for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
              UpdatePerimeter(i, j);
            }
          }
        }

        for (int T = 0; T < MAX_T; ++T) {
          int xi = rnd(0, n - 1), xj = rnd(0, n - 1);
          std::vector<std::array<int, 2>> applied;

          double E = Eval(T / MAX_T);
          // int tries = 10;
          while (!Toggle(xi, xj)) {
            xi = rnd(0, n - 1), xj = rnd(0, n - 1);
          }
          // if (tries <= 0) continue;
          double Ep = Eval(T / MAX_T);

          // const double kMaxTemp = 1e-2;
          // const double kMinTemp = 1e-2;
          // double t = 1 - (T + 1) / MAX_T;
          // double temp =
          //     (std::exp(t) - 1) / (2.71829 - 1) * (kMaxTemp - kMinTemp) +
          //     kMinTemp;
          // double P = Ep > E ? std::exp(-(Ep - E) / temp) : 1;
          // std::cerr << "acceptP=" << P << std::endl;
          // if (P >= rnd(0, 1024) / 1024.0) {
          if (Ep < E) {
            // keep
          } else {
            Toggle(xi, xj);  // undo
            continue;
          }

          // {
          //   for (int i = 0; i < n; ++i) {
          //     for (int j = 0; j < n; ++j) {
          //       if (i == xi && j == xj)
          //         std::cout << " >" << std::setw(4);
          //       else
          //         std::cout << std::setw(6);
          //       if (sel[i][j])
          //         std::cout << g[i][j];
          //       else
          //         std::cout << ".";
          //     }
          //     std::cout << "\n";
          //   }
          //   std::cout << "perimeter=" << perimeter * S << "\n";
          //   std::cout << "score=" << Ep << "\n";
          //   std::cout << std::endl;
          // }

          // now generate the edges (for visualize)
          auto Visualize = [&]() -> vpt {
            // its a cycle
            std::vector<std::vector<std::array<int, 3>>> adj((n + 1) * (n + 1));
            int start = -1;
            auto AddEdge = [&](int ui, int uj, int vi, int vj) -> void {
              int uk = ui * (n + 1) + uj;
              int vk = vi * (n + 1) + vj;
              adj[uk].push_back({vk, vi, vj});
              adj[vk].push_back({uk, ui, uj});
              start = uk;
            };
            for (int i = 0; i < n; ++i) {
              for (int j = 0; j < n; ++j) {
                if (!sel[i][j]) continue;
                const static std::vector<std::array<int, 6>> kAdj2 = {
                    {-1, 0, 0, 0, 0, 1},  // ^
                    {0, 1, 0, 1, 1, 1},   // >
                    {1, 0, 1, 1, 1, 0},   // v
                    {0, -1, 1, 0, 0, 0},  // <
                };
                for (auto [di, dj, udi, udj, vdi, vdj] : kAdj2) {
                  int ni = i + di, nj = j + dj;
                  if (ni >= 0 && ni < n && nj >= 0 && nj < n && !sel[ni][nj]) {
                    AddEdge(i + udi, j + udj, i + vdi, j + vdj);
                  }
                }
              }
            }
            assert(start != -1 && "ought to have some edges");
            for (auto w : adj) {
              assert((w.size() == 0 || w.size() == 2) && "either 0 or 2 edges");
            }
            int prev = -1, curr = start;
            std::vector<std::array<int, 2>> order, ans;
            do {
              for (auto [next, xi, xj] : adj[curr]) {
                if (prev == next) continue;
                prev = curr;
                curr = next;
                order.push_back({xi, xj});
                break;
              }
            } while (curr != start);

            for (auto [i, j] : order) {
              if (ans.size() >= 2) {
                auto [pi, pj] = ans[ans.size() - 2];
                if (pi == i || pj == j) ans.pop_back();
              }
              ans.push_back({i, j});
            }
            for (auto &[x,y] : ans) {
              std::swap(x,y);
              x *= S;
              y *= S;
            }

            return ans;
          };

          psel = sel;
          // Visualize();
          if (score > best_score) {
            best = Visualize();
            best_score = score;
          }
        }
        // break;
      }
    }
    std::cout << best.size() << "\n";
    for (auto [x, y] : best) {
      std::cout << x << " " << y << "\n";
    }
    std::cerr << best_score/10 << "\n";
  }

 private:
  int N;   // num fish
  vpt ap;  // good (mackerel)
  vpt an;  // bad (sardine)
};

int32_t main() {
  // std::ios_base::sync_with_stdio(false);
  // std::cin.tie(0);

  std::srand(0);
  Solver solver(std::cin);
  solver.Solve(std::cout);
  return 0;
}