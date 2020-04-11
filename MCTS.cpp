#include <bits/stdc++.h>
using namespace std;
#define REP8(i, n) for (int_fast8_t i = 0; i < (n); i++)
#define FOR8(i, m, n) for (int_fast8_t i = (m); i < (n); i++)
#define REP(i, n) for (int_fast16_t i = 0; i < (n); i++)
#define FOR(i, m, n) for (int_fast16_t i = (m); i < (n); i++)
#define isInside(x, y) ((x) >= 0 && (x) < 8 && (y) >= 0 && (y) < 8)
using pii = pair<int, int>;

bool DEBUG = true;
bool PLAY = true;
constexpr double TIME_LIMIT = 500;
std::random_device rd;
std::mt19937 mt(rd());

uint_fast64_t getrand() {
    uint_fast64_t seed = time(NULL);
    mt.seed(seed);
    return mt();
}
//周期2^64-1の乱数
uint_fast64_t xor64() {
    static uint_fast64_t x = getrand();
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
}
double getrand01() { return (double)xor64() / ULLONG_MAX; }

// map<char, int> ord;
uint_fast8_t ord[150];
constexpr int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
constexpr int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
constexpr int_fast16_t BOARD_SCORE[8][8] = {
    {100, -40, 20, 5, 5, 20, -40, 100},    //
    {-40, -80, -1, -1, -1, -1, -80, -40},  //
    {20, -1, 5, 1, 1, 5, -1, 20},          //
    {5, -1, 1, 0, 0, 1, -1, 5},            //
    {5, -1, 1, 0, 0, 1, -1, 5},            //
    {20, -1, 5, 1, 1, 5, -1, 20},          //
    {-40, -80, -1, -1, -1, -1, -80, -40},  //
    {100, -40, 20, 5, 5, 20, -40, 100}     //
};
constexpr uint_fast8_t BOARD_SIZE = 8;
constexpr uint_fast16_t PLAYOUT_COUNT = 1000;
uint_fast64_t firstHash, secondHash, playerHash;
// 0->'.', 1->'B', 2->'W'
uint_fast64_t zobristHash[8][8][3];
char disk[3] = {'B', 'W', '.'};
unordered_map<uint_fast64_t, pair<uint_fast64_t, uint_fast64_t>> hash2score;  // count,win
void init() {
    firstHash = xor64();
    secondHash = xor64();
    playerHash = firstHash ^ secondHash;
    REP8(i, BOARD_SIZE)
    REP8(j, BOARD_SIZE)
    REP8(k, 3) {
        zobristHash[i][j][k] = xor64();
        // cout << i << " " << j << " " << k << " :" << zobristHash[i][j][k] << endl;
    }
    ord['B'] = 0, ord['W'] = 1, ord['.'] = 2;
}

