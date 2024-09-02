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
      edge_list.push_back({u, v});
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
    H = hubs.size();
    std::vector<int> to_hub_index(N, -1);
    for (int i = 0; i < H; ++i) to_hub_index[hubs[i]] = i;

    // compute APSP over hubs
    h_dist.assign(H, std::vector<int>(H));
    h_path.assign(H, std::vector<std::vector<int>>(H));
    std::vector<int> vis(N, -1);
    for (int i = 0; i < H; ++i) {
      const int s = hubs[i];
      // std::queue<int> q;
      // q.push(s);
      std::queue<std::vector<int>> q;
      q.push({s});

      for (int depth = 0; !q.empty(); ++depth) {
        int qn = q.size();
        while (qn--) {
          // int u = q.front();
          auto path = q.front();
          int u = path.back();

          q.pop();
          if (vis[u] == i) continue;
          vis[u] = i;
          if (to_hub_index[u] != -1) {
            h_dist[i][to_hub_index[u]] = depth;
            h_path[i][to_hub_index[u]] = path;
          }
          for (auto v : adj[u]) {
            if (vis[v] == i) continue;
            // q.push(v);
            auto path2 = path;
            path2.push_back(v);
            q.push(std::move(path2));
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
        edge_order.push_back({h_dist[i][j], i, j});
      }
    }
    std::sort(edge_order.begin(), edge_order.end());

    // hub network adjacency list
    h_adj.assign(H, {});
    UF dsu(H);
    // build MST first
    for (auto &[w, u, v] : edge_order) {
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
      if (w == -1) continue;  // used in MST
      bool take_edge = true;
      // compute h_adj SSSP distance
      // you'll take it, unless u-v is sufficiently close
      take_edge = true;
      const int threshold = std::max(8, (int)(h_dist[s][t] * 1.6));
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

  void ConnectNodes() {
    std::vector<int> nhubdist(N, N + 5);  // distance to nearest hub
    std::vector<std::set<int>> nhub(N);   // nearest hub(s)
    std::queue<std::array<int, 2>> q;     // <hub, node>
    for (int i = 0; i < H; ++i) q.push({i, hubs[i]});
    for (int depth = 0; !q.empty(); ++depth) {
      int qn = q.size();
      while (qn--) {
        auto [h, u] = q.front();
        q.pop();
        if (depth > nhubdist[u]) continue;
        nhub[u].insert(h);
        if (depth == nhubdist[u]) continue;
        nhubdist[u] = depth;
        for (auto v : adj[u]) q.push({h, v});
      }
    }

    // for (int i = 0; i < N; ++i) {
    //   std::cout << i << " d=" << nhubdist[i] << " | ";
    //   for (auto x : nhub[i]) std::cout << x << " ";
    //   std::cout << "\n";
    // }

    // edge usage
    typedef std::vector<double> vd;
    std::vector<vd> hw(H, vd(H));  // hub-to-hub weights
    std::vector<vd> nw(N, vd(H));  // node-to-hub weights
    for (int i = 0; i < T; ++i) {
      std::map<std::array<int, 2>, int> n_cts, h_cts;
      int tot = 0;

      int u = i == 0 ? 0 : targets[i - 1];
      int v = targets[i];
      if (u > v) std::swap(u, v);
      for (int hu : nhub[u]) {
        for (int hv : nhub[v]) {
          ++n_cts[{u, hu}];
          ++n_cts[{v, hv}];
          if (hu > hv) std::swap(hu, hv);
          ++h_cts[{hu, hv}];
          ++tot;
        }
      }

      for (auto [u_h, w] : n_cts) {
        auto [u, h] = u_h;
        nw[u][h] += (double)w / tot;
      }
      for (auto [hu_hv, w] : h_cts) {
        auto [hu, hv] = hu_hv;
        hw[hu][hv] += (double)w / tot;
      }
    }

    std::vector<std::tuple<double, int, int>> hwo;  // hub weight order
    for (int i = 0; i < H; ++i) {
      for (int j = i + 1; j < H; ++j) {
        if (hw[i][j]) hwo.push_back({hw[i][j], i, j});
      }
    }
    std::sort(hwo.rbegin(), hwo.rend());

    // for (auto [w, u, v] : hwo) std::cout << u << "->" << v << " " << w <<
    // "\n";
    std::vector<std::deque<int>> paths;
    std::vector<int> usage(N, -1);  // -1 if unassigned, -2 if assigned
                                    // >=0 if endpoint of a path
                                    // 2i if the front of the ith path
                                    // (2i+1) if the back of the ith path
    for (auto [w, s, t] : hwo) {    // IN HUB SPACE
      // find path from s to t (in hub space)
      std::vector<int> vis(H), st_path, uv_path;
      std::priority_queue<std::pair<int, std::vector<int>>> pq;
      pq.push({0, {s}});
      while (!pq.empty()) {
        auto [c, path] = pq.top();
        auto u = path.back();
        pq.pop();
        c *= -1;
        if (vis[u]) continue;
        vis[u] = 1;
        if (u == t) {
          st_path = std::move(path);
          break;
        }
        for (auto [v, dc] : h_adj[u]) {
          if (vis[v]) continue;
          auto path2 = path;
          path2.push_back(v);
          pq.push({v, std::move(path2)});
        }
      }

      for (int i = 1; i < st_path.size(); ++i) {
        int s = st_path[i - 1], t = st_path[i];
        if (!uv_path.empty()) uv_path.pop_back();
        for (auto u : h_path[s][t]) uv_path.push_back(u);
      }

      /* printing the hpath and path
      // std::cout << "h" << s << "->h" << t << " w=" << w << " | ";
      // for (auto x : st_path) std::cout << x << " ";
      // std::cout << "\n";
      // for (int i = 1; i < st_path.size(); ++i) {
      //   int s = st_path[i-1], t = st_path[i];
      //   for (auto x : h_path[s][t]) std::cout << x << " ";
      //   std::cout << ", ";
      // }
      // std::cout << "\n";
      // for (auto x : uv_path) std::cout << x << " ";
      // std::cout << "\n\n";
      */

      auto AppendToPath = [&](int ref, int u) -> void {
        int e = ref;
        if (!(e & 1)) paths[e / 2].push_front(u);
        if (e & 1) paths[e / 2].push_back(u);
      };

      for (int i = 1; i < uv_path.size(); ++i) {
        int u = uv_path[i - 1], v = uv_path[i];
        if (usage[v] == -1) std::swap(u, v);  // ensure if there is a -1, its u first
        // if usage[u] == -2 (assigned) -- do nothing
        if (usage[u] == -1) {  // one unassigned
          // if usage[v] == -2 (assigned) -- do nothing
          // in future -- you'll take into account extra A space
          if (usage[v] == -1) {  // both unassigned
            usage[u] = usage[v] = 2 * paths.size();
            ++usage[v];
            paths.push_back({u, v});
          } else if (usage[v] >= 0) {  // one is an endpoint
            int e = usage[v];
            usage[u] = e;
            usage[v] = -2;
            AppendToPath(e, u);
          }
        } else if (usage[u] >= 0) {  // part of an endpoint
          if (usage[v] >= 0) {       // ALSO an endpoint??
            // whoa combine the paths
            int eu = usage[u];
            int ev = usage[v];
            if (eu == ev) continue; // uh you can skip this
            usage[u] = usage[v] = -2;
            AppendToPath(eu, v);
            // AppendToPath(ev, u);
          }
          // else usage[v] == -2 ;; assigned
        }
      }
    }
    std::cout << "var paths=[\n";
    for (auto &path : paths) {
      std::cout << "[";
      for (auto x : path) std::cout << x << ",";
      std::cout << "],";
    }
    std::cout << "\n];\n";
  }

  /**
   * @brief Output render code to visualize.
   *
   * @param os ostream to write to
   */
  void Render(std::ostream &os) {
    os << "var coords=[\n";
    for (auto [x, y] : coords) os << "[" << x << "," << y << "],";
    os << "\n];\nvar edges=[\n";
    for (auto [x, y] : edge_list) os << "[" << x << "," << y << "],";
    os << "\n];\nvar hubs=[\n";
    for (auto x : hubs) os << x << ",";
    os << "\n];";
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
  std::vector<std::array<int, 2>> edge_list;
  std::vector<std::vector<int>> adj;  // adj-list for nodes
  std::vector<int> A, B;              // active nodes (slow/fast memory)
  std::vector<int> targets;           // paths that must be taken
  std::vector<std::array<int, 2>> coords;

  int H;
  std::vector<int> hubs;                               // which nodes are hubs
  std::vector<std::vector<std::array<int, 2>>> h_adj;  // adj-list for hubs
  std::vector<std::vector<std::vector<int>>> h_path;   // APSP over hub paths
  std::vector<std::vector<int>> h_dist;                // APSP over hubs
};

int32_t main() {
  std::srand(0);
  Solver sol;
  sol.Parse(std::cin);
  sol.AssignHubs();
  sol.ConnectHubs();
  sol.ConnectNodes();
  // sol.Render(std::cout);
  return 0;
}