#include <bits/stdc++.h>

struct Vec2 {
  int x, y;

  Vec2(int x, int y) : x(x), y(y) {}
  Vec2 operator+(const Vec2 &o) const { return {x + o.x, y + o.y}; }
  Vec2 operator-(const Vec2 &o) const { return {x - o.x, y - o.y}; }
  Vec2 operator*(const int k) const { return {x * k, y * k}; }

  int manhattan() { return std::abs(x) + std::abs(y); }
  int parity() { return (x + y) & 1; }

  friend std::ostream &operator<<(std::ostream &os, const Vec2 &o) {
    os << "(" << o.x << " " << o.y << ")";
    return os;
  }
};
const std::vector<Vec2> dirs = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const std::string dirs_str = "RDLU";

class Arm {
 public:
  Arm(int len, char &out, char &g_out) : L(len), out(out), g_out(g_out) {}

  int len() const { return L; }
  /**
   * @brief Sets and returns if there is work to be done.
   *
   * @param ndir new direction
   */
  bool Set(int ndir) {
    this->ndir = ndir;
    return ndir != dir;
  }
  void Grab() { g_out = 'P'; }
  void Ungrab() { g_out = '.'; }
  void Tick() {
    // std::cerr << dir << "->" << ndir << " ";
    if (ndir == dir) {
      out = '.';
    } else if ((ndir - dir + 4) % 4 == 1) {
      ++dir;
      out = 'R';
    } else {
      --dir;
      out = 'L';
    }
    dir = (dir + 4) % 4;
    // std::cerr << " ;now " << dir << "\n";
  }
  Vec2 Get() { return dirs[dir] * L; }

 private:
  int L;
  int dir = 0;
  int ndir = 0;
  char &out;
  char &g_out;
};

class Crane {
 public:
  /**
   * Construct a new Crane object
   *
   * 2 4 8 16
   *
   * arm_complexity should be 3 (N<=16) or 4 (N>=17)
   *
   * @param arm_complexity how many nodes are used for the arm
   * @param leaves nodes used for the leaves
   * @param state string output state
   * @param gs grabbing output state
   */
  Crane(int arm_complexity, std::vector<int> leaves, std::string &state,
        std::string &gs) {
    for (int l = arm_complexity - 1; l >= 0; --l) {
      state.push_back('?');
      gs.push_back('.');
      arms_.push_back({2 << l, state.back(), gs.back()});
    }
    for (auto l : leaves) {
      state.push_back('?');
      gs.push_back('.');
      leaves_.push_back({l, state.back(), gs.back()});
    }
  }

  int size() const { return arms_.size(); }
  std::vector<Arm> &arms() { return arms_; }
  std::vector<Arm> &leaves() { return leaves_; }

  bool GoTo(Vec2 from, Vec2 to) {
    bool busy = false;
    Vec2 u = from;
    int ddir = 0;
    for (auto &arm : arms_) {
      int ndir = GetDirection(u, to, arm.len());
      ndir = (ndir - ddir + 4) % 4;
      if (arm.Set(ndir)) busy = true;
      ndir = (ndir + ddir) % 4;
      ddir = ndir;
      u = u + dirs[ndir] * arm.len();
      // std::cerr << u << " " << arm.len() << " d=" << ndir << " " <<
      // dirs_str[ndir] << "\n";
    }
    int ndir = GetDirection(u, to, 1);
    ndir = (ndir - ddir + 4) % 4;
    if (leaves_[0].Set(ndir)) busy = true;
    ndir = (ndir + ddir) % 4;

    return busy;
  }
  void Tick() {
    for (auto &arm : arms_) arm.Tick();
    for (auto &arm : leaves_) arm.Tick();
  }
  void WriteSetup(std::vector<std::array<int, 2>> &o) const {
    for (int i = 0; i < arms_.size(); ++i) {
      if (i == 0) {  // attach to root
        o.push_back({0, arms_[i].len()});
      } else {  // attach to prev
        o.push_back({(int)o.size(), arms_[i].len()});
      }
    }
    int connection_point = o.size();
    for (const auto &arm : leaves_) {
      o.push_back({connection_point, arm.len()});
    }
  }