// int ord(char player){
//     if(player=='B')return 1;
//     else if(player=='W')return 2;
//     else return 0;
// }
const double CS = 1;
char opponent(char player) { return (player == 'W') ? 'B' : 'W'; }
// bool isInside(int x, int y) { return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE; }
void print(string S) { cout << S << endl; }
struct Board {
    char board[BOARD_SIZE][BOARD_SIZE];
    bool player;
    int last_x, last_y;
    uint_fast64_t hash;
    vector<uint_fast64_t> next_states;
    Board() {
        REP8(i, BOARD_SIZE)
        REP8(j, BOARD_SIZE)
        board[i][j] = '.';
        board[3][3] = 'W';
        board[3][4] = 'B';
        board[4][3] = 'B';
        board[4][4] = 'W';
        player = 0;
        last_x = -1;
        last_y = -1;
        hash = 0;
        hash ^= firstHash;
        REP8(i, BOARD_SIZE)
        REP8(j, BOARD_SIZE) {
            if (board[i][j] == 'B') hash ^= zobristHash[i][j][0];
            if (board[i][j] == 'W') hash ^= zobristHash[i][j][1];
            if (board[i][j] == '.') hash ^= zobristHash[i][j][2];
        }
    }
    void print() {
        cout << "  ";
        REP8(i, BOARD_SIZE)
        cout << (int)i << " ";
        cout << endl;

        REP8(i, BOARD_SIZE) {
            cout << (int)i << " ";
            REP8(j, BOARD_SIZE) {
                bool is_last = false;
                if (i == last_x && j == last_y) is_last = true;
                string s;
                if (is_last) s += "\033[31m";
                s += board[i][j];
                if (is_last) s += "\033[m";
                cout << s << " ";
            }

            cout << endl;
        }
        cout << endl;
    }
    //黒 - 白
    int countDisks() {
        int ret = 0;
        REP8(i, BOARD_SIZE)
        REP8(j, BOARD_SIZE) {
            if (board[i][j] == 'B')
                ret++;
            else if (board[i][j] == 'W')
                ret--;
        }
        return ret;
    }
    //(i,j)に石をおけるか
    bool canPut(int i, int j) {
        if (board[i][j] != '.') return false;
        REP8(dir, 8) {
            int nx = i + dx[dir], ny = j + dy[dir];
            if (!isInside(nx, ny)) continue;
            if (board[nx][ny] != disk[!player]) continue;
            REP8(k, BOARD_SIZE) {
                nx += dx[dir], ny += dy[dir];
                if (!isInside(nx, ny)) break;
                if (board[nx][ny] == '.') break;  //空きマスでもアウト
                if (board[nx][ny] == disk[player]) return true;
            }
        }
        return false;
    }
    // player_がそのマスに石をおけるか
    bool canPut(int i, int j, bool player_) {
        if (board[i][j] != '.') return false;
        REP8(dir, 8) {
            int nx = i + dx[dir], ny = j + dy[dir];
            if (!isInside(nx, ny)) continue;
            if (board[nx][ny] != disk[!player_]) continue;
            REP8(k, BOARD_SIZE) {
                nx += dx[dir], ny += dy[dir];
                if (!isInside(nx, ny)) break;
                if (board[nx][ny] == '.') break;  //空きマスでもアウト
                if (board[nx][ny] == disk[player_]) return true;
            }
        }
        return false;
    }
    // player_が石をおけるか
    bool canMove(bool player_) {
        REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
            if (canPut(i, j, player_)) return true;
        }
        return false;
    }
    //終了状態か
    bool isTerminal() { return !canMove(0) && !canMove(1); }

    //現在の盤面のハッシュ値を返す
    uint_fast64_t calcHash() {
        uint_fast64_t ret = 0;
        if (player == 0)
            ret ^= firstHash;
        else
            ret ^= secondHash;
        REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) { ret ^= zobristHash[i][j][ord[board[i][j]]]; }
        return ret;
    }
    //(i,j)に石を置いて手番を渡す

    void put(int i, int j) {
        // if (DEBUG) {
        //     cout << "put " << player << " on (" << i << "," << j << ")" << endl;
        //     // cout << board[i][j] << endl;
        // }
        hash ^= zobristHash[i][j][2];
        board[i][j] = disk[player];
        // cout << "ord[" << player << "]:" << ord[player] << endl;
        hash ^= zobristHash[i][j][player];
        REP8(dir, 8) {
            int nx = i + dx[dir], ny = j + dy[dir];
            if (!isInside(nx, ny)) continue;
            if (board[nx][ny] != disk[!player]) continue;
            REP8(k, BOARD_SIZE) {
                nx += dx[dir], ny += dy[dir];
                if (!isInside(nx, ny)) break;     //はみ出たらアウト
                if (board[nx][ny] == '.') break;  //空きマスでもアウト

                // i,jからnx,nyまで全部ひっくり返す
                if (board[nx][ny] == disk[player]) {
                    int x = i + dx[dir], y = j + dy[dir];
                    while (x != nx || y != ny) {
                        hash ^= zobristHash[x][y][ord[board[x][y]]];
                        board[x][y] = disk[player];  //ひっくり返す
                        hash ^= zobristHash[x][y][player];
                        x += dx[dir];
                        y += dy[dir];
                    }
                    break;
                }
            }
        }
        last_x = i;
        last_y = j;
        player = !player;
        hash ^= playerHash;
        // hash = calcHash();
    }
    void pass() {
        player = !player;
        hash ^= playerHash;
        // hash = calcHash();
    }
    //黒の確定石の数-白の確定石
    int_fast8_t calc_confirmed_stone() {
        if (board[0][0] == '.' && board[0][7] == '.' && board[7][0] == '.' && board[7][7] == '.') return 0;
        uint_fast8_t side[28] = {};
        uint_fast8_t confirmed[28] = {};
        REP8(i, 28) confirmed[i] = 2;
        REP8(j, 8) {
            if (board[0][j] == 'B')
                side[j] = 0;
            else if (board[0][j] == 'W')
                side[j] = 1;
            else
                side[j] = 2;

            if (board[7][j] == 'B')
                side[21 - j] = 0;
            else if (board[7][j] == 'W')
                side[21 - j] = 1;
            else
                side[21 - j] = 2;
        }
        FOR(i, 1, 7) {
            if (board[i][0] == 'B')
                side[28 - i] = 0;
            else if (board[i][0] == 'W')
                side[28 - i] = 1;
            else
                side[28 - i] = 2;

            if (board[i][7] == 'B')
                side[7 + i] = 0;
            else if (board[i][7] == 'W')
                side[7 + i] = 1;
            else
                side[7 + i] = 2;
        }
        uint_fast8_t space[4] = {};
        REP(i, 28) {
            if (side[i] == 2) {
                if (i <= 7) space[0] = true;
                if (7 <= i && i <= 14) space[1] = true;
                if (14 <= i && i <= 21) space[2] = true;
                if ((21 <= i && i <= 27) || i == 0) space[3] = true;
            }
        }
        //左上隅
        if (side[0] == 0) {
            uint_fast8_t pos = 0;
            while (pos <= 7 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos++;
            }
            pos = 27;
            while (pos >= 21 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos--;
            }
        } else if (side[0] == 1) {
            uint_fast8_t pos = 0;
            while (pos <= 7 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos++;
            }
            pos = 27;
            while (pos >= 21 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos--;
            }
        }
        //右上隅
        if (side[7] == 0) {
            uint_fast8_t pos = 7;
            while (pos <= 14 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos++;
            }
            pos = 6;
            while (pos > 0 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos--;
            }
        } else if (side[7] == 1) {
            uint_fast8_t pos = 7;
            while (pos <= 14 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos++;
            }
            pos = 6;
            while (pos > 0 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos--;
            }
        }

        //右下隅
        if (side[14] == 0) {
            uint8_t pos = 14;
            while (pos > 7 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos--;
            }
            pos = 15;
            while (pos < 21 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos++;
            }
        } else if (side[14] == 1) {
            uint8_t pos = 14;
            while (pos > 7 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos--;
            }
            pos = 15;
            while (pos < 21 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos++;
            }
        }

        //左下隅
        if (side[21] == 0) {
            uint8_t pos = 21;
            while (pos < 28 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos++;
            }
            pos = 20;
            while (pos > 14 && side[pos] == 0) {
                confirmed[pos] = 0;
                pos--;
            }
        } else if (side[21] == 1) {
            uint8_t pos = 21;
            while (pos < 28 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos++;
            }
            pos = 20;
            while (pos > 14 && side[pos] == 1) {
                confirmed[pos] = 1;
                pos--;
            }
        }

        //上辺
        if (!space[0]) {
            REP8(j, 8) { confirmed[j] = side[j]; }
        }
        if (!space[1]) {
            FOR(i, 7, 14) { confirmed[i] = side[i]; }
        }
        if (!space[2]) {
            FOR(i, 14, 21) confirmed[i] = side[i];
        }
        if (!space[3]) {
            FOR(i, 21, 28) confirmed[i] = side[i];
        }
        int_fast8_t ret = 0;
        REP(i, 28) {
            if (confirmed[i] == 0)
                ret++;
            else if (confirmed[i] == 1)
                ret--;
        }
        return ret;
    }
    //盤面の評価値
    int_fast16_t board_score() {
        int_fast16_t ret = 0;
        REP8(i, 8) REP8(j, 8) {
            if (board[i][j] == 'B')
                ret += BOARD_SCORE[i][j];
            else if (board[i][j] == 'W')
                ret -= BOARD_SCORE[i][j];
        }
        return ret;
    }
    //開放度
    int_fast8_t calc_openness() {
        int_fast8_t ret = 0;
        REP8(i, 8) REP8(j, 8) {
            if (canPut(i, j, 0)) ret++;
            if (canPut(i, j, 1)) ret--;
        }
        return ret;
    }
    //全体の評価値
    double eval() {
        // double ret = 0;
        return calc_confirmed_stone() * 100 + calc_openness() * 60;
    }
    //ランダムに手番を進める
    void advance() {
        if (isTerminal()) return;
        if (!canMove(player)) {
            // if (DEBUG) {
            //     print();
            //     cout << disk[player] << " cannot move!!!!!" << endl;
            // }
            player = !player;
            hash ^= playerHash;
            return;
        }
        vector<pii> cells;
        REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
            if (board[i][j] != '.') continue;
            if (canPut(i, j)) {
                cells.push_back({i, j});
            }
        }
        int c_nxt = cells.size();
        assert(c_nxt > 0);
        int idx = xor64() % c_nxt;
        put(cells[idx].first, cells[idx].second);
    }
    //評価値がもっとも高い手を選んで進める
    void advance_eval() {
        if (isTerminal()) return;
        if (!canMove(player)) {
            // if (DEBUG) {
            //     print();
            //     cout << disk[player] << " cannot move!!!!!" << endl;
            // }
            player = !player;
            hash ^= playerHash;
            return;
        }
        vector<pair<double, pii>> cells;

        REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
            if (board[i][j] != '.') continue;
            if (canPut(i, j)) {
                Board next_b = *this;
                next_b.put(i, j);
                double res = next_b.eval();
                cells.push_back({res, {i, j}});
            }
        }
        sort(cells.begin(), cells.end());
        if (player == 1) {
            put(cells[0].second.first, cells[0].second.second);
        } else {
            int n = cells.size();
            put(cells[n - 1].second.first, cells[n - 1].second.second);
        }
    }
    //一回プレイアウト
    int playout() {
        Board tmp_b = *this;
        while (!tmp_b.isTerminal()) {
            tmp_b.advance();
        }
        int res = tmp_b.countDisks();
        if (res > 0)
            return 1;
        else if (res == 0)
            return 0;
        else
            return -1;
    }
    int playout_greedy() {
        Board tmp_b = *this;
        while (!tmp_b.isTerminal()) {
            if (xor64() % 20 == 0)  // 20回に1度は完全ランダム
                tmp_b.advance();
            else
                tmp_b.advance_eval();
        }
        int res = tmp_b.countDisks();
        if (res > 0)
            return 1;
        else if (res == 0)
            return 0;
        else
            return -1;
    }
    void finish() {
        int ret = countDisks();
        if (ret > 0) {
            cout << "first player won" << endl;
        } else if (ret < 0) {
            cout << "second player won" << endl;
        } else {
            cout << "draw" << endl;
        }
        return;
    }
};

