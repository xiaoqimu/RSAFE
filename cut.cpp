#include <iostream>
#include <vector>

using namespace std;

// 定义图的最大节点数
const int MAXN = 100;

vector<int> adj[MAXN];  // 存储图的邻接表
int low[MAXN];          // 记录每个节点能够访问到的最小编号
int dfn[MAXN];          // 记录每个节点的深度优先搜索编号
bool visited[MAXN];     // 记录节点是否已被访问
int tim = 0;            // 深度优先搜索的计时器

vector<pair<int, int>> cutEdges;  // 存储割边的结果

// 深度优先搜索遍历图，寻找割边
void dfs(int u, int fa) {
    visited[u] = true;
    dfn[u] = low[u] = ++tim;

    for (int v : adj[u]) {
        if (v == fa) continue;  // 防止重复访问父节点

        if (!visited[v]) {
            dfs(v, u);
            // 更新当前节点u能够访问到的最小编号
            low[u] = min(low[u], low[v]);
            
            // 判断(u, v)是否是割边
            if (low[v] > dfn[u]) {
                cutEdges.push_back(make_pair(u, v));
            }
        } else {
            // 更新当前节点u能够访问到的最小编号
            low[u] = min(low[u], dfn[v]);
        }
    }
}

// 寻找无向图的割边
vector<pair<int, int>> findCutEdges(int n) {
    cutEdges.clear();
    tim = 0;
    fill(dfn, dfn + n, 0);
    fill(low, low + n, 0);
    fill(visited, visited + n, false);

    // 从每个未访问过的节点出发进行深度优先搜索
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            dfs(i, -1);
        }
    }

    return cutEdges;
}

int main() {
    int n, m;
    freopen("cut.in","r",stdin);
    cin >> n >> m;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<pair<int, int>> cutEdges = findCutEdges(n);

    cout << "割边集为：" << endl;
    for (const auto& edge : cutEdges) {
        cout << edge.first << " " << edge.second << endl;
    }

    return 0;
}
