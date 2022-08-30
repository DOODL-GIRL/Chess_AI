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

//ȫ�ֱ���
int tuple6type[4][4][4][4][4][4]{};//���ͱ�ʶ����,0����,1����,2����,3�߽�

/*���ߺ���*/
int evaluate(int board[15][15], int(&stat)[17])//stat����ĳ������17����������;������Ӯ����ͨ����������ֵ����
{
    int A[17][17];//�����߽�����������,board[i][j]=A[i-1][j-1],3��ʾ�߽�
    for (int i = 0; i < 17; ++i)A[i][0] = 3;
    for (int i = 0; i < 17; ++i)A[i][16] = 3;
    for (int j = 0; j < 17; ++j)A[0][j] = 3;
    for (int j = 0; j < 17; ++j)A[16][j] = 3;
    for (int i = 0; i < 15; ++i)
        for (int j = 0; j < 15; ++j)
            A[i + 1][j + 1] = board[i][j];

    //�жϺ�������
    for (int i = 1; i <= 15; ++i)
    {
        for (int j = 0; j < 12; ++j)
        {
            int type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
            stat[type]++;
        }
    }
    //�ж���������
    for (int j = 1; j <= 15; ++j)
    {
        for (int i = 0; i < 12; ++i)
        {
            int type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
            stat[type]++;
        }
    }
    //�ж���������������
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 12; ++j) {
            int type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
            stat[type]++;
        }
    }
    //�ж���������������
    for (int i = 0; i < 12; ++i) {
        for (int j = 5; j < 17; ++j) {
            int type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
            stat[type]++;
        }
    }

    //�������������ֱ���ж���Ӯ
    //��Ӯ
    if (stat[WIN] > 0)
        return R_WHITE;
    //��Ӯ
    else if (stat[LOSE] > 0)
        return R_BLACK;
    else
        return R_DRAW;
}

/*Node�ຯ��ʵ��*/
Node::Node()//�޲ι��캯��
{
    father = nullptr;
    children.clear();
    value = INT_MIN;
    depth = cntX = cntY = 0;
    memset(board, C_NONE, sizeof(board));
}

Node::Node(Node* node, int opeX, int opeY)//���ø���㹹���ӽ��
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

bool Node::is_max_node()//�жϵ�ǰ����Ƿ�ΪMAX���
{
    if (depth % 2 == 0)
        return true;
    else
        return false;
}

int Node::get_score()//����ĳ����÷�
{
    //�����Ͷ�ӦȨ��
    int weight[17]= { 0,1000000,-10000000,50000,-100000,400,-100000,400,-8000,20,-50,20,-50,1,-3,1,-3 };
    int stat[17];//ͳ��17�����͵ĸ���
    memset(stat, 0, sizeof(stat));
    evaluate(board, stat);
    int score = 0;
    for (int i = 0; i < 17; ++i)
        score += stat[i] * weight[i];//�����Ʒ�
    return score;
}


/*GameTree�ຯ��ʵ��*/
GameTree::GameTree(int maxDepth, int radius, int(&board)[15][15])//���캯��
{
    this->maxDepth = maxDepth;
    this->radius=radius;
    root = new(nothrow)Node;
    if (root == NULL)
        exit(EXIT_FAILURE);
    memcpy(this->root->board, board, sizeof(board));
}

vector<pair<int, int>> GameTree::get_search_nodes(Node* node) //���ص�ǰ��ֵĴ���չ�����꼯��
{
    bool hasChess = false;
    bool newBoard[15][15];
    memset(newBoard, false, sizeof(newBoard));
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (node->board[i][j] == C_NONE) continue;
            //ֻѡ�������ھ�ÿ��������λ��radius��ΧΪ�ڵ�λ����Ϊ��һ���ĺ�ѡλ�ã�������չ��
            hasChess = true;
            int x1 = max(0, i - radius);//��չ��߽�
            int x2 = min(14, i + radius);//��չ�ұ߽�
            int y1 = max(0, j - radius);//��չ�ϱ߽�
            int y2 = min(14, j + radius);//��չ�±߽�
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

    if (!hasChess) //������û������ʱ�����м�λ�ã���ʵ���������ֲ���Ҫ�������
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

int GameTree::expand_children_nodes(Node* node) //��չnode��㣬����node���������ӽ��
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
    return int(mask.size());//����node�����ӽ������
}

bool GameTree::is_alpha_beta_cut(Node* node) //�жϵ�ǰ����Ƿ��ܽ���ab��֦
{
    if (node == nullptr || node->father == nullptr) 
        return false;
    if (node->is_max_node() && node->value > node->father->value) 
        return true;
    if (!node->is_max_node() && node->value < node->father->value) 
        return true;
    return is_alpha_beta_cut(node->father);
}