  static int GetDirection(Vec2 from, Vec2 to, int k = 1) {
    int ret = 0;
    int best = 9999;
    for (int d = 0; d < 4; ++d) {
      int dist = (from + (dirs[d] * k) - to).manhattan();
      if (dist < best) {
        best = dist;
        ret = d;
      }
    }
    return ret;
  }

 private:
  std::vector<Arm> arms_;
  std::vector<Arm> leaves_;
};

class Solver {
 public:
  typedef std::vector<std::string> vstr;
  Solver(int N, int M, int V, vstr A, vstr B) : N(N), M(M), V(V), A(A), B(B) {}

  void Solve(std::ostream &os) {
    std::vector<std::array<int, 2>> tree;
    std::string s = ".";  // root
    std::string gs = ".";
    Crane crane(V == 5 || N <= 16 ? 3 : 4, {1}, s, gs);
    bool FULL = !(V == 5 && N > 16);
    int REACH = (2 << crane.arms().size()) - 1;
    // std::cerr << "reach" << REACH << std::endl;
    crane.WriteSetup(tree);
    Vec2 center(N / 2, N / 2);
    if (!FULL) center = {0, N / 2};

    Vec2 kcenter = center;

    Vec2 tcenter = center;  // target center position
    auto UpdateTCenter = [&](Vec2 p) -> void {
      tcenter = center;
      while ((p - tcenter).parity() == 0 || (p - tcenter).manhattan() > REACH) {
        int d = Crane::GetDirection(tcenter,
                                    FULL && tcenter.parity() ? kcenter : p);
        s[0] = dirs_str[d];
        tcenter = tcenter + dirs[d];
      }
    };
    auto PostTick = [&]() -> void { s[0] = '.'; };
    auto Hover = [&](Vec2 p, bool can_move_center) -> bool {
      // std::cerr << center << "->" << p << "\n";
      bool busy = false;
      if ((p - center).parity() == 0 || (p - center).manhattan() > REACH) {
        int d =
            Crane::GetDirection(center, FULL && center.parity() ? kcenter : p);
        s[0] = dirs_str[d];
        if (can_move_center) center = center + dirs[d];
        busy = true;
      }
      return crane.GoTo(tcenter, p) || busy;
    };

    std::vector<Vec2> a, b;
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if (A[i][j] == B[i][j]) continue;
        if (A[i][j] == '1') a.push_back({i, j});
        if (B[i][j] == '1') b.push_back({i, j});
      }
    }
    // std::random_shuffle(a.begin(), a.end());
    // std::random_shuffle(b.begin(), b.end());

    os << s.size() << "\n";
    for (auto [x, y] : tree) std::cout << x << " " << y << "\n";
    os << center.x << " " << center.y << "\n";
    for (int i = 0; i < a.size(); ++i) {
      UpdateTCenter(a[i]);
      while (Hover(a[i], true)) {
        crane.Tick();
        if (!Hover(a[i], false)) crane.leaves()[0].Grab();
        os << s << gs << '\n';
        PostTick();
        crane.leaves()[0].Ungrab();
      }
      UpdateTCenter(b[i]);
      while (Hover(b[i], true)) {
        crane.Tick();
        if (!Hover(b[i], false)) crane.leaves()[0].Grab();
        os << s << gs << '\n';
        PostTick();
        crane.leaves()[0].Ungrab();
      }
    }
  }

 private:
  int N, M, V;
  std::vector<std::string> A, B;
};

int32_t main() {
  // std::ios_base::sync_with_stdio(false);
  // std::cin.tie(0);

  int N, M, V;
  std::cin >> N >> M >> V;
  std::vector<std::string> a(N), b(N);
  for (auto &x : a) std::cin >> x;
  for (auto &x : b) std::cin >> x;

  Solver solver(N, M, V, a, b);
  solver.Solve(std::cout);
  return 0;
}