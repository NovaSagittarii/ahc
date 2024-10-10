function f(x, y) {
  ellipse(200 + x * 20, 200 + y * 20, 5, 5);
}
f(0, 0);
function dfs(L, x, y, i) {
  if (i < L.length) {
    dfs(L, x + L[i], y, i + 1);
    dfs(L, x - L[i], y, i + 1);
    dfs(L, x, y + L[i], i + 1);
    dfs(L, x, y - L[i], i + 1);
  } else {
    f(x, y);
  }
}
dfs([4, 2, 1], 0, 0, 0);