void GameTree::update_value_from_node(Node* node) //���·�Ҷ�ӽ��Ĺ�ֵ
{
    if (node == nullptr) 
        return;
    if (node->children.empty()) //Ҷ�ӽ���ֵ�Ѿ�ͨ��evaluate��������
    {
        update_value_from_node(node->father);
        return;
    }
    if (node->is_max_node()) //�ý��Ϊmax���
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
    else  //�ý��Ϊmin���
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

void GameTree::set_next_pos()//������һ�������λ��
{
    best = *root->children.begin();
    for (Node* p : root->children)
        if (p->value > best->value) best = p;
}

void GameTree::game_tree()//�������������������̵ĺ��ĺ���
{
    /*��ʱ��ر���*/
    double time = 0;
    double counts = 0;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;

    /*��ʼ��������ʱ*/
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nBeginTime);//��ʼ��ʱ

    openTable.push_back(root);//����������open����
    while (!openTable.empty()) 
    {
        Node* node = openTable.front();
        openTable.pop_front();
        //closedTable.push_back(node);
        if (is_alpha_beta_cut(node->father))//������㱻��֦����ý�㲻������չ 
            continue;
        if (node->depth < maxDepth) 
        {
            int numExpand = expand_children_nodes(node);
            if (numExpand != 0) 
                continue;
        }
        //��ΪҶ�ӽ����ʼ����������
        node->value = node->get_score() + (node->cntX) * (14 - node->cntX) + (node->cntY) * (14 - node->cntY);//Խ�����м��λ�õ÷�Խ��
        update_value_from_node(node);
    }

    set_next_pos();

    QueryPerformanceCounter(&nEndTime);//ֹͣ��ʱ
    time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//�������ִ��ʱ�䵥λΪs
    int time_ms = time * 1000;
    cout << time_ms << "ms" << endl;
}


/*Play�ຯ��ʵ��*/
Play::Play()
{
    memset(cur_board, C_NONE, sizeof(cur_board));
    path.clear();
    turn = T_BLACK;
    status = UNDERWAY;
    result = R_DRAW;
}

