#ifndef CHESSAI_H
#define CHESSAI_H

#include<iostream>
#include<string.h>
#include<memory.h>
#include<set>
#include<deque>
#include<vector>
using namespace std;

/*棋子：无子，黑子，白子*/
#define C_NONE 0
#define C_BLACK 1
#define C_WHITE 2

/*方向：向右、向上、右上、左上*/
#define RIGHT 0
#define UP 1
#define UPRIGHT 2
#define UPLEFT 3

/*各个棋型的代号*/
#define OTHER 0//其他棋型
#define WIN 1//白赢
#define LOSE 2//黑赢
#define FLEX4 3//白活4
#define flex4 4//黑活4
#define BLOCK4 5//白眠4
#define block4 6//黑眠4
#define FLEX3 7//白活3
#define flex3 8//黑活3
#define BLOCK3 9//白眠3
#define block3 10//黑眠3
#define FLEX2 11//白活2
#define flex2 12//黑活2
#define BLOCK2 13//白眠2
#define block2 14//黑眠2
#define FLEX1 15//白活1
#define flex1 16//黑活1

/*游戏状态：进行中/已结束*/
#define UNDERWAY 0
#define FINISH 1

/*轮到谁下：黑棋/白棋*/
#define T_BLACK 0
#define T_WHITE 1

/*比赛结果：黑棋赢，白棋赢，和棋（暂未分出胜负）*/
#define R_BLACK 0
#define R_WHITE 1
#define R_DRAW 2

class Node //结点结构体
{
public:
    int value;//当前结点估值
    int depth;//当前结点深度（根结点深度为0）
    Node* father;//父结点
    set<Node*> children;//子结点集合
    int cntX, cntY;//最后一步落子的横纵坐标
    int board[15][15]{};//当前局面
public:
    Node();//无参构造函数
    Node(Node* node, int opeX, int opeY);//利用父结点构造子结点

    bool is_max_node();//判断当前结点是否为MAX结点
    int get_score(); //当前局面评分（搜索树叶子结点评估值）,能够判断输赢
};

class GameTree //博弈树结构体
{
public:
    int radius;//扩展距离
    int maxDepth;//最大深度
    Node* root;//根节点
    Node* best = NULL;//指向最佳后继结点
    deque<Node*> openTable;//双向队列，存放待扩展结点
public:
    GameTree(int maxDepth, int expandRadius, int(&board)[15][15]);//指定最大深度和扩展距离，当前棋盘棋子分布构造博弈树
    vector<pair<int, int>> get_search_nodes(Node* node);//返回当前棋局的待扩展点坐标集合
    int expand_children_nodes(Node* node); //扩展node节点，生成node结点的所有子结点
    bool is_alpha_beta_cut(Node* node);//判断当前结点是否能进行ab剪枝
    void update_value_from_node(Node* node);//更新非叶子结点的估值
    void set_next_pos();//确定下一步的最佳位置
    void game_tree();//控制整个博弈搜索过程的核心函数
};


class Play
{
private:
    int cur_board[15][15];//当前棋盘状态
    vector<pair<int, int>> path;//记录每次添加的坐标点对，便于悔棋操作
    int turn;//当前轮到（黑棋/白棋）
    int status;//当前状态（游戏进行中/游戏结束）
    int result;//游戏结果（白棋胜/黑棋胜/平局）
public:
    Play();
    void init_tuple6type();//初始化六元组
    pair<int, int> get_from_black();//从用户输入获取黑棋下一步位置
    void display_board();//展示当前棋盘状态
    int game();//控制整个游戏的核心函数
};

#endif // CHESSAI_H