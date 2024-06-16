#include <bits/stdc++.h>

typedef std::vector<std::vector<int>> vvi;
typedef std::array<int, 2> ai2;

inline int dist(int x1, int y1, int x2, int y2) {
  return std::abs(x1-x2) + std::abs(y1-y2);
}
inline int dist(int x1, int y1, ai2 xy2) {
  auto [x2, y2] = xy2;
  return dist(x1, y1, x2, y2);
}

class Solution {
 private:
  int n;
  vvi a;
  long long cost = 0;
  int di = 0;
  int dj = 0;
  int curr = 0;
  std::ostringstream out;
  ai2 Highest() const {
    ai2 res {-1, -1};
    int best = -200;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (a[i][j] > best) {
          best = a[i][j];
          res = {i, j};
        }
      }
    }
    return res;
  }
  ai2 NearHi() const {
    ai2 res {-1, -1};
    int best = 2000;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (a[i][j] > 0) {
          int d = dist(i, j, di, dj);
          if (d < best) {
            best = d;
            res = {i, j};
          }
        }
      }
    }
    return res;
  }
  ai2 NearLo() const {
    ai2 res {-1, -1};
    int best = 2000;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (a[i][j] < 0) {
          int d = dist(i, j, di, dj);
          if (d < best) {
            best = d;
            res = {i, j};
          }
        }
      }
    }
    return res;
  }
  void Load(int x) {
    a[di][dj] -= x;
    curr += x;
    cost += x;
    out << '+' << x << '\n';
  }
  void Unload(int x) {
    a[di][dj] += x;
    curr -= x;
    cost += x;
    out << '-' << x << '\n';
  }
  void Level() {
    if (a[di][dj] == 0) return;
    if (a[di][dj] < 0) { // fill
      int w = std::min(std::abs(a[di][dj]), curr);
      if (curr > 0) {
        Unload(w);
      }
    } else { // flatten
      Load(a[di][dj]);
    }
  }
  bool GoTo(ai2 ij) {
    auto [i, j] = ij;
    return GoTo(i, j);
  }
  bool GoTo(int i, int j) { // returns true if successful movement
    if (i == -1) return false;
    char w = '?';
    if (di < i) { // too high, go down
      w = 'D';
      ++di;
    } else if (di > i) { // too low, go up
      w = 'U';
      --di;
    } else {
      if (dj < j) { // too left, go right
        w = 'R';
        ++dj;
      } else if (dj > j) { // too right, go left
        w = 'L';
        --dj;
      }
    }
    if (w != '?') { // did you make a movement?
      out << w << '\n';
      cost += curr + 100;
    }
    return true;
  }
 public:
  Solution(int n, const vvi& a) {
    this->n = n;
    this->a = a;
  }

  /**
   * T  -- maximum time spent looking for additional High in state=1 (pickup)
   * D  -- maximum distance to get additional High in state=1 (pickup)
   * D2 -- maximum distance rerouting to get additional High in state=2 (dumping)
   */
  void Solve(int T, int D, int D2) {
    int state = 0;
    int t = 0; // time spent collecting
    while (NearLo()[0] != -1 || curr) {
      Level();
      switch (state) {
        case 0:
          if (GoTo(Highest())) {
            state = 1; // find nearby
            t = 0;
          } else {
            state = 2; // there are no more, just spread
          }
          break;
        case 1: {
          auto [li, lj] = NearHi();
          if (t <= T && dist(di, dj, li, lj) <= D) {
            if (GoTo(li, lj)) {
              ++t;
            } else {
              state = 2; // nothing to go to
              t = 0;
            }
          } else {
            state = 2; // give up
          }
          break;
        }
        case 2:
          if (curr > 0) {
            auto l = NearHi();
            if (t <= T && dist(di, dj, l) <= D2) {
              if (GoTo(l)) {
                ++t;
              } else t = T+1;
            } else {
              GoTo(NearLo());
            }
          } else {
            state = 0;
          }
          break;
      }
    }
  }
  double ComputeScore() const {
    return 1e9 / cost;
  }
  std::string Export() const {
    return out.str();
  }
};

int32_t main() {
  int n;
  std::cin >> n;
  std::vector<std::vector<int>> a(n, std::vector<int>(n));
  for (auto &row : a) {
    for (auto &x : row) std::cin >> x;
  }

  double best = 0;
  std::string sol;
  for (int t = 5; t <= 40; ++t) {
    for (int d = 3; d <= 9; d += 2) {
      for (int d2 = 0; d2 <= 18; d2 += 2) {
        Solution s(n, a);
        s.Solve(t, d, d2);
        double score = s.ComputeScore();
        if (score > best) {
          best = score;
          sol = s.Export();
        }
      }
    }
  }
  std::cout << sol;
}