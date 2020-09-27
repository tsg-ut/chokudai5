#include<bits/stdc++.h>

template<class T, class S>
void guchol(unsigned long N, T mp, S ans){
    vector<unsigned long> cnt(10);
    for(unsigned long i{1}; i <= N; ++i){
        string S;
        cin >> S;
        for(unsigned long j{1}; j <= N; ++j)++cnt[mp[i][j] = S[j - 1] - 48];
    }
    const auto to{static_cast<unsigned long>(max_element(begin(cnt), end(cnt)) - begin(cnt))};
    vector<tuple<unsigned long, unsigned long, unsigned long>> ans;
    const auto& fill = [&mp](const auto& f, unsigned long x, unsigned long y, unsigned long c, unsigned long d) -> void {
        for(const auto& [i, j] : vector<pair<unsigned long, unsigned long>>{{1UL, 0UL}, {~0UL, 0UL}, {0UL, 1UL}, {0UL, ~0UL}})if(mp[x + i][y + j] == d){
            mp[x + i][y + j] = c;
            f(f, x + i, y + j, c, d);
        }
    };
    for(unsigned long i{1}; i <= N; ++i){
        for(unsigned long j{1}; j <= N; ++j){
            if(mp[i][j] != to){
                ans.emplace_back(i, j, to);
                fill(fill, i, j, to, mp[i][j]);
            }
        }
    }
    cout << size(ans) << endl;
    for(const auto& [x, y, c] : ans)cout << x << " " << y << " " << c << endl;
    return 0;
}
