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

inline int RandInt() { return std::rand(); }

class Solver {
 public:
  Solver() {}

  /**
   * @brief Parses input
   *
   * @param cin stream to read from
   */
  void Parse(std::istream &cin) {
    cin >> N >> M >> T >> LA >> LB;
    adj.resize(N);
    A.assign(LA, -1);
    B.assign(LB, -1);
    for (int i = 0; i < M; ++i) {
      int u, v;
      cin >> u >> v;
      adj[u].push_back(v);
      adj[v].push_back(u);
    }
    targets.resize(T);
    for (auto &x : targets) cin >> x;
    coords.resize(N);
    for (auto &[x, y] : coords) cin >> x >> y;
  }

  void AssignHubs() {
    // std::vector<int> hubs;
    std::vector<bool> is_hub(N, false);

    for (int i = 0; i < 3; ++i) {  // random seeding
      int x;
      do {
        x = RandInt() % N;
      } while (is_hub[x]);
      is_hub[x] = true;
      hubs.push_back(x);
    }

    int maxhdist = 0;
    std::vector<int> hdist(N);  // distance to hub
    // but kinda only used for visited in this case

    auto HubDistance = [&]() -> int {
      hdist.assign(N, -1);
      std::queue<int> q;
      for (auto h : hubs) q.push(h);

      // computes distance to nearest hub
      int depth = 0;
      for (; !q.empty(); ++depth) {
        int qn = q.size();
        while (qn--) {
          int u = q.front();
          q.pop();
          if (hdist[u] != -1) continue;
          hdist[u] = depth;
          maxhdist = u;
          for (auto v : adj[u]) {
            if (hdist[v] != -1) continue;
            q.push(v);
          }
        }
      }
      return depth;  // returns max depth
    };

    while (HubDistance() > 4) {
      hubs.push_back(maxhdist);
    }

    // for (auto x : hubs) std::cout << x << " ";
    // std::cout << "\nvar coords=[";
    // for (auto [x, y] : coords) std::cout << "[" << x << "," << y << "],";
    // std::cout << "]";
  }

  void ConnectHubs() {
    const int H = hubs.size();
    std::vector<int> to_hub_index(N, -1);
    for (int i = 0; i < H; ++i) to_hub_index[hubs[i]] = i;

    // compute APSP over hubs
    std::vector<std::vector<int>> hdist(H, std::vector<int>(H));
    std::vector<int> vis(N, -1);
    for (int i = 0; i < H; ++i) {
      const int s = hubs[i];
      std::queue<int> q;
      q.push(s);
      for (int depth = 0; !q.empty(); ++depth) {
        int qn = q.size();
        while (qn--) {
          int u = q.front();
          q.pop();
          if (vis[u] == i) continue;
          vis[u] = i;
          if (to_hub_index[u] != -1) hdist[i][to_hub_index[u]] = depth;
          for (auto v : adj[u]) {
            if (vis[v] == i) continue;
            q.push(v);
          }
        }
      }
    }

    // build a stuff MST, more of a MSG, st
    // the existing network is no worse than a factor of 1.6x
    // localroute=2 per step, optimalroute= ~1.2 per step
    // so this gives a factor of about 2/1.2 ~ 1.67x worse allowed
    std::vector<std::array<int, 3>> edge_order;
    for (int i = 0; i < H; ++i) {
      for (int j = i + 1; j < H; ++j) {
        edge_order.push_back({hdist[i][j], i, j});
      }
    }
    std::sort(edge_order.begin(), edge_order.end());

    // hub network adjacency list
    std::vector<std::vector<std::array<int, 2>>> h_adj(H);
    UF dsu(H);
    // build MST first
    for (auto& [w, u, v] : edge_order) {
      if (!dsu.sameSet(u, v)) {
        h_adj[u].push_back({v, w});
        h_adj[v].push_back({u, w});
        dsu.join(u, v);
        // std::cout << "[" << u << "," << v << "],";
        w = -1;
      }
    }
    // std::cout << "\n";

    // then add edges to form MSG
    for (auto [w, s, t] : edge_order) {
      if (w == -1) continue; // used in MST
      bool take_edge = true;
      // compute h_adj SSSP distance
      // you'll take it, unless u-v is sufficiently close
      take_edge = true;
      const int threshold = std::max(8, (int)(hdist[s][t] * 1.6));
      std::priority_queue<std::array<int, 2>> pq;
      pq.push({0, s});
      int vis_id = s * H + t;
      while (!pq.empty()) {
        auto [c, u] = pq.top();
        pq.pop();
        c *= -1;
        if (vis[u] == vis_id) continue;
        vis[u] = vis_id;
        if (c > threshold) continue;
        if (u == t) {
          take_edge = false;
          break;
        }
        for (auto [v, dc] : h_adj[u]) {
          if (vis[v] == vis_id) continue;
          pq.push({-(c + dc), v});
        }
      }
      if (take_edge) {
        h_adj[s].push_back({t, w});
        h_adj[t].push_back({s, w});
        // std::cout << "[" << s << "," << t << "],";
      }
    }
  }

 private:
  // struct Edge {
  //   int frequency = 0;
  // };
  // struct Node {
  //   bool is_hub = false;
  //   std::vector<std::pair<int, Edge*>> adj;
  // };

  int N, M, T, LA, LB;
  std::vector<std::vector<int>> adj;
  std::vector<int> A, B;
  std::vector<int> targets;
  std::vector<std::array<int, 2>> coords;
  std::vector<int> hubs;
};

int32_t main() {
  std::srand(0);
  Solver sol;
  sol.Parse(std::cin);
  sol.AssignHubs();
  sol.ConnectHubs();
  return 0;
}