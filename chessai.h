#ifndef CHESSAI_H
#define CHESSAI_H

#include<iostream>
#include<string.h>
#include<memory.h>
#include<set>
#include<deque>
#include<vector>
using namespace std;

/*���ӣ����ӣ����ӣ�����*/
#define C_NONE 0
#define C_BLACK 1
#define C_WHITE 2

/*�������ҡ����ϡ����ϡ�����*/
#define RIGHT 0
#define UP 1
#define UPRIGHT 2
#define UPLEFT 3

/*�������͵Ĵ���*/
#define OTHER 0//��������
#define WIN 1//��Ӯ
#define LOSE 2//��Ӯ
#define FLEX4 3//�׻�4
#define flex4 4//�ڻ�4
#define BLOCK4 5//����4
#define block4 6//����4
#define FLEX3 7//�׻�3
#define flex3 8//�ڻ�3
#define BLOCK3 9//����3
#define block3 10//����3
#define FLEX2 11//�׻�2
#define flex2 12//�ڻ�2
#define BLOCK2 13//����2
#define block2 14//����2
#define FLEX1 15//�׻�1
#define flex1 16//�ڻ�1

/*��Ϸ״̬��������/�ѽ���*/
#define UNDERWAY 0
#define FINISH 1

/*�ֵ�˭�£�����/����*/
#define T_BLACK 0
#define T_WHITE 1

/*�������������Ӯ������Ӯ�����壨��δ�ֳ�ʤ����*/
#define R_BLACK 0
#define R_WHITE 1
#define R_DRAW 2

class Node //���ṹ��
{
public:
    int value;//��ǰ����ֵ
    int depth;//��ǰ�����ȣ���������Ϊ0��
    Node* father;//�����
    set<Node*> children;//�ӽ�㼯��
    int cntX, cntY;//���һ�����ӵĺ�������
    int board[15][15]{};//��ǰ����
public:
    Node();//�޲ι��캯��
    Node(Node* node, int opeX, int opeY);//���ø���㹹���ӽ��

    bool is_max_node();//�жϵ�ǰ����Ƿ�ΪMAX���
    int get_score(); //��ǰ�������֣�������Ҷ�ӽ������ֵ��,�ܹ��ж���Ӯ
};

class GameTree //�������ṹ��
{
public:
    int radius;//��չ����
    int maxDepth;//������
    Node* root;//���ڵ�
    Node* best = NULL;//ָ����Ѻ�̽��
    deque<Node*> openTable;//˫����У���Ŵ���չ���
public:
    GameTree(int maxDepth, int expandRadius, int(&board)[15][15]);//ָ�������Ⱥ���չ���룬��ǰ�������ӷֲ����첩����
    vector<pair<int, int>> get_search_nodes(Node* node);//���ص�ǰ��ֵĴ���չ�����꼯��
    int expand_children_nodes(Node* node); //��չnode�ڵ㣬����node���������ӽ��
    bool is_alpha_beta_cut(Node* node);//�жϵ�ǰ����Ƿ��ܽ���ab��֦
    void update_value_from_node(Node* node);//���·�Ҷ�ӽ��Ĺ�ֵ
    void set_next_pos();//ȷ����һ�������λ��
    void game_tree();//�������������������̵ĺ��ĺ���
};


class Play
{
private:
    int cur_board[15][15];//��ǰ����״̬
    vector<pair<int, int>> path;//��¼ÿ����ӵ������ԣ����ڻ������
    int turn;//��ǰ�ֵ�������/���壩
    int status;//��ǰ״̬����Ϸ������/��Ϸ������
    int result;//��Ϸ���������ʤ/����ʤ/ƽ�֣�
public:
    Play();
    void init_tuple6type();//��ʼ����Ԫ��
    pair<int, int> get_from_black();//���û������ȡ������һ��λ��
    void display_board();//չʾ��ǰ����״̬
    int game();//����������Ϸ�ĺ��ĺ���
};

#endif // CHESSAI_H