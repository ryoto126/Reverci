#ifndef _BOARD_H_
#define _BOARD_H_
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

extern int_fast8_t ord[150];
extern const int_fast8_t dx[8];
extern const int_fast8_t dy[8];
extern const int_fast16_t BOARD_SCORE[8][8];

extern uint_fast64_t firstHash, secondHash, playerHash;
// 0->'.', 1->'B', 2->'W'
extern uint_fast64_t zobristHash[8][8][3];
extern const char disk[3];

uint_fast64_t getrand();
//周期2^64-1の乱数
uint_fast64_t xor64();
double getrand01();
struct Board {
    char board[BOARD_SIZE][BOARD_SIZE];
    bool player;
    int_fast8_t last_x, last_y;
    int_fast8_t depth;
    uint_fast64_t hash;
    vector<uint_fast64_t> next_states;

    Board();
    // void undo() { this = *this->pre; }
    void print();
    //黒 - 白
    int_fast8_t countDisks();
    //(i,j)に石をおけるか
    bool canPut(int i, int j);
    // player_がそのマスに石をおけるか
    bool canPut(int i, int j, bool player_);
    // player_が石をおけるか
    bool canMove(bool player_);
    //終了状態か
    bool isTerminal();
    //現在の盤面のハッシュ値を返す
    uint_fast64_t calcHash();
    //(i,j)に石を置いて手番を渡す

    void put(int_fast8_t i, int_fast8_t j);
    void pass();
    //黒の確定石の数-白の確定石
    int_fast8_t calc_confirmed_stone();
    //盤面の評価値
    int_fast16_t board_score();
    //開放度
    int_fast8_t calc_openness();
    //全体の評価値
    double eval();
    int_fast16_t eval_fixed();
    int_fast16_t eval_terminated();
    //ランダムに手番を進める
    void advance();
    //評価値がもっとも高い手を選んで進める
    void advance_eval();
    //一回プレイアウト
    int_fast8_t playout();
    int_fast8_t playout_greedy();
    void finish();
};

#endif