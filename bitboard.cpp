#include <random>
#include <unordered_map>

#include "bits/stdc++.h"
#ifdef _MSC_VER
#include <intrin.h>
#define __builtin_popcount __popcnt
#endif
using namespace std;
#define REP8(i, n) for (int_fast8_t i = 0; i < (n); i++)
#define FOR8(i, m, n) for (int_fast8_t i = (m); i < (n); i++)
#define REP(i, n) for (int_fast16_t i = 0; i < (n); i++)
#define FOR(i, m, n) for (int_fast16_t i = (m); i < (n); i++)
#define isInside(x, y) ((x) >= 0 && (x) < 8 && (y) >= 0 && (y) < 8)
#define BIT(i, j) (1ULL << ((i)*8 + j))
using pii = pair<int_fast8_t, int_fast8_t>;

#define SIZE 8

bool DEBUG = true;
bool PLAY = false;
bool TEST = true;
bool DISPLAY = true;
double TIME_LIMIT = 500;
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
int_fast8_t ord[150];
constexpr int_fast8_t dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
constexpr int_fast8_t dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
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
constexpr int_fast8_t BOARD_SIZE = 8;
constexpr int_fast16_t PLAYOUT_COUNT = 1000;
uint_fast64_t firstHash, secondHash, playerHash;
// 0->'.', 1->'B', 2->'W'
uint_fast64_t zobristHash[8][8][3];
char disk[3] = {'B', 'W', '.'};
unordered_map<uint_fast64_t, pair<uint_fast64_t, uint_fast64_t>> hash2score;  // count,win
// struct BitBoard : public bitset<64> {
//     bitset<64>::reference operator()(int_fast8_t i, int_fast8_t j) { return (*this)[i * SIZE + j]; }
//     void print() {
//         REP(i, 8) {
//             REP(j, 8) { cout << (*this)(i, j) << " "; }
//             cout << endl;
//         }
//     }
//     BitBoard() {}
//     BitBoard(unsigned long long x) { *(this) = bitset<64>(x); }
//     bitset<64> transfer() { return (*this) & bitset(0x7f7f7f7f7f7f7f7f); }
// };

using BitBoard = uint64_t;
BitBoard set(BitBoard b, int i, int j) { return b | (1ULL << (i * SIZE + j)); }
BitBoard reset(BitBoard b, int i, int j) { return b & ~(1ULL << (i * SIZE + j)); }
bool get(BitBoard b, int i, int j) { return b & (1ULL << (i * SIZE + j)); }
BitBoard transfer(BitBoard b) { return (b >> 1) & (0x7f7f7f7f7f7f7f7fULL); }
void printBIT(BitBoard b_) {
    bitset<64> b(b_);
    REP(i, 8) {
        REP(j, 8) { cout << b[i * SIZE + j] << " "; }
        cout << endl;
    }
}
BitBoard makeLegalBoard(BitBoard myPos, BitBoard opPos) {
    BitBoard legalBoard;
    BitBoard horizontalWatchBoard = opPos & 0x7e7e7e7e7e7e7e7eULL;
    BitBoard verticalWatchBoard = opPos & 0x00FFFFFFFFFFFF00ULL;
    BitBoard allSideWatchBoard = opPos & 0x007e7e7e7e7e7e00ULL;
    BitBoard blankBoard = ~(myPos | opPos);
    BitBoard tmp = 0;
    //左
    tmp = horizontalWatchBoard & (myPos << 1);
    tmp |= horizontalWatchBoard & (tmp << 1);
    tmp |= horizontalWatchBoard & (tmp << 1);
    tmp |= horizontalWatchBoard & (tmp << 1);
    tmp |= horizontalWatchBoard & (tmp << 1);
    tmp |= horizontalWatchBoard & (tmp << 1);
    legalBoard = blankBoard & (tmp << 1);

    //右
    tmp = horizontalWatchBoard & (myPos >> 1);
    tmp |= horizontalWatchBoard & (tmp >> 1);
    tmp |= horizontalWatchBoard & (tmp >> 1);
    tmp |= horizontalWatchBoard & (tmp >> 1);
    tmp |= horizontalWatchBoard & (tmp >> 1);
    tmp |= horizontalWatchBoard & (tmp >> 1);
    legalBoard |= blankBoard & (tmp >> 1);

    //上
    tmp = verticalWatchBoard & (myPos << 8);
    tmp |= verticalWatchBoard & (tmp << 8);
    tmp |= verticalWatchBoard & (tmp << 8);
    tmp |= verticalWatchBoard & (tmp << 8);
    tmp |= verticalWatchBoard & (tmp << 8);
    tmp |= verticalWatchBoard & (tmp << 8);
    legalBoard |= blankBoard & (tmp << 8);

    //下
    tmp = verticalWatchBoard & (myPos >> 8);
    tmp |= verticalWatchBoard & (tmp >> 8);
    tmp |= verticalWatchBoard & (tmp >> 8);
    tmp |= verticalWatchBoard & (tmp >> 8);
    tmp |= verticalWatchBoard & (tmp >> 8);
    tmp |= verticalWatchBoard & (tmp >> 8);
    legalBoard |= blankBoard & (tmp >> 8);

    //右斜め上
    tmp = allSideWatchBoard & (myPos << 7);
    tmp |= allSideWatchBoard & (tmp << 7);
    tmp |= allSideWatchBoard & (tmp << 7);
    tmp |= allSideWatchBoard & (tmp << 7);
    tmp |= allSideWatchBoard & (tmp << 7);
    tmp |= allSideWatchBoard & (tmp << 7);
    legalBoard |= blankBoard & (tmp << 7);

    //左斜め上
    tmp = allSideWatchBoard & (myPos << 9);
    tmp |= allSideWatchBoard & (tmp << 9);
    tmp |= allSideWatchBoard & (tmp << 9);
    tmp |= allSideWatchBoard & (tmp << 9);
    tmp |= allSideWatchBoard & (tmp << 9);
    tmp |= allSideWatchBoard & (tmp << 9);
    legalBoard |= blankBoard & (tmp << 9);

    //右斜め下
    tmp = allSideWatchBoard & (myPos >> 9);
    tmp |= allSideWatchBoard & (tmp >> 9);
    tmp |= allSideWatchBoard & (tmp >> 9);
    tmp |= allSideWatchBoard & (tmp >> 9);
    tmp |= allSideWatchBoard & (tmp >> 9);
    tmp |= allSideWatchBoard & (tmp >> 9);
    legalBoard |= blankBoard & (tmp >> 9);

    //左斜め下
    tmp = allSideWatchBoard & (myPos >> 7);
    tmp |= allSideWatchBoard & (tmp >> 7);
    tmp |= allSideWatchBoard & (tmp >> 7);
    tmp |= allSideWatchBoard & (tmp >> 7);
    tmp |= allSideWatchBoard & (tmp >> 7);
    tmp |= allSideWatchBoard & (tmp >> 7);
    legalBoard |= blankBoard & (tmp >> 7);

    return legalBoard;
}
BitBoard transfer(BitBoard target, int k) {
    if (k == 0) {
        return (target << 8) & 0xffffffffffffff00ULL;
    }
    if (k == 1) {
        return (target << 7) & 0x7f7f7f7f7f7f7f00ULL;
    }
    if (k == 2) {
        return (target >> 1) & 0x7f7f7f7f7f7f7f7fULL;
    }
    if (k == 3) {
        return (target >> 9) & 0x007f7f7f7f7f7f7fULL;
    }
    if (k == 4) {
        return (target >> 8) & 0x00ffffffffffffffULL;
    }
    if (k == 5) {
        return (target >> 7) & 0x00fefefefefefefeULL;
    }
    if (k == 6) {
        return (target << 1) & 0xfefefefefefefefeULL;
    }
    if (k == 7) {
        return (target << 9) & 0xfefefefefefefe00ULL;
    }
    return 0;
}

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
struct Board {
    BitBoard pos[2];
    bool player;
    int_fast8_t last_x, last_y;
    int_fast8_t depth;

    Board() {
        pos[0] = 0x0000000810000000ULL;  // B
        pos[1] = 0x0000001008000000ULL;  // W
        player = 0;
        last_x = -1, last_y = -1;
        depth = 0;
    }
    char getDisk(int i, int j) {
        if (pos[0] & BIT(i, j))
            return 'B';
        else if (pos[1] & BIT(i, j))
            return 'W';
        else
            return '.';
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
                s += getDisk(i, j);
                if (is_last) s += "\033[m";
                cout << s << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    BitBoard getRev(BitBoard target, bool player_) {
        BitBoard rev = 0;
        REP(k, 8) {
            BitBoard rev_ = 0;
            BitBoard mask = transfer(target, k);
            while (mask != 0 && ((mask & pos[!player_]) != 0)) {
                rev_ |= mask;
                mask = transfer(mask, k);
            }
            if (mask & pos[player_]) rev |= rev_;
        }
        return rev;
    }
    //黒 - 白
    int_fast8_t countDisks() { return __builtin_popcount(pos[0]) - __builtin_popcount(pos[1]); }
    //(i,j)に石をおけるか
    bool canPut(int i, int j) { return BIT(i, j) & makeLegalBoard(pos[player], pos[!player]); }
    bool empty(int i, int j) { return ((pos[0] | pos[1]) & BIT(i, j)) == 0; }
    // player_がそのマスに石をおけるか
    bool canPut(int i, int j, bool player_) {
        return BIT(i, j) & makeLegalBoard(pos[player_], pos[!player_]);
    }
    // player_が石をおけるか
    bool canMove(bool player_) { return makeLegalBoard(pos[player_], pos[!player_]); }
    //終了状態か
    bool isTerminal() { return !canMove(0) && !canMove(1); }
    void put(int_fast8_t i, int_fast8_t j) {
        if (!empty(i, j)) {
            if (DEBUG) {
                cout << "(" << (int)i << ", " << (int)j << ") is not empty!" << endl;
            }
            return;
        }
        BitBoard rev = getRev(BIT(i, j), player);
        if (DEBUG) {
            cout << "rev:" << endl;
            printBIT(rev);
        }
        if (!rev) return;
        last_x = i, last_y = j;
        pos[player] ^= (BIT(i, j) | rev);
        pos[!player] ^= rev;
        player = !player;
        depth++;
    }
    void pass() {
        player = !player;
        depth++;
    }

    //黒の確定石の数-白の確定石
    int_fast8_t calc_confirmed_stone() {
        if (empty(0, 0) && empty(0, 7) && empty(7, 0) && empty(7, 7)) return 0;
        uint_fast8_t side[28] = {};
        uint_fast8_t confirmed[28] = {};
        REP8(i, 28) confirmed[i] = 2;
        REP8(j, 8) {
            if (pos[0] & BIT(0, j))
                side[j] = 0;
            else if (pos[1] & BIT(0, j))
                side[j] = 1;
            else
                side[j] = 2;

            if (pos[0] & BIT(7, j))
                side[21 - j] = 0;
            else if (pos[1] & BIT(7, j))
                side[21 - j] = 1;
            else
                side[21 - j] = 2;
        }
        FOR(i, 1, 7) {
            if (pos[0] & BIT(i, 0))
                side[28 - i] = 0;
            else if (pos[1] & BIT(i, 0))
                side[28 - i] = 1;
            else
                side[28 - i] = 2;

            if (pos[0] & BIT(i, 7))
                side[7 + i] = 0;
            else if (pos[1] & BIT(i, 7))
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
            if (pos[0] & BIT(i, j))
                ret += BOARD_SCORE[i][j];
            else if (pos[1] & BIT(i, j))
                ret -= BOARD_SCORE[i][j];
        }
        return ret;
    }
    //開放度
    int_fast8_t calc_openness() {
        BitBoard legal = makeLegalBoard(pos[player], pos[!player]);
        return __builtin_popcount(legal);
    }
    //全体の評価値
    double eval() {
        // double ret = 0;
        double BS = 1;
        if (this->depth < 20) BS *= 3;
        return board_score() * BS * getrand01() + calc_confirmed_stone() * getrand01() * 80 +
               calc_openness() * getrand01() * 40;
    }
    int_fast16_t eval_fixed() {
        double BS = 1;
        if (this->depth < 20) BS *= 3;
        // if (DEBUG) {
        //     cout << "board score:" << board_score() * BS << endl;
        //     cout << "comfirmed score:" << calc_confirmed_stone() * 80 << endl;
        //     cout << "openness:" << calc_openness() * 40 << endl;
        // }
        return board_score() * BS + calc_confirmed_stone() * 80 + calc_openness() * 40;
    }
    int_fast16_t eval_terminated() { return countDisks() * 100; }
    //ランダムに手番を進める
    void advance() {
        if (isTerminal()) return;
        if (!canMove(player)) {
            // if (DEBUG) {
            //     print();
            //     cout << disk[player] << " cannot move!!!!!" << endl;
            // }
            player = !player;
            return;
        }
        BitBoard legal = makeLegalBoard(pos[player], pos[!player]);
        if (DEBUG) {
            cout << "legal:" << endl;
            printBIT(legal);
        }
        vector<pii> cells;

        REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
            if (canPut(i, j)) {
                cells.push_back({i, j});
            }
        }
        int_fast8_t c_nxt = cells.size();
        assert(c_nxt > 0);
        int_fast8_t idx = xor64() % c_nxt;
        if (DEBUG) {
            cout << "put" << disk[player] << " on (" << (int)cells[idx].first << ", "
                 << (int)cells[idx].second << ")" << endl;
        }
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
            return;
        }
        vector<pair<double, pii>> cells;

        REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
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
            int_fast8_t n = cells.size();
            put(cells[n - 1].second.first, cells[n - 1].second.second);
        }
    }
    //一回プレイアウト
    int_fast8_t playout() {
        Board tmp_b = *this;
        while (!tmp_b.isTerminal()) {
            tmp_b.advance();
        }
        int_fast8_t res = tmp_b.countDisks();
        if (res > 0)
            return 1;
        else if (res == 0)
            return 0;
        else
            return -1;
    }
    int_fast8_t playout_greedy() {
        Board tmp_b = *this;
        while (!tmp_b.isTerminal()) {
            // if (xor64() % 20 == 0)
            //     tmp_b.advance();
            // else
            tmp_b.advance_eval();
        }
        int_fast8_t res = tmp_b.countDisks();
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
int_fast16_t get_min(Board &b, int_fast8_t depth, int_fast16_t par_max);
int_fast16_t get_max(Board &b, int_fast8_t depth, int_fast16_t par_min);
/*----------prototype declaration----------*/

const double C = 1.41421;
double uct_score(int_fast32_t win_count, int_fast32_t visited_count, int_fast32_t N, char player) {
    if (N <= 0) {
        cerr << "N:" << N << endl;
    }
    assert(N > 0);
    double win_rate = (double)win_count / visited_count;
    if (player == 1) win_rate *= -1;
    double bias = sqrt(2 * log(N) / visited_count);
    return win_rate + bias;
}
int threshold_vis = 50;

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

int_fast32_t node_num = 0;

Board Minimax(Board b) {
    clock_t start_t = clock();
    int_fast16_t depth = 0;
    node_num = 0;
    if (b.depth < 10)
        depth = 7;
    else if (b.depth < 35)
        depth = 9;
    else if (b.depth < 48)
        depth = 10;
    else
        depth = 15;
    if (!b.canMove(b.player)) {
        b.pass();
        return b;
    }
    // assert(!b.isTerminal());
    BitBoard legal = makeLegalBoard(b.pos[b.player], b.pos[!b.player]);
    if (b.player == 0) {
        int_fast16_t mx = -30000;
        Board ans;

        REP(i, 8) REP(j, 8) {
            if (legal & BIT(i, j)) {
                Board next_b = b;
                next_b.put(i, j);
                int_fast16_t res = get_min(next_b, depth - 1, mx);
                if (res > mx) {
                    mx = res;
                    ans = next_b;
                }
            }
        }
        if (DISPLAY) {
            cout << "elapsed time:" << static_cast<double>(clock() - start_t) / CLOCKS_PER_SEC * 1000.0
                 << endl;
            cout << "max:" << mx << endl;
            cout << "depth:" << depth << endl;
            cout << "number of nodes:" << node_num << endl;
        }
        if (DISPLAY) {
            if (mx == 20000)
                cout << "first player will win" << endl;
            else if (mx == -20000)
                cout << "second player will win" << endl;
        }

        if (PLAY) ans.print();
        return ans;
    } else {
        int_fast16_t mn = 30000;
        Board ans;
        REP(i, 8) REP(j, 8) {
            if (legal & BIT(i, j)) {
                Board next_b = b;
                next_b.put(i, j);
                int_fast16_t res = get_min(next_b, depth - 1, mn);
                if (res < mn) {
                    mn = res;
                    ans = next_b;
                }
            }
        }
        if (DISPLAY) {
            cout << "elapsed time:" << static_cast<double>(clock() - start_t) / CLOCKS_PER_SEC * 1000.0
                 << endl;
            cout << "min:" << mn << endl;
            cout << "depth:" << depth << endl;
            cout << "number of nodes:" << node_num << endl;
        }
        if (DISPLAY) {
            if (mn == 20000)
                cout << "first player will win" << endl;
            else if (mn == -20000)
                cout << "second player will win" << endl;
        }
        if (PLAY) ans.print();
        return ans;
    }
}

int_fast16_t get_min(Board &b, int_fast8_t depth, int_fast16_t par_max) {
    // if (DEBUG) cout << "depth:" << (int)depth << endl;
    node_num++;
    if (depth == 0) {
        int_fast16_t res = b.eval_fixed();
        if (DEBUG) {
            // b.print();
            // cout << "fixed value:" << res << endl;
        }
        return res;
    }
    if (b.depth >= 60 && b.isTerminal()) {
        // cout << "terminated!" << endl;
        int_fast16_t res = b.eval_terminated();
        if (res < 0)
            return -20000;
        else if (res > 0)
            return 20000;
        else
            return 0;
    }

    int_fast16_t ret = 20000;
    if (!b.canMove(b.player)) {
        Board next_b = b;
        next_b.pass();
        return get_max(next_b, depth - 1, ret);
    }
    BitBoard legal = makeLegalBoard(b.pos[b.player], b.pos[!b.player]);
    if (depth >= 3) {
        vector<pair<double, pii>> v;
        REP(i, 8) REP(j, 8) {
            if (legal & BIT(i, j)) {
                Board next_b = b;
                next_b.put(i, j);
                double res = next_b.eval_fixed();
                v.push_back({res, {i, j}});
            }
        }
        sort(v.begin(), v.end());
        for (auto &p : v) {
            Board next_b = b;
            next_b.put(p.second.first, p.second.second);
            ret = min(ret, get_max(next_b, depth - 1, ret));
            if (ret <= par_max) return ret;
        }
    } else {
        REP(i, 8) REP(j, 8) {
            if (legal & BIT(i, j)) {
                Board next_b = b;
                next_b.put(i, j);
                ret = min(ret, get_max(next_b, depth - 1, ret));
                if (ret <= par_max) return ret;
            }
        }
    }
    return ret;
}
int_fast16_t get_max(Board &b, int_fast8_t depth, int_fast16_t par_min) {
    // if (DEBUG) cout << "depth:" << (int)depth << endl;
    node_num++;
    if (depth == 0) {
        if (DEBUG) {
            int_fast16_t res = b.eval_fixed();
            // if (DEBUG) {
            //     b.print();
            //     cout << "fixed value:" << res << endl;
            // }
            return res;
        }
        return b.eval_fixed();
    }
    if (b.depth >= 60 && b.isTerminal()) {
        // cout << "terminated!" << endl;
        int_fast16_t res = b.eval_terminated();
        if (res < 0)
            return -20000;
        else if (res > 0)
            return 20000;
        else
            return 0;
    }
    int_fast16_t ret = -20000;
    if (!b.canMove(b.player)) {
        Board next_b = b;
        next_b.pass();
        return get_min(next_b, depth - 1, ret);
    }
    BitBoard legal = makeLegalBoard(b.pos[b.player], b.pos[!b.player]);
    if (depth >= 3) {
        vector<pair<double, pii>> v;
        REP(i, 8) REP(j, 8) {
            if (legal & BIT(i, j)) {
                Board next_b = b;
                next_b.put(i, j);
                double res = next_b.eval_fixed();
                v.push_back({res, {i, j}});
            }
        }
        sort(v.rbegin(), v.rend());
        for (auto &p : v) {
            Board next_b = b;
            next_b.put(p.second.first, p.second.second);
            ret = max(ret, get_min(next_b, depth - 1, ret));
            if (ret >= par_min) return ret;
        }
    } else {
        REP(i, 8) REP(j, 8) {
            if (legal & BIT(i, j)) {
                Board next_b = b;
                next_b.put(i, j);
                ret = max(ret, get_min(next_b, depth - 1, ret));
                if (ret >= par_min) return ret;
            }
        }
    }

    return ret;
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
    DISPLAY = true;
    DEBUG = false;
    PLAY = true;
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
        b = Minimax(b);
    }
}
void second() {
    Board b;
    DEBUG = true;
    PLAY = true;
    while (true) {
        if (b.isTerminal()) {
            b.finish();
            return;
        }
        b = Minimax(b);
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

const int MATCH_NUM = 10;
void test_rand() {
    REP(i, 100) { cout << getrand01() << endl; }
}
void test() {
    Board b;
    b.print();
    BitBoard legal = makeLegalBoard(b.pos[0], b.pos[1]);
    printBIT(legal);
    b.advance();
    b.print();
}
void batch_test() {
    PLAY = true;
    DEBUG = false;
    int win_c = 0;
    int lose_c = 0;
    REP(_, MATCH_NUM) {
        Board b;
        b.print();
        cout << "first:Minimax second:random" << endl;
        while (true) {
            if (b.isTerminal()) {
                if (DEBUG) {
                    cout << "game end" << endl;
                }
                b.finish();
                break;
            }
            if (!b.canMove(b.player))
                b.pass();
            else {
                if (b.player == 0) {
                    b = Minimax(b);
                    // b.advance();
                    // b.print();
                } else {
                    b.advance();
                    b.print();
                }
            }

            while (true) {
                if (b.isTerminal()) {
                    if (DEBUG) {
                        cout << "game end" << endl;
                    }
                    b.finish();
                    break;
                }
                if (!b.canMove(b.player)) {
                    b.pass();
                    break;
                }
                if (b.player == 0) {
                    b = Minimax(b);
                    // b.advance();
                    // b.print();
                } else {
                    b.advance();
                    b.print();
                }
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