// Problem 080 - Count ministers never invitible
// Approach:
// Build graph of "no-witness" pairs. A valid banquet seating is an odd-length cycle.
// A minister is invitible iff they lie on at least one odd cycle.
// We count vertices not in any odd cycle.
//
// Technique:
// - BFS each component to build a forest with parent/depth and a 0/1 color.
// - Any edge (u,v) with color[u]==color[v] implies an odd cycle along the tree path u-v plus (u,v).
// - For each such edge, mark the path using node-path difference on the BFS tree:
//     add[u]++, add[v]++, add[lca(u,v)]--, add[parent[lca(u,v)]]--.
// - Accumulate values from deeper nodes to roots (by descending depth). Nodes with sum>0 lie on odd cycles.
// - Answer = n - count(sum>0).
// Complexity: O((n+m) log n) due to LCA with binary lifting.

#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> g(n + 1);
    g.reserve(n + 1);
    for (int i = 0; i < m; ++i) {
        int x, y; cin >> x >> y;
        if (x < 1 || x > n || y < 1 || y > n) continue;
        if (x == y) {
            // Self-loop would be an odd cycle of length 1, but banquet requires >=3.
            // Ignore as it doesn't help form valid seating.
            continue;
        }
        g[x].push_back(y);
        g[y].push_back(x);
    }

    const int LOG = 20; // since n <= 1e5, 17 is enough; 20 for safety
    vector<array<int, 20>> up(n + 1);
    vector<int> depth(n + 1, 0);
    vector<int> parent(n + 1, 0);
    vector<int> color(n + 1, -1);
    vector<pair<int,int>> odd_edges;
    odd_edges.reserve(m);

    // BFS forest
    vector<int> order; order.reserve(n);
    for (int s = 1; s <= n; ++s) if (color[s] == -1) {
        color[s] = 0; depth[s] = 0; parent[s] = 0; up[s].fill(0);
        queue<int> q; q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            for (int v : g[u]) {
                if (color[v] == -1) {
                    color[v] = color[u] ^ 1;
                    parent[v] = u;
                    depth[v] = depth[u] + 1;
                    up[v].fill(0);
                    up[v][0] = u;
                    q.push(v);
                } else if (v != parent[u]) {
                    if (color[v] == color[u]) {
                        odd_edges.emplace_back(u, v);
                    }
                }
            }
        }
    }

    // Build binary lifting table
    for (int k = 1; k < LOG; ++k) {
        for (int v = 1; v <= n; ++v) {
            int mid = up[v][k-1];
            up[v][k] = mid ? up[mid][k-1] : 0;
        }
    }

    auto lca = [&](int a, int b) {
        if (a == 0 || b == 0) return a ^ b; // if one is 0, return the other
        if (depth[a] < depth[b]) swap(a, b);
        int diff = depth[a] - depth[b];
        for (int k = 0; diff; ++k) {
            if (diff & 1) a = up[a][k];
            diff >>= 1;
        }
        if (a == b) return a;
        for (int k = LOG - 1; k >= 0; --k) {
            if (up[a][k] != up[b][k]) {
                a = up[a][k];
                b = up[b][k];
            }
        }
        return parent[a];
    };

    vector<int> add(n + 1, 0);
    add.shrink_to_fit();
    for (auto [u, v] : odd_edges) {
        int w = lca(u, v);
        add[u] += 1;
        add[v] += 1;
        if (w) add[w] -= 1;
        int pw = w ? parent[w] : 0;
        if (pw) add[pw] -= 1;
    }

    // Accumulate from deeper to shallower using depth order
    // Sort nodes by depth descending
    vector<int> nodes(n);
    iota(nodes.begin(), nodes.end(), 1);
    sort(nodes.begin(), nodes.end(), [&](int a, int b){ return depth[a] > depth[b]; });
    for (int u : nodes) {
        if (parent[u]) add[parent[u]] += add[u];
    }

    long long marked = 0;
    for (int i = 1; i <= n; ++i) if (add[i] > 0) ++marked;
    cout << (n - marked) << '\n';
    return 0;
}