/*----------prototype declaration----------*/
Board Montecarlo(Board b);
double playOut(Board b);
int move(Board b);
void init();
/*----------prototype declaration----------*/
const double C = 1.41421;
double uct_score(int win_count, int visited_count, int N, char player) {
    assert(N > 0);
    double win_rate = (double)win_count / visited_count;
    if (player == 1) win_rate *= -1;
    double bias = sqrt(2 * log(N) / visited_count);
    return win_rate + bias;
}
const int threshold_vis = 50;
//モンテカルロ木探索
Board MCTS(Board b) {
    if (!b.canMove(b.player)) {
        b.pass();
        return b;
    }
    clock_t start_t = clock();
    //ハッシュ値→盤面
    unordered_map<uint_fast64_t, Board> hash2board;
    unordered_map<uint_fast64_t, vector<uint_fast64_t>> G;
    unordered_map<uint_fast64_t, int> visited_count;
    unordered_map<uint_fast64_t, int> win_count;  //勝ち:+1 負け:-1
    // b.hash = b.calcHash();
    uint_fast64_t root_h = b.hash;
    hash2board[root_h] = b;
    int t = 0;
    int update_cnt = 0;
    REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
        if (b.canPut(i, j)) {
            Board next_b = b;
            next_b.put(i, j);
            hash2board[next_b.hash] = next_b;
            int res = next_b.playout();
            t++;
            update_cnt++;
            G[root_h].push_back(next_b.hash);
            visited_count[next_b.hash]++;
            visited_count[root_h]++;
            if (res == 1) win_count[next_b.hash] += res;
        }
    }
    //制限時間までくりかえす

    while (static_cast<double>(clock() - start_t) / CLOCKS_PER_SEC * 1000.0 < TIME_LIMIT) {
        vector<uint_fast64_t> path;
        uint_fast64_t pos_h = root_h;
        path.push_back(pos_h);
        while (true) {
            int N = visited_count[root_h];
            if (G[pos_h].size() == 0) break;
            double max_score = -1e9;
            uint_fast64_t argmax_h = 0;
            for (auto next_h : G[pos_h]) {
                double score = uct_score(win_count[next_h], visited_count[next_h], N, b.player);
                if (score > max_score) {
                    max_score = score;
                    argmax_h = next_h;
                }
            }
            path.push_back(argmax_h);
            pos_h = argmax_h;
        }
        //葉ノードへの訪問回数が閾値を超えているならば、すべての遷移先(合法手)から一度ずつプレイアウトを行う
        if (visited_count[pos_h] > threshold_vis) {
            Board leaf_b = hash2board[pos_h];
            REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
                if (leaf_b.canPut(i, j)) {
                    Board next_b = leaf_b;
                    next_b.put(i, j);
                    hash2board[next_b.hash] = next_b;
                    int res = next_b.playout();
                    t++;
                    // t++;
                    G[pos_h].push_back(next_b.hash);
                    visited_count[next_b.hash]++;
                    if (res == 1) win_count[next_b.hash] += res;
                }
            }
        }
        //そうでなければ、葉ノードから1回プレイアウトを行ってパス上のすべてのノードのスコアを更新する
        else {
            Board leaf_b = hash2board[pos_h];
            int res = leaf_b.playout();
            t++;
            update_cnt++;
            // path.push_back(pos_h);
            for (auto h : path) {
                visited_count[h]++;
                if (res == 1) win_count[h] += res;
            }
        }
    }
    int max_vis = 0;
    uint_fast64_t next_h = 0;
    for (auto h : G[root_h]) {
        cout << "visited count:" << visited_count[h] << endl;
        cout << "winning rate:" << (double)win_count[h] / visited_count[h] << endl;
        if (visited_count[h] > max_vis) {
            max_vis = visited_count[h];
            next_h = h;
        }
    }
    Board ret = hash2board[next_h];
    cout << "number of updates:" << update_cnt << endl;
    cout << "number of playouts" << t << endl;
    if (PLAY) ret.print();
    return ret;
}

