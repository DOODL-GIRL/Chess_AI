#include "chessai.h"
#include<iostream>
#include<string>
#include<memory.h>
#include<graphics.h>
#include<set>
#include<deque>
#include<vector>
#include<windows.h>
using namespace std;

//全局变量
int tuple6type[4][4][4][4][4][4]{};//棋型辨识数组,0无子,1黑子,2白子,3边界

/*工具函数*/
int evaluate(int board[15][15], int(&stat)[17])//stat返回某棋面中17种棋型数量;局面输赢可以通过函数返回值返回
{
    int A[17][17];//包括边界的虚拟大棋盘,board[i][j]=A[i-1][j-1],3表示边界
    for (int i = 0; i < 17; ++i)A[i][0] = 3;
    for (int i = 0; i < 17; ++i)A[i][16] = 3;
    for (int j = 0; j < 17; ++j)A[0][j] = 3;
    for (int j = 0; j < 17; ++j)A[16][j] = 3;
    for (int i = 0; i < 15; ++i)
        for (int j = 0; j < 15; ++j)
            A[i + 1][j + 1] = board[i][j];

    //判断横向棋型
    for (int i = 1; i <= 15; ++i)
    {
        for (int j = 0; j < 12; ++j)
        {
            int type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
            stat[type]++;
        }
    }
    //判断竖向棋型
    for (int j = 1; j <= 15; ++j)
    {
        for (int i = 0; i < 12; ++i)
        {
            int type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
            stat[type]++;
        }
    }
    //判断左上至右下棋型
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 12; ++j) {
            int type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
            stat[type]++;
        }
    }
    //判断右上至左下棋型
    for (int i = 0; i < 12; ++i) {
        for (int j = 5; j < 17; ++j) {
            int type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
            stat[type]++;
        }
    }

    //若出现连五可以直接判断输赢
    //白赢
    if (stat[WIN] > 0)
        return R_WHITE;
    //黑赢
    else if (stat[LOSE] > 0)
        return R_BLACK;
    else
        return R_DRAW;
}

/*Node类函数实现*/
Node::Node()//无参构造函数
{
    father = nullptr;
    children.clear();
    value = INT_MIN;
    depth = cntX = cntY = 0;
    memset(board, C_NONE, sizeof(board));
}

Node::Node(Node* node, int opeX, int opeY)//利用父结点构造子结点
{
    depth = node->depth + 1;
    value = is_max_node() ? INT32_MIN : INT32_MAX;
    father = node;
    children.clear();
    cntX = opeX;
    cntY = opeY;
    memcpy(board, node->board, sizeof(board));
    board[cntX][cntY] = (depth % 2 == 0) ? C_BLACK : C_WHITE;
}

bool Node::is_max_node()//判断当前结点是否为MAX结点
{
    if (depth % 2 == 0)
        return true;
    else
        return false;
}

int Node::get_score()//返回某棋面得分
{
    //各棋型对应权重
    int weight[17]= { 0,1000000,-10000000,50000,-100000,400,-100000,400,-8000,20,-50,20,-50,1,-3,1,-3 };
    int stat[17];//统计17种棋型的个数
    memset(stat, 0, sizeof(stat));
    evaluate(board, stat);
    int score = 0;
    for (int i = 0; i < 17; ++i)
        score += stat[i] * weight[i];//初步计分
    return score;
}


/*GameTree类函数实现*/
GameTree::GameTree(int maxDepth, int radius, int(&board)[15][15])//构造函数
{
    this->maxDepth = maxDepth;
    this->radius=radius;
    root = new(nothrow)Node;
    if (root == NULL)
        exit(EXIT_FAILURE);
    memcpy(this->root->board, board, sizeof(board));
}

vector<pair<int, int>> GameTree::get_search_nodes(Node* node) //返回当前棋局的待扩展点坐标集合
{
    bool hasChess = false;
    bool newBoard[15][15];
    memset(newBoard, false, sizeof(newBoard));
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (node->board[i][j] == C_NONE) continue;
            //只选择棋盘内距每个有棋子位置radius范围为内的位置作为下一步的候选位置（进行扩展）
            hasChess = true;
            int x1 = max(0, i - radius);//扩展左边界
            int x2 = min(14, i + radius);//扩展右边界
            int y1 = max(0, j - radius);//扩展上边界
            int y2 = min(14, j + radius);//扩展下边界
            for (int x = x1; x <= x2; x++)
                for (int y = y1; y <= y2; y++)
                    if (node->board[x][y] == 0)
                        newBoard[x][y] = true;
            /*for (int i = 0; i < 15; i++)
            {
                for (int j = 0; j < 15; j++)
                    cout << newBoard[i][j] << " ";
                cout << endl;
            }*/
        }
    }

    vector<pair<int, int>> mask;

    if (!hasChess) //棋盘上没有棋子时下在中间位置，事实上人类先手不需要考虑这个
    {
        mask.emplace_back(pair<int, int>(7, 7));
    }
    else 
    {
        for (int i = 0; i < 15; i++)
            for (int j = 0; j < 15; j++)
                if (newBoard[i][j])
                    mask.emplace_back(pair<int, int>(i, j));
    }
    return mask;
}

