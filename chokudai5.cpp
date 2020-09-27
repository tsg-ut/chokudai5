#include <bits/stdc++.h>
#include <atcoder/all>

#define rep_(i, a_, b_, a, b, ...) for (int i = (a), i##_len = (b); i < i##_len; ++i)
#define rep(i, ...) rep_(i, __VA_ARGS__, __VA_ARGS__, 0, __VA_ARGS__)
#define reprev_(i, a_, b_, a, b, ...) for (int i = (b-1), i##_min = (a); i >= i##_min; --i)
#define reprev(i, ...) reprev_(i, __VA_ARGS__, __VA_ARGS__, 0, __VA_ARGS__)
#define all(x) (x).begin(), (x).end()
template <class T> bool chmax(T &a, const T &b) { if (a < b) { a = b; return 1; } return 0; }
template <class T> bool chmin(T &a, const T &b) { if (b < a) { a = b; return 1; } return 0; }
// rep (i, n)       -> 0, 1, ..., n-1
// rep (i, l, r)    -> l, l+1, ..., r-1
// reprev (i, n)    -> n-1, n-2, ..., 1, 0
// reprev (i, l, r) -> r-1, r-2, ..., l+1, l

using namespace std;
using namespace atcoder;

namespace MMNMM{
    class splitmix64{
        unsigned long x;
    public:
        using result_type = unsigned long;
        constexpr unsigned long max() const {
            return std::numeric_limits<unsigned long>::max();
        }
        constexpr unsigned long min() const {
            return std::numeric_limits<unsigned long>::min();
        }
        constexpr explicit splitmix64(unsigned long seed = 0) : x(seed) {}
        constexpr unsigned long operator()() {
            unsigned long z = (x += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            return z ^ (z >> 31);
        }
    };
}

MMNMM::splitmix64 engine(std::random_device{}());

struct Query
{
    // these are all 1-indexed
    int y;
    int x;
    int c;
};
std::ostream& operator << (std::ostream& os, const Query& q) {
	os << q.y << " " << q.x << " " << q.c;
    return os;
}
int initialColorMap[100][100]; // 入力として与えられたcolormap
int colorMap[100][100]; // 現クエリにおけるcolormapの状態
int id, N, K;

void solve(vector<Query>& answer); // 前方宣言
int calc_score(vector<Query> &answer);

template<class T, class S>
void guchol(int N, T&& mp, S&& ans){
    vector<int> cnt(10);
    for(const auto& i : mp)for(const auto& j : i)cnt[j]++;
    const auto to{static_cast<int>(max_element(begin(cnt), end(cnt)) - begin(cnt))};
    const auto& fill = [&mp](const auto& f, int x, int y, int c, int d) -> void {
        for(const auto& [i, j] : vector<pair<int, int>>{{1, 0}, {-1, 0}, {0, 1}, {0, -1}})if(mp[x + i][y + j] == d){
            mp[x + i][y + j] = c;
            f(f, x + i, y + j, c, d);
        }
    };
    for(int i{1}; i <= N; ++i){
        for(int j{1}; j <= N; ++j){
            if(mp[i][j] != to){
                ans.emplace_back(Query{i, j, to});
                fill(fill, i, j, to, mp[i][j]);
            }
        }
    }
    cout << size(ans) << endl;
    for(const auto& [x, y, c] : ans)cout << x << " " << y << " " << c << endl;
}


int main()
{
    cin >> id >> N >> K; // N == 100, K == 9
    std::string line;
    rep (i, N) {
        cin >> line;
        rep (j, N) {
            int col = line[j] - '0';
            initialColorMap[i][j] = col;
            colorMap[i][j] = col;
        }
    }
    cerr << "ready " << endl;
    vector<Query> queries;
    solve(queries);

    // guchol(N, colorMap, queries); /*

    cout << queries.size() << endl;
    for (auto&& q : queries)
		std::cout << q << std::endl;
    //*/

/*
    std::cerr << "<<colormap>>" << std::endl;
    for (auto&& row : colorMap) {
        for (auto&& c : row)
            std::cerr << c;
        
        std::cerr << std::endl;
    }
*/

    return 0;
}

int index(int row, int col)
{
    return row * N + col;
}
void buildDSU(dsu& forest) // order: N^2 a(N)
{
    rep (i, N)
    {
        rep (j, N)
        {
            int self = colorMap[i][j];
            if (i-1 >= 0 && self == colorMap[i-1][j])
                forest.merge(index(i,j), index(i-1, j));
            if (i+1 < N && self == colorMap[i+1][j])
                forest.merge(index(i,j), index(i+1, j));
            if (j-1 >= 0 && self == colorMap[i][j-1])
                forest.merge(index(i,j), index(i, j-1));
            if (j+1 < N && self == colorMap[i][j+1])
                forest.merge(index(i,j), index(i, j+1));
        }
    }
}

void solve(vector<Query>& answer)
{
    /* いちばん連結成分数が減る貪欲法
     * 各クエリで各マス周りのuniqueな連結成分数を数える
     * order is O(N^4 a(N))
     * score is 49,972,451
     */

    // build dsu
    dsu forest(N * N);
    buildDSU(forest);
    cerr << "build done" << endl;

    for (int q = 0; q < N*N-1; q++) // for each query
    {
        Query bestQ;
        double bestScore = -1;
        set<int> bestColorset;
        int bestLeader;
        // 1 merge -> 連結成分--;
        // 周りのuniqueな連結成分数を数える

        bool iscontinue = false;
        auto groups = forest.groups();
        for (const auto& g : groups)
        {
            int leader = forest.leader(g[0]);
            vector<set<int>> colorset(9);
            for (int ind : g)
            {
                int i = ind / N;
                int j = ind % N;
                if (i-1 >= 0 && !forest.same(leader, index(i-1, j)))
                    colorset[colorMap[i-1][j]-1].insert(forest.leader(index(i-1, j)));
                if (i+1 < N && !forest.same(leader, index(i+1, j)))
                    colorset[colorMap[i+1][j]-1].insert(forest.leader(index(i+1, j)));
                if (j-1 >= 0 && !forest.same(leader, index(i, j-1)))
                    colorset[colorMap[i][j-1]-1].insert(forest.leader(index(i, j-1)));
                if (j+1 < N && !forest.same(leader, index(i, j+1)))
                    colorset[colorMap[i][j+1]-1].insert(forest.leader(index(i, j+1)));
            }
            for (int col = 0; col < 9; col++)
            {
                // cerr << "set" << colorset[col].size() << " " << "best" << bestScore << endl;
                int i = leader / N;
                int j = leader % N;
                double localScore = 1e6 * (double)colorset[col].size() + (abs(i - N/2) + abs(j - N/2));
                if (colorset[col].size() > 0 && bestScore < localScore)
                {
                    // this is the best query
                    // ここで中心をpreferするようにしませんか(このままだと端から揃うようになるので)
                    bestScore = localScore;
                    bestQ.y = i+1; bestQ.x = j+1; bestQ.c = col+1; // 1-indexed
                    bestColorset = colorset[col];
                    bestLeader = leader;

                    iscontinue = true;
                }
            }
        }
        // if (bestScore <= 1e4 - 1)
        if(!iscontinue) // 条件を，減る連結成分の数が1以上，にした　<= ここ!iscontinueでは？
        {
            // no more query
            return;
        }
        answer.push_back(bestQ);
        // update colorMap
        
        rep(i,N*N){
            if(forest.same(bestLeader,i)){
                colorMap[i/N][i%N] = bestQ.c;
            }
        }

        // bestColorset.insert(bestLeader);
        for (const auto& b : bestColorset)
        {
            forest.merge(b, index(bestQ.y-1, bestQ.x-1));
            /*
            for (const auto& g : groups)
            {
                if (forest.same(g[0], b))
                {
                    // 色更新
                    // ここなんですが，bestQ.c - 1 っておかしくないですか(colorMapのinedx)
                    // 1-indexedころしませんか？？？
                    if (forest.same(g[0], bestLeader)) for (int ind : g) colorMap[ind/N][ind%N] = bestQ.c;
                    forest.merge(b, index(bestQ.y-1, bestQ.x-1));
                    break;
                }
            }
            */
        }
        // cerr << "N of group after query: " << groups.size() << endl;
    }
}

int colorMapCalcScore[100][100];
int calc_score(vector<Query> &answer) {

    /* 
     * order is O(N^2 Q)
     */

    auto&& colorMap = colorMapCalcScore;
    memcpy(colorMap, initialColorMap, sizeof(colorMap));

    for (Query &qry : answer) {
        int Y = qry.y, X = qry.x, C = qry.c;
        int FirstC = colorMap[Y][X]; 

        // 変更後の色がもとの色と同じ
        if(C == FirstC) continue;

        queue<pair<int, int>> q;
        q.push(make_pair(Y, X));
        colorMap[Y][X] = C;
        
        vector<int> vy = {1, 0, -1, 0};
        vector<int> vx = {0, 1, 0, -1};
        while(!q.empty()){
            auto now = q.front(); q.pop();
            int nowY = now.first;
            int nowX = now.second;

            for(int k = 0; k < 4; k++){
                int nextY = nowY + vy[k];
                int nextX = nowX + vx[k];
                if(nextY < 0 || nextY >= N) continue;
                if(nextX < 0 || nextX >= N) continue;
                if(colorMap[nextY][nextX] == FirstC){
                    colorMap[nextY][nextX] = C;
                    q.push(make_pair(nextY, nextX));
                }
            }
        }
    }
    
    int point = 0;
    for(int i = 1; i <= K; i++){
        int cnt = 0;
        for(int y = 0; y < N; y++){
            for(int x = 0; x < N; x++){
                if(colorMap[y][x] == i) cnt++;
            }
        }
        point = max(point, cnt);
    }
    point *= 100;
    point -= answer.size();

    return point;
}