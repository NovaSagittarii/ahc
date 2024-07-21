#include <bits/stdc++.h>

class Solution {
 public:
  Solution(std::istream& in, std::ostream& out) : in_(in), out_(out) {
    in >> n_ >> m_ >> t_;
    seed_count_ = 2 * n_ * (n_ - 1);
    a_.assign(seed_count_, std::vector<int>(m_));
    char_max_.assign(m_, 0);
    icm.clear();
  }

  void ReadSeeds() {
    for (int i = 0; i < m_; ++i) char_max_[i] = 0;
    for (int i = 0; i < seed_count_; ++i) {
      for (int j = 0; j < m_; ++j) {
        int x;
        in_ >> x;
        char_max_[j] = std::max(char_max_[j], x);
        a_[i][j] = x;
      }
    }
    if (icm.empty()) icm = char_max_;
  }

  void Breed() {
    /**
     * Want to select candidates.
     * Type, offspring, amt
     * DISCARD [0] -- 20
     * LOW_PR  [2] --  4
     * MED_PR  [3] -- 16
     * HIGH_PR [4] -- 16
     */
    // inputs seedcount*M = 60*15 = 900 vars "context" -- possibly drop 15?
    // and then for each seed, see how it compares -- eval it.

    std::vector<std::pair<double, int>> results(seed_count_);
    for (int i = 0; i < seed_count_; ++i) {
      results[i] = {Evaluate(a_[i]), i};
    }
    // std::shuffle(results.begin(), results.end(),
    // std::default_random_engine(0));
    std::sort(results.rbegin(), results.rend());

    std::vector<std::vector<int>> out(n_, std::vector<int>(n_, -1));
    int k = 0;  // which result current looking at
    for (int i = 1; i < n_ - 1; ++i) {
      for (int j = 1; j < n_ - 1; ++j) {
        out[i][j] = results[k++].second;
      }
    }
    for (int i = 1; i < n_ - 1; ++i) {
      out[0][i] = results[k++].second;
      out[n_ - 1][i] = results[k++].second;
      out[i][0] = results[k++].second;
      out[i][n_ - 1] = results[k++].second;
    }
    out[0][0] = results[k++].second;
    out[n_ - 1][0] = results[k++].second;
    out[0][n_ - 1] = results[k++].second;
    out[n_ - 1][n_ - 1] = results[k++].second;

    for (auto& row : out) {
      for (auto x : row) out_ << x << " ";
      out_ << "\n";
    }
    out_ << std::flush;
  }

  void Run() {
    for (int i = 0; i < t_; ++i) {
      ReadSeeds();
      Breed();
    }
  }

 private:
  typedef std::vector<int> vi;
  typedef std::vector<vi> vvi;

  double Evaluate(const vi& w) {
    double score = 0;
    double value = 0;
    for (int i = 0; i < m_; ++i) {
      double x = std::abs(std::pow(std::abs((double)w[i] / char_max_[i]), 0.5));
      if (w[i] >= icm[i] * 0.5) x *= 1.1;
      if (w[i] >= icm[i] * 0.7) x *= 1.5;
      if (w[i] >= icm[i] * 0.8) x *= 2;
      if (w[i] >= icm[i] * 0.9) x *= 4;
      if (w[i] >= icm[i] * 0.95) x *= 8;
      if (w[i] == icm[i]) x *= 10; // please keep these
      score += x;
      value += std::abs(w[i]);
    }
    return score + std::sqrt(value);
  }

  std::istream& in_;     // read input from here
  std::ostream& out_;    // write output to here
  int n_ = 6;            // grid dimension
  int m_ = 15;           // eval vector size
  int t_ = 10;           // number of iterations
  int seed_count_ = 60;  // = 2n(n-1)
  vvi a_;                // [seedcount][m] holds current seed characteristics
  vi char_max_;          // [m] characteristic max
  vi icm;                // [m] initial characteristic max
};

// header guards to when compiling main.cpp as header in jury.cpp
#ifndef AHC035_JURY_MODE_
#define AHC035_JURY_MODE_
int32_t main() {
  Solution sol(std::cin, std::cout);
  sol.Run();
}
#endif  // AHC035_JURY_MODE_