int GameTree::expand_children_nodes(Node* node) //扩展node结点，生成node结点的所有子结点
{
    vector<pair<int, int>> mask = get_search_nodes(node);
    for (auto pos : mask) 
    {
        //cout << pos.first << " " << pos.second << endl;
        Node* n = new Node(node, pos.first, pos.second);
        if (n == NULL)exit(EXIT_FAILURE);
        node->children.insert(n);
        openTable.push_front(n);
    }
    return int(mask.size());//返回node结点的子结点数量
}

bool GameTree::is_alpha_beta_cut(Node* node) //判断当前结点是否能进行ab剪枝
{
    if (node == nullptr || node->father == nullptr) 
        return false;
    if (node->is_max_node() && node->value > node->father->value) 
        return true;
    if (!node->is_max_node() && node->value < node->father->value) 
        return true;
    return is_alpha_beta_cut(node->father);
}

void GameTree::update_value_from_node(Node* node) //更新非叶子结点的估值
{
    if (node == nullptr) 
        return;
    if (node->children.empty()) //叶子结点估值已经通过evaluate函数更新
    {
        update_value_from_node(node->father);
        return;
    }
    if (node->is_max_node()) //该结点为max结点
    {
        int cntValue = INT32_MIN;
        for (Node* n : node->children)
            if (n->value != INT32_MAX) 
                cntValue = max(cntValue, n->value);
        if (cntValue > node->value) 
        {
            node->value = cntValue;
            update_value_from_node(node->father);
        }
    }
    else  //该结点为min结点
    {
        int cntValue = INT32_MAX;
        for (Node* n : node->children)
            if (n->value != INT32_MIN) cntValue = min(cntValue, n->value);
        if (cntValue < node->value) 
        {
            node->value = cntValue;
            update_value_from_node(node->father);
        }
    }
}

void GameTree::set_next_pos()//设置下一步的最佳位置
{
    best = *root->children.begin();
    for (Node* p : root->children)
        if (p->value > best->value) best = p;
}

void GameTree::game_tree()//控制整个博弈搜索过程的核心函数
{
    /*计时相关变量*/
    double time = 0;
    double counts = 0;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;

    /*开始搜索并计时*/
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nBeginTime);//开始计时

    openTable.push_back(root);//将根结点放入open表中
    while (!openTable.empty()) 
    {
        Node* node = openTable.front();
        openTable.pop_front();
        //closedTable.push_back(node);
        if (is_alpha_beta_cut(node->father))//若父结点被剪枝，则该结点不进行扩展 
            continue;
        if (node->depth < maxDepth) 
        {
            int numExpand = expand_children_nodes(node);
            if (numExpand != 0) 
                continue;
        }
        //若为叶子结点则开始对棋面评分
        node->value = node->get_score() + (node->cntX) * (14 - node->cntX) + (node->cntY) * (14 - node->cntY);//越靠近中间的位置得分越高
        update_value_from_node(node);
    }

    set_next_pos();

    QueryPerformanceCounter(&nEndTime);//停止计时
    time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s
    int time_ms = time * 1000;
    cout << time_ms << "ms" << endl;
}


/*Play类函数实现*/
Play::Play()
{
    memset(cur_board, C_NONE, sizeof(cur_board));
    path.clear();
    turn = T_BLACK;
    status = UNDERWAY;
    result = R_DRAW;
}