Board MCTS_eval(Board b) {
    if (!b.canMove(b.player)) {
        b.pass();
        return b;
    }
    clock_t start_t = clock();
    //ハッシュ値→盤面
    unordered_map<uint_fast64_t, Board> hash2board;
    unordered_map<uint_fast64_t, vector<uint_fast64_t>> G;
    unordered_map<uint_fast64_t, int> visited_count;
    unordered_map<uint_fast64_t, int> win_count;  //勝ち:+1 負け:-1
    // b.hash = b.calcHash();
    uint_fast64_t root_h = b.hash;
    hash2board[root_h] = b;
    int t = 0;
    int update_cnt = 0;
    REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
        if (b.canPut(i, j)) {
            Board next_b = b;
            next_b.put(i, j);
            hash2board[next_b.hash] = next_b;
            int res = next_b.playout_greedy();
            t++;
            update_cnt++;
            G[root_h].push_back(next_b.hash);
            visited_count[next_b.hash]++;
            visited_count[root_h]++;
            if (res == 1) win_count[next_b.hash] += res;
        }
    }
    //制限時間までくりかえす

    while (static_cast<double>(clock() - start_t) / CLOCKS_PER_SEC * 1000.0 < TIME_LIMIT) {
        vector<uint_fast64_t> path;
        uint_fast64_t pos_h = root_h;
        path.push_back(pos_h);
        while (true) {
            int N = visited_count[root_h];
            if (G[pos_h].size() == 0) break;
            double max_score = -1e9;
            uint_fast64_t argmax_h = 0;
            for (auto next_h : G[pos_h]) {
                double score = uct_score(win_count[next_h], visited_count[next_h], N, b.player);
                if (score > max_score) {
                    max_score = score;
                    argmax_h = next_h;
                }
            }
            path.push_back(argmax_h);
            pos_h = argmax_h;
        }
        //葉ノードへの訪問回数が閾値を超えているならば、すべての遷移先(合法手)から一度ずつプレイアウトを行う
        if (visited_count[pos_h] > threshold_vis) {
            Board leaf_b = hash2board[pos_h];
            REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
                if (leaf_b.canPut(i, j)) {
                    Board next_b = leaf_b;
                    next_b.put(i, j);
                    hash2board[next_b.hash] = next_b;
                    int res = next_b.playout_greedy();
                    t++;
                    // t++;
                    G[pos_h].push_back(next_b.hash);
                    visited_count[next_b.hash]++;
                    if (res == 1) win_count[next_b.hash] += res;
                }
            }
        }
        //そうでなければ、葉ノードから1回プレイアウトを行ってパス上のすべてのノードのスコアを更新する
        else {
            Board leaf_b = hash2board[pos_h];
            int res = leaf_b.playout_greedy();
            t++;
            update_cnt++;
            // path.push_back(pos_h);
            for (auto h : path) {
                visited_count[h]++;
                if (res == 1) win_count[h] += res;
            }
        }
    }
    int max_vis = 0;
    uint_fast64_t next_h = 0;
    for (auto h : G[root_h]) {
        if (DEBUG) {
            cout << "visited count:" << visited_count[h] << endl;
            cout << "winning rate:" << (double)win_count[h] / visited_count[h] << endl;
        }

        if (visited_count[h] > max_vis) {
            max_vis = visited_count[h];
            next_h = h;
        }
    }
    Board ret = hash2board[next_h];
    cout << "number of updates:" << update_cnt << endl;
    cout << "number of playouts" << t << endl;
    if (PLAY) ret.print();
    return ret;
}
Board Montecarlo(Board b) {
    if (!b.canMove(b.player)) {
        b.pass();
        b.print();
        return b;
    }
    //次の盤面から先手が勝つ確率
    double exp = (b.player == 0) ? -2 : 2;

    Board ret_b;
    REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
        if (!b.canPut(i, j)) continue;
        Board tmp_b = b;
        tmp_b.put(i, j);
        double res = playOut(tmp_b);
        if (b.player == 0 && res > exp) {
            exp = res;
            ret_b = tmp_b;
        } else if (b.player == 1 && res < exp) {
            exp = res;
            ret_b = tmp_b;
        }
    }
    if (PLAY) ret_b.print();
    return ret_b;
}
Board greedy(Board b) {
    if (!b.canMove(b.player)) {
        b.pass();
        return b;
    }
    b.advance_eval();
    if (PLAY) b.print();
    if (DEBUG) {
        cout << "confirmed stone:" << (int)b.calc_confirmed_stone() << endl;
    }
    return b;
}

