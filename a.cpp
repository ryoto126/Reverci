#include <bits/stdc++.h>
using namespace std;
#define REP(i, n) for (int i = 0; i < (n); i++)
#define FOR(i, m, n) for (int i = (m); i < (n); i++)
using pii = pair<int, int>;

const bool DEBUG = false;
const bool PLAY = true;
std::random_device rd;
std::mt19937 mt(rd());
uint64_t getrand() {
    uint64_t seed = time(NULL);
    mt.seed(seed);
    return mt();
}
//周期2^64-1の乱数
uint64_t xor64() {
    static uint64_t x = getrand();
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
}

const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int BOARD_SIZE = 8;
const int PLAYOUT_COUNT = 5000;
uint64_t firstHash, secondHash, playerHash;
// 0->'.', 1->'B', 2->'W'
uint64_t zobristHash[8][8][3];
unordered_map<uint64_t, pair<uint64_t, uint64_t>> mp;  // count,win
void init() {
    firstHash = xor64();
    secondHash = xor64();
    playerHash = firstHash ^ secondHash;
    REP(i, BOARD_SIZE)
    REP(j, BOARD_SIZE)
    REP(k, 3) { zobristHash[i][j][k] = xor64(); }
}

// int ord(char player){
//     if(player=='B')return 1;
//     else if(player=='W')return 2;
//     else return 0;
// }
int ord[150];
char opponent(char player) { return (player == 'W') ? 'B' : 'W'; }
bool isInside(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}
void print(string S) { cout << S << endl; }
struct Board {
    char board[BOARD_SIZE][BOARD_SIZE];
    char player;
    int last_x, last_y;
    uint64_t hash;
    vector<uint64_t> next_states;
    Board() {
        REP(i, BOARD_SIZE)
        REP(j, BOARD_SIZE)
        board[i][j] = '.';
        board[3][3] = 'W';
        board[3][4] = 'B';
        board[4][3] = 'B';
        board[4][4] = 'W';
        player = 'B';
        last_x = -1;
        last_y = -1;
        hash = 0;
        hash ^= firstHash;
        REP(i, BOARD_SIZE)
        REP(j, BOARD_SIZE) {
            if (board[i][j] == '.') hash ^= zobristHash[i][j][0];
            if (board[i][j] == 'B') hash ^= zobristHash[i][j][1];
            if (board[i][j] == 'W') hash ^= zobristHash[i][j][2];
        }
    }
    void print() {
        cout << "  ";
        REP(i, BOARD_SIZE)
        cout << i << " ";
        cout << endl;

        REP(i, BOARD_SIZE) {
            cout << i << " ";
            REP(j, BOARD_SIZE) {
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
        REP(i, BOARD_SIZE)
        REP(j, BOARD_SIZE) {
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
        REP(dir, 8) {
            int nx = i + dx[dir], ny = j + dy[dir];
            if (!isInside(nx, ny)) continue;
            if (board[nx][ny] != opponent(player)) continue;
            REP(k, BOARD_SIZE) {
                nx += dx[dir], ny += dy[dir];
                if (!isInside(nx, ny)) break;
                if (board[nx][ny] == '.') break;  //空きマスでもアウト
                if (board[nx][ny] == player) return true;
            }
        }
        return false;
    }
    // player_が石をおけるか
    bool canMove(char player_) {
        REP(i, BOARD_SIZE)
        REP(j, BOARD_SIZE) {
            if (board[i][j] != '.') continue;
            REP(dir, 8) {
                int nx = i + dx[dir], ny = j + dy[dir];
                if (!isInside(nx, ny)) continue;
                if (board[nx][ny] != opponent(player_)) continue;
                REP(k, BOARD_SIZE) {
                    nx += dx[dir], ny += dy[dir];
                    if (!isInside(nx, ny)) break;
                    if (board[nx][ny] == '.') break;  //空きマスでもアウト
                    if (board[nx][ny] == player_) return true;
                }
            }
        }
        return false;
    }
    //終了状態か
    bool isTerminal() { return !canMove('W') && !canMove('B'); }
    //(i,j)に石を置いて手番を渡す
    void put(int i, int j) {
        if (DEBUG) {
            cout << "put " << player << " on (" << i << "," << j << ")" << endl;
            // cout << board[i][j] << endl;
        }
        hash ^= zobristHash[i][j][0];
        board[i][j] = player;
        hash ^= zobristHash[i][j][ord[player]];
        REP(dir, 8) {
            int nx = i + dx[dir], ny = j + dy[dir];
            if (!isInside(nx, ny)) continue;
            if (board[nx][ny] != opponent(player)) continue;
            REP(k, BOARD_SIZE) {
                nx += dx[dir], ny += dy[dir];
                if (!isInside(nx, ny)) break;     //はみ出たらアウト
                if (board[nx][ny] == '.') break;  //空きマスでもアウト

                // i,jからnx,nyまで全部ひっくり返す
                if (board[nx][ny] == player) {
                    int x = i + dx[dir], y = j + dy[dir];
                    while (x != nx || y != ny) {
                        hash ^= zobristHash[x][y][ord[board[x][y]]];
                        board[x][y] = player;  //ひっくり返す
                        hash ^= zobristHash[x][y][ord[player]];
                        x += dx[dir];
                        y += dy[dir];
                    }
                    break;
                }
            }
        }
        last_x = i;
        last_y = j;
        player = opponent(player);
    }
    void pass() {
        player = opponent(player);
        hash ^= playerHash;
    }

    //ランダムに手番を進める
    void advance() {
        if (isTerminal()) return;
        if (!canMove(player)) {
            if (DEBUG) {
                cout << "cannot move!!!!!" << endl;
            }
            player = opponent(player);
            return;
        }
        vector<pii> cells;
        REP(i, BOARD_SIZE)
        REP(j, BOARD_SIZE) {
            if (board[i][j] != '.') continue;
            REP(dir, 8) {
                int nx = i + dx[dir], ny = j + dy[dir];
                if (!isInside(nx, ny)) continue;
                if (board[nx][ny] != opponent(player)) continue;
                REP(k, BOARD_SIZE) {
                    nx += dx[dir], ny += dy[dir];
                    if (!isInside(nx, ny)) break;
                    if (board[nx][ny] == '.') break;  //空きマスでもアウト
                    if (board[nx][ny] == player) {
                        cells.push_back({i, j});
                        dir = 8;
                        break;
                    }
                }
            }
        }
        int c_nxt = cells.size();
        assert(c_nxt > 0);
        int idx = xor64() % c_nxt;
        put(cells[idx].first, cells[idx].second);
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
unordered_map<uint64_t, Board> mp;
Board Montecarlo(Board b) {
    if (!b.canMove(b.player)) {
        b.player = opponent(b.player);
        b.print();
        return b;
    }
    //次の盤面から先手が勝つ確率
    double exp = (b.player == 'B') ? -2 : 2;

    Board ret_b;
    REP(i, BOARD_SIZE)
    REP(j, BOARD_SIZE) {
        if (!b.canPut(i, j)) continue;
        Board tmp_b = b;
        tmp_b.put(i, j);
        double res = playOut(tmp_b);
        if (b.player == 'B' && res > exp) {
            exp = res;
            ret_b = tmp_b;
        } else if (b.player == 'W' && res < exp) {
            exp = res;
            ret_b = tmp_b;
        }
    }
    if (PLAY) ret_b.print();
    return ret_b;
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

int move(Board b) {
    //もし終了状態なら勝敗を返して終了
    if (DEBUG) {
        b.print();
    }
    // b.print();
    if (b.isTerminal()) {
        int res = b.countDisks();
        if (DEBUG) {
            if (res > 0)
                print("First player won");
            else if (res == 0)
                print("Draw");
            else
                print("Second player won");
        }
        return res;
    }
    //もし進めないならパス
    if (!b.canMove(b.player)) {
        b.player = opponent(b.player);
    }
    Board next_b = Montecarlo(b);
    return move(next_b);
    // Board next_b = Montecarlo(b);
}

void init() {
    Board b;
    // b.print();
    ord['.'] = 0;
    ord['B'] = 1;
    ord['W'] = 2;
    while (!b.isTerminal()) {
        int res = move(b);
        if (DEBUG || 1) {
            if (res > 0)
                print("First player won");
            else if (res == 0)
                print("Draw");
            else
                print("Second player won");

            REP(__, 10)
            cout << endl;
        }
    }
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
            int x, y;
            cout << "input:";
            cin >> x >> y;
            if (!b.canPut(x, y)) continue;
            b.put(x, y);
            b.print();
            break;
        }
        b = Montecarlo(b);
    }
}
void second() {
    Board b;
    while (true) {
        if (b.isTerminal()) {
            b.finish();
            return;
        }
        b = Montecarlo(b);
        while (true) {
            if (b.isTerminal()) {
                b.finish();
                return;
            }
            if (!b.canMove(b.player)) {
                b.pass();
                break;
            }
            int x, y;
            cout << "input:";
            cin >> x >> y;
            if (!b.canPut(x, y)) continue;
            b.put(x, y);
            break;
        }
    }
}
void test() {
    Board b;
    while (true) {
        if (b.isTerminal()) {
            b.finish();
            return;
        }
        b = Montecarlo(b);
        while (true) {
            if (b.isTerminal()) {
                b.finish();
                return;
            }
            if (!b.canMove(b.player)) {
                b.pass();
                break;
            }
            b = Montecarlo(b);
            break;
        }
    }
}
const int MATCH_NUM = 100;
signed main() {
    init();
    string S;
    cin >> S;
    // cout << "\033[31m " << S << "\033[m" << endl;
    if (S == "first")
        first();
    else if (S == "second")
        second();
    else
        test();
}