void Play::init_tuple6type() //六元组初始化函数
{
    memset(tuple6type, 0, sizeof(tuple6type));//全部设为0
    //白连5,ai赢
    tuple6type[2][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][0] = WIN;
    tuple6type[0][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][1] = WIN;
    tuple6type[1][2][2][2][2][2] = WIN;
    tuple6type[3][2][2][2][2][2] = WIN;//边界考虑
    tuple6type[2][2][2][2][2][3] = WIN;
    //黑连5,ai输
    tuple6type[1][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][0] = LOSE;
    tuple6type[0][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][2] = LOSE;
    tuple6type[2][1][1][1][1][1] = LOSE;
    tuple6type[3][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][3] = LOSE;
    //白活4
    tuple6type[0][2][2][2][2][0] = FLEX4;
    //黑活4
    tuple6type[0][1][1][1][1][0] = flex4;
    //白活3
    tuple6type[0][2][2][2][0][0] = FLEX3;
    tuple6type[0][0][2][2][2][0] = FLEX3;
    tuple6type[0][2][0][2][2][0] = FLEX3;
    tuple6type[0][2][2][0][2][0] = FLEX3;
    //黑活3
    tuple6type[0][1][1][1][0][0] = flex3;
    tuple6type[0][0][1][1][1][0] = flex3;
    tuple6type[0][1][0][1][1][0] = flex3;
    tuple6type[0][1][1][0][1][0] = flex3;
    //白活2
    tuple6type[0][2][2][0][0][0] = FLEX2;
    tuple6type[0][2][0][2][0][0] = FLEX2;
    tuple6type[0][2][0][0][2][0] = FLEX2;
    tuple6type[0][0][2][2][0][0] = FLEX2;
    tuple6type[0][0][2][0][2][0] = FLEX2;
    tuple6type[0][0][0][2][2][0] = FLEX2;
    //黑活2
    tuple6type[0][1][1][0][0][0] = flex2;
    tuple6type[0][1][0][1][0][0] = flex2;
    tuple6type[0][1][0][0][1][0] = flex2;
    tuple6type[0][0][1][1][0][0] = flex2;
    tuple6type[0][0][1][0][1][0] = flex2;
    tuple6type[0][0][0][1][1][0] = flex2;
    //白活1
    tuple6type[0][2][0][0][0][0] = FLEX1;
    tuple6type[0][0][2][0][0][0] = FLEX1;
    tuple6type[0][0][0][2][0][0] = FLEX1;
    tuple6type[0][0][0][0][2][0] = FLEX1;
    //黑活1
    tuple6type[0][1][0][0][0][0] = flex1;
    tuple6type[0][0][1][0][0][0] = flex1;
    tuple6type[0][0][0][1][0][0] = flex1;
    tuple6type[0][0][0][0][1][0] = flex1;

    int p1, p2, p3, p4, p5, p6, x, y, ix, iy;//x:左5中黑个数,y:左5中白个数,ix:右5中黑个数,iy:右5中白个数
    for (p1 = 0; p1 < 4; ++p1) {
        for (p2 = 0; p2 < 3; ++p2) {
            for (p3 = 0; p3 < 3; ++p3) {
                for (p4 = 0; p4 < 3; ++p4) {
                    for (p5 = 0; p5 < 3; ++p5) {
                        for (p6 = 0; p6 < 4; ++p6) {
                            x = y = ix = iy = 0;

                            if (p1 == 1)x++;
                            else if (p1 == 2)y++;

                            if (p2 == 1) { x++; ix++; }
                            else if (p2 == 2) { y++; iy++; }

                            if (p3 == 1) { x++; ix++; }
                            else if (p3 == 2) { y++; iy++; }

                            if (p4 == 1) { x++; ix++; }
                            else if (p4 == 2) { y++; iy++; }

                            if (p5 == 1) { x++; ix++; }
                            else if (p5 == 2) { y++; iy++; }

                            if (p6 == 1)ix++;
                            else if (p6 == 2)iy++;

                            if (p1 == 3 || p6 == 3) {//有边界
                                if (p1 == 3 && p6 != 3) {//左边界
                                    //白冲4
                                    if (ix == 0 && iy == 4) {//若右边有空位是活4也没关系，因为活4权重远大于冲4，再加上冲4权重变化可以不计
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    //黑冲4
                                    if (ix == 4 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    //白眠3
                                    if (ix == 0 && iy == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    //黑眠3
                                    if (ix == 3 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    //白眠2
                                    if (ix == 0 && iy == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    //黑眠2
                                    if (ix == 2 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                                else if (p6 == 3 && p1 != 3) {//右边界
                                   //白冲4
                                    if (x == 0 && y == 4) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    //黑冲4
                                    if (x == 4 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    //黑眠3
                                    if (x == 3 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    //白眠3
                                    if (x == 0 && y == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    //黑眠2
                                    if (x == 2 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    //白眠2
                                    if (x == 0 && y == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                            }
                            else {//无边界
                               //白冲4
                                if ((x == 0 && y == 4) || (ix == 0 && iy == 4)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                }
                                //黑冲4
                                if ((x == 4 && y == 0) || (ix == 4 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                }
                                //白眠3
                                if ((x == 0 && y == 3) || (ix == 0 && iy == 3)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                }
                                //黑眠3
                                if ((x == 3 && y == 0) || (ix == 3 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                }
                                //白眠2
                                if ((x == 0 && y == 2) || (ix == 0 && iy == 2)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                }
                                //黑眠2
                                if ((x == 2 && y == 0) || (ix == 2 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

pair<int, int> Play::get_from_black()//通过用户输入获取黑棋下一步位置（包括悔棋），与可视化对接
{
    int x, y;
    MOUSEMSG m;		// 定义鼠标消息
    while (true)
    {
        //获取一条鼠标消息
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONUP)
        {
            if (m.x >= 350 + 10 && m.x <= 350 + 290 && m.y >= 50 + 10 && m.y <= 50 + 290)//黑棋落子
            {
                x = (m.x - 10 - 350) / 20;
                y = (m.y - 10 - 50) / 20;
                if (cur_board[x][y] == C_NONE)
                    break;
            }
            else if (m.x >= 20 && m.x <= 132 && m.y >= 20 && m.y <= 60)//悔棋
            {
                x = -1;
                y = -1;
                break;
            }
            else if (m.x >= 20 && m.x <= 132 && m.y >= 100 && m.y <= 140)//退出游戏
            {
                closegraph();
                exit(0);
            }
        }
    }
    return make_pair(x, y);
}

void Play::display_board()//展示棋盘，与可视化对接
{
    IMAGE img;
    loadimage(&img, _T("img\\chess_board.png"), 300, 300, true);
    putimage(350, 50, &img);
    setlinecolor(BLACK);
    for (int i = 0; i < 15; i++)//画网格线
    {
        line(350 + 10, 50 + 10 + i * 20, 350 + 290, 50 + 10 + i * 20);
        line(350 + 10 + 20 * i, 50 + 10, 350 + 10 + 20 * i, 50 + 290);
    }
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (cur_board[i][j] == C_BLACK)
            {
                setfillcolor(BLACK);
                solidcircle(350 + 10 + i * 20, 50 + 10 + j * 20, 7);
            }
            else if (cur_board[i][j] == C_WHITE)
            {
                setfillcolor(WHITE);
                solidcircle(350 + 10 + i * 20, 50 + 10 + j * 20, 7);
            }
            else;
        }
    }
}

int Play::game()//控制整个游戏的核心函数
{
    status = UNDERWAY;
    int count = 0;//落子总数
    init_tuple6type();
    while (status == UNDERWAY)//要设置一个set函数能够在搜索过程中判断输赢
    {
        //若胜负已分，直接返回结果
        if (result == R_BLACK)
            status = FINISH;
        else if (result == R_WHITE)
            status = FINISH;
        else if (count == 15 * 15)
            status = FINISH;
        else
        {
            int stat[17];//统计17种棋型的个数
            memset(stat, 0, sizeof(stat));
            //黑棋(人类）
            turn = T_BLACK;
            while (1)
            {
                pair<int, int> black_new = get_from_black();
                if (black_new.first == -1 && black_new.second == -1)//黑棋选择悔棋
                {
                    if (count < 2)
                        continue;
                    else
                    {
                        pair<int, int>regreted_white = path[path.size() - 1];
                        path.pop_back();
                        count--;
                        pair<int, int>regreted_black = path[path.size() - 1];
                        path.pop_back();
                        count--;
                        cur_board[regreted_white.first][regreted_white.second] = C_NONE;
                        cur_board[regreted_black.first][regreted_black.second] = C_NONE;
                        display_board();
                        result = evaluate(cur_board, stat);
                    }
                }
                else
                {
                    count++;
                    cur_board[black_new.first][black_new.second] = C_BLACK;
                    path.push_back(black_new);
                    display_board();
                    result = evaluate(cur_board, stat);
                    break;
                }
            }
            if (result == R_DRAW)//暂未分出胜负
            {
                //白棋（AI)
                turn = T_WHITE;
                GameTree my_game_tree(2, 3, cur_board);//构建博弈搜索树
                my_game_tree.game_tree();
                pair<int, int> white_new = make_pair(my_game_tree.best->cntX, my_game_tree.best->cntY);
                count++;
                cur_board[white_new.first][white_new.second] = C_WHITE;
                display_board();
                path.push_back(white_new);
                result = evaluate(cur_board, stat);
            }
        }
    }
    return result;
}