//盤面bからはじめてランダムに手を進めた時、先手が勝つ確率
double playOut(Board b) {
    double ret = 0;
    REP(_, PLAYOUT_COUNT) {
        Board tmp_b = b;
        while (!tmp_b.isTerminal()) {
            tmp_b.advance();
        }
        int cnt_B = tmp_b.countDisks();
        if (cnt_B > 0)
            ret += 1;
        else if (cnt_B < 0)
            ret -= 1;
    }
    return ret / (double)PLAYOUT_COUNT;
}

void first() {
    Board b;
    b.print();
    while (true) {
        if (b.isTerminal()) {
            b.finish();
            return;
        }
        while (true) {
            if (b.isTerminal()) {
                b.finish();
                return;
            }
            if (!b.canMove(b.player)) {
                b.pass();
                break;
            }
            cout << "input: ";
            string S;
            getline(cin, S);
            if (S.size() == 3 && ('0' <= S[0] && S[0] <= '9') && ('0' <= S[2] && S[2] <= '9')) {
                int x = S[0] - '0', y = S[2] - '0';
                if (!b.canPut(x, y)) continue;
                b.put(x, y);
                b.print();
                break;
            } else {
                cout << "invalid input" << endl;
                continue;
            }
        }
        b = MCTS_eval(b);
    }
}
void second() {
    Board b;
    while (true) {
        if (b.isTerminal()) {
            b.finish();
            return;
        }
        b = MCTS_eval(b);
        while (true) {
            if (b.isTerminal()) {
                b.finish();
                return;
            }
            if (!b.canMove(b.player)) {
                b.pass();
                break;
            }
            cout << "input:";
            string S;
            getline(cin, S);
            if (S.size() == 3 && ('0' <= S[0] && S[0] <= '9') && ('0' <= S[2] && S[2] <= '9')) {
                int x = S[0] - '0', y = S[2] - '0';
                if (!b.canPut(x, y)) continue;
                b.put(x, y);
                b.print();
            } else {
                cout << "invalid input" << endl;
                continue;
            }
            break;
        }
    }
}
void test() {
    Board b;
    cout << "first:MCTS second:MCTS with greedy" << endl;
    while (true) {
        if (b.isTerminal()) {
            b.finish();
            return;
        }
        if (!b.canMove(b.player))
            b.pass();
        else {
            if (b.player == 0)
                b = MCTS(b);
            else
                b = MCTS_eval(b);
        }

        while (true) {
            if (b.isTerminal()) {
                b.finish();
                return;
            }
            if (!b.canMove(b.player)) {
                b.pass();
                break;
            }
            if (b.player == 0)
                b = MCTS(b);
            else
                b = MCTS_eval(b);
            break;
        }
    }
}
const int MATCH_NUM = 30;
void test_rand() {
    REP(i, 100) { cout << getrand01() << endl; }
}
void batch_test() {
    PLAY = false;
    DEBUG = false;
    int win_c = 0;
    int lose_c = 0;
    REP(_, MATCH_NUM) {
        Board b;
        cout << "first:MCTS with greedy second:Montecarlo" << endl;
        while (true) {
            if (b.isTerminal()) {
                b.finish();
                break;
            }
            if (!b.canMove(b.player))
                b.pass();
            else {
                if (b.player == 0)
                    b = MCTS_eval(b);
                else
                    b = Montecarlo(b);
            }

            while (true) {
                if (b.isTerminal()) {
                    b.finish();
                    break;
                }
                if (!b.canMove(b.player)) {
                    b.pass();
                    break;
                }
                if (b.player == 0)
                    b = MCTS_eval(b);
                else
                    b = Montecarlo(b);
                break;
            }
        }
        int ret = b.countDisks();
        b.print();
        if (ret > 0)
            win_c++;
        else if (ret < 0)
            lose_c++;
    }

    cout << "win:" << win_c << endl;
    cout << "lose:" << lose_c << endl;
}
signed main() {
    init();
    string S;
    cin >> S;
    // cout << "\033[31m " << S << "\033[m" << endl;
    if (S == "first")
        first();
    else if (S == "second")
        second();
    else if (S == "test")
        test();
    else
        batch_test();
}