void Play::init_tuple6type() //��Ԫ���ʼ������
{
    memset(tuple6type, 0, sizeof(tuple6type));//ȫ����Ϊ0
    //����5,aiӮ
    tuple6type[2][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][0] = WIN;
    tuple6type[0][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][1] = WIN;
    tuple6type[1][2][2][2][2][2] = WIN;
    tuple6type[3][2][2][2][2][2] = WIN;//�߽翼��
    tuple6type[2][2][2][2][2][3] = WIN;
    //����5,ai��
    tuple6type[1][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][0] = LOSE;
    tuple6type[0][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][2] = LOSE;
    tuple6type[2][1][1][1][1][1] = LOSE;
    tuple6type[3][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][3] = LOSE;
    //�׻�4
    tuple6type[0][2][2][2][2][0] = FLEX4;
    //�ڻ�4
    tuple6type[0][1][1][1][1][0] = flex4;
    //�׻�3
    tuple6type[0][2][2][2][0][0] = FLEX3;
    tuple6type[0][0][2][2][2][0] = FLEX3;
    tuple6type[0][2][0][2][2][0] = FLEX3;
    tuple6type[0][2][2][0][2][0] = FLEX3;
    //�ڻ�3
    tuple6type[0][1][1][1][0][0] = flex3;
    tuple6type[0][0][1][1][1][0] = flex3;
    tuple6type[0][1][0][1][1][0] = flex3;
    tuple6type[0][1][1][0][1][0] = flex3;
    //�׻�2
    tuple6type[0][2][2][0][0][0] = FLEX2;
    tuple6type[0][2][0][2][0][0] = FLEX2;
    tuple6type[0][2][0][0][2][0] = FLEX2;
    tuple6type[0][0][2][2][0][0] = FLEX2;
    tuple6type[0][0][2][0][2][0] = FLEX2;
    tuple6type[0][0][0][2][2][0] = FLEX2;
    //�ڻ�2
    tuple6type[0][1][1][0][0][0] = flex2;
    tuple6type[0][1][0][1][0][0] = flex2;
    tuple6type[0][1][0][0][1][0] = flex2;
    tuple6type[0][0][1][1][0][0] = flex2;
    tuple6type[0][0][1][0][1][0] = flex2;
    tuple6type[0][0][0][1][1][0] = flex2;
    //�׻�1
    tuple6type[0][2][0][0][0][0] = FLEX1;
    tuple6type[0][0][2][0][0][0] = FLEX1;
    tuple6type[0][0][0][2][0][0] = FLEX1;
    tuple6type[0][0][0][0][2][0] = FLEX1;
    //�ڻ�1
    tuple6type[0][1][0][0][0][0] = flex1;
    tuple6type[0][0][1][0][0][0] = flex1;
    tuple6type[0][0][0][1][0][0] = flex1;
    tuple6type[0][0][0][0][1][0] = flex1;

    int p1, p2, p3, p4, p5, p6, x, y, ix, iy;//x:��5�кڸ���,y:��5�а׸���,ix:��5�кڸ���,iy:��5�а׸���
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

                            if (p1 == 3 || p6 == 3) {//�б߽�
                                if (p1 == 3 && p6 != 3) {//��߽�
                                    //�׳�4
                                    if (ix == 0 && iy == 4) {//���ұ��п�λ�ǻ�4Ҳû��ϵ����Ϊ��4Ȩ��Զ���ڳ�4���ټ��ϳ�4Ȩ�ر仯���Բ���
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    //�ڳ�4
                                    if (ix == 4 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    //����3
                                    if (ix == 0 && iy == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    //����3
                                    if (ix == 3 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    //����2
                                    if (ix == 0 && iy == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    //����2
                                    if (ix == 2 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                                else if (p6 == 3 && p1 != 3) {//�ұ߽�
                                   //�׳�4
                                    if (x == 0 && y == 4) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    //�ڳ�4
                                    if (x == 4 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    //����3
                                    if (x == 3 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    //����3
                                    if (x == 0 && y == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    //����2
                                    if (x == 2 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    //����2
                                    if (x == 0 && y == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                            }
                            else {//�ޱ߽�
                               //�׳�4
                                if ((x == 0 && y == 4) || (ix == 0 && iy == 4)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                }
                                //�ڳ�4
                                if ((x == 4 && y == 0) || (ix == 4 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                }
                                //����3
                                if ((x == 0 && y == 3) || (ix == 0 && iy == 3)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                }
                                //����3
                                if ((x == 3 && y == 0) || (ix == 3 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                }
                                //����2
                                if ((x == 0 && y == 2) || (ix == 0 && iy == 2)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                }
                                //����2
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

pair<int, int> Play::get_from_black()//ͨ���û������ȡ������һ��λ�ã��������壩������ӻ��Խ�
{
    int x, y;
    MOUSEMSG m;		// ���������Ϣ
    while (true)
    {
        //��ȡһ�������Ϣ
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONUP)
        {
            if (m.x >= 350 + 10 && m.x <= 350 + 290 && m.y >= 50 + 10 && m.y <= 50 + 290)//��������
            {
                x = (m.x - 10 - 350) / 20;
                y = (m.y - 10 - 50) / 20;
                if (cur_board[x][y] == C_NONE)
                    break;
            }
            else if (m.x >= 20 && m.x <= 132 && m.y >= 20 && m.y <= 60)//����
            {
                x = -1;
                y = -1;
                break;
            }
            else if (m.x >= 20 && m.x <= 132 && m.y >= 100 && m.y <= 140)//�˳���Ϸ
            {
                closegraph();
                exit(0);
            }
        }
    }
    return make_pair(x, y);
}

void Play::display_board()//չʾ���̣�����ӻ��Խ�
{
    IMAGE img;
    loadimage(&img, _T("img\\chess_board.png"), 300, 300, true);
    putimage(350, 50, &img);
    setlinecolor(BLACK);
    for (int i = 0; i < 15; i++)//��������
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

int Play::game()//����������Ϸ�ĺ��ĺ���
{
    status = UNDERWAY;
    int count = 0;//��������
    init_tuple6type();
    while (status == UNDERWAY)//Ҫ����һ��set�����ܹ��������������ж���Ӯ
    {
        //��ʤ���ѷ֣�ֱ�ӷ��ؽ��
        if (result == R_BLACK)
            status = FINISH;
        else if (result == R_WHITE)
            status = FINISH;
        else if (count == 15 * 15)
            status = FINISH;
        else
        {
            int stat[17];//ͳ��17�����͵ĸ���
            memset(stat, 0, sizeof(stat));
            //����(���ࣩ
            turn = T_BLACK;
            while (1)
            {
                pair<int, int> black_new = get_from_black();
                if (black_new.first == -1 && black_new.second == -1)//����ѡ�����
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
            if (result == R_DRAW)//��δ�ֳ�ʤ��
            {
                //���壨AI)
                turn = T_WHITE;
                GameTree my_game_tree(2, 3, cur_board);//��������������
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