#include "Board.h"

#include <bits/stdc++.h>
using namespace std;
#define REP8(i, n) for (int_fast8_t i = 0; i < (n); i++)
#define FOR8(i, m, n) for (int_fast8_t i = (m); i < (n); i++)
#define REP(i, n) for (int_fast16_t i = 0; i < (n); i++)
#define FOR(i, m, n) for (int_fast16_t i = (m); i < (n); i++)
#define isInside(x, y) ((x) >= 0 && (x) < 8 && (y) >= 0 && (y) < 8)
using pii = pair<int_fast8_t, int_fast8_t>;
#define BOARD_SIZE 8
#define PLAYOUT_COUNT 1000

Board::Board() {
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
    depth = 0;
    hash ^= firstHash;
    REP8(i, BOARD_SIZE)
    REP8(j, BOARD_SIZE) {
        if (board[i][j] == 'B') hash ^= zobristHash[i][j][0];
        if (board[i][j] == 'W') hash ^= zobristHash[i][j][1];
        if (board[i][j] == '.') hash ^= zobristHash[i][j][2];
    }
}
// void undo() { this = *this->pre; }
void Board::print() {
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
int_fast8_t Board::countDisks() {
    int_fast8_t ret = 0;
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
bool Board::canPut(int i, int j) {
    if (board[i][j] != '.') return false;
    REP8(dir, 8) {
        int_fast8_t nx = i + dx[dir], ny = j + dy[dir];
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
bool Board::canPut(int i, int j, bool player_) {
    if (board[i][j] != '.') return false;
    REP8(dir, 8) {
        int_fast8_t nx = i + dx[dir], ny = j + dy[dir];
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
bool Board::canMove(bool player_) {
    REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) {
        if (canPut(i, j, player_)) return true;
    }
    return false;
}
//終了状態か
bool Board::isTerminal() { return !canMove(0) && !canMove(1); }

//現在の盤面のハッシュ値を返す
uint_fast64_t Board::calcHash() {
    uint_fast64_t ret = 0;
    if (player == 0)
        ret ^= firstHash;
    else
        ret ^= secondHash;
    REP8(i, BOARD_SIZE) REP8(j, BOARD_SIZE) { ret ^= zobristHash[i][j][ord[board[i][j]]]; }
    return ret;
}
//(i,j)に石を置いて手番を渡す

void Board::put(int_fast8_t i, int_fast8_t j) {
    // if (DEBUG) {
    //     cout << "put " << player << " on (" << i << "," << j << ")" << endl;
    //     // cout << board[i][j] << endl;
    // }
    hash ^= zobristHash[i][j][2];
    board[i][j] = disk[player];
    // cout << "ord[" << player << "]:" << ord[player] << endl;
    hash ^= zobristHash[i][j][player];
    REP8(dir, 8) {
        int_fast8_t nx = i + dx[dir], ny = j + dy[dir];
        if (!isInside(nx, ny)) continue;
        if (board[nx][ny] != disk[!player]) continue;
        REP8(k, BOARD_SIZE) {
            nx += dx[dir], ny += dy[dir];
            if (!isInside(nx, ny)) break;     //はみ出たらアウト
            if (board[nx][ny] == '.') break;  //空きマスでもアウト

            // i,jからnx,nyまで全部ひっくり返す
            if (board[nx][ny] == disk[player]) {
                int_fast8_t x = i + dx[dir], y = j + dy[dir];
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
    depth++;
    // hash = calcHash();
}
void Board::pass() {
    player = !player;
    hash ^= playerHash;
    depth++;
    // hash = calcHash();
}
//黒の確定石の数-白の確定石
int_fast8_t Board::calc_confirmed_stone() {
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
int_fast16_t Board::board_score() {
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
int_fast8_t Board::calc_openness() {
    int_fast8_t ret = 0;
    REP8(i, 8) REP8(j, 8) {
        if (canPut(i, j, 0)) ret++;
        if (canPut(i, j, 1)) ret--;
    }
    return ret;
}
//全体の評価値
double Board::eval() {
    // double ret = 0;
    double BS = 1;
    if (this->depth < 20) BS *= 3;
    return board_score() * BS * getrand01() + calc_confirmed_stone() * getrand01() * 80 +
           calc_openness() * getrand01() * 40;
}
int_fast16_t Board::eval_fixed() {
    double BS = 1;
    if (this->depth < 20) BS *= 3;
    // if (DEBUG) {
    //     cout << "board score:" << board_score() * BS << endl;
    //     cout << "comfirmed score:" << calc_confirmed_stone() * 80 << endl;
    //     cout << "openness:" << calc_openness() * 40 << endl;
    // }
    return board_score() * BS + calc_confirmed_stone() * 80 + calc_openness() * 40;
}
int_fast16_t Board::eval_terminated() { return countDisks() * 100; }
//ランダムに手番を進める
void Board::advance() {
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
    int_fast8_t c_nxt = cells.size();
    assert(c_nxt > 0);
    int_fast8_t idx = xor64() % c_nxt;
    put(cells[idx].first, cells[idx].second);
}
//評価値がもっとも高い手を選んで進める
void Board::advance_eval() {
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
        int_fast8_t n = cells.size();
        put(cells[n - 1].second.first, cells[n - 1].second.second);
    }
}
//一回プレイアウト
int_fast8_t Board::playout() {
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
int_fast8_t Board::playout_greedy() {
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
void Board::finish() {
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