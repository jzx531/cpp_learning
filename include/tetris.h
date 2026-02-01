#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h> // 用于 FILE
#include <stdbool.h> // 用于 bool

/*
  将一个俄罗斯方块类型（tetromino type）转换为其对应的单元格值。
 */
#define TYPE_TO_CELL(x) ((x)+1)

/*
  用于打印俄罗斯方块游戏板的字符串表示。
 */
#define TC_EMPTY_STR " "
#define TC_BLOCK_STR "\u2588"

/*
  判断一个俄罗斯方块单元格的性质。
 */
#define TC_IS_EMPTY(x) ((x) == TC_EMPTY)
#define TC_IS_FILLED(x) (!TC_IS_EMPTY(x))

/*
  一个俄罗斯方块（tetromino）包含多少个单元格？
 */
#define TETRIS 4

/*
  共有多少种不同的俄罗斯方块？
 */
#define NUM_TETROMINOS 7

/*
  每种俄罗斯方块有多少种旋转方向（朝向）？
 */
#define NUM_ORIENTATIONS 4

/*
  游戏关卡相关常量。
 */
#define MAX_LEVEL 19
#define LINES_PER_LEVEL 10

/*
  “单元格”（cell）是俄罗斯方块游戏板中的一个 1x1 方块。
 */
typedef enum {
  TC_EMPTY, TC_CELLI, TC_CELLJ, TC_CELLL, TC_CELLO, TC_CELLS, TC_CELLT, TC_CELLZ
} tetris_cell;

/*
  “类型”（type）指俄罗斯方块的形状种类（不包括旋转方向）。
 */
typedef enum {
  TET_I, TET_J, TET_L, TET_O, TET_S, TET_T, TET_Z
} tetris_type;

/*
  一个行、列坐标对。允许使用负数，因为偏移计算中需要用到。
 */
typedef struct {
  int row;
  int col;
} tetris_location;

/*
  “方块”（block）是一个结构体，包含关于一个俄罗斯方块的信息：
  具体是什么类型、当前朝向如何、以及它在游戏板上的位置。
 */
typedef struct {
  int typ;
  int ori;
  tetris_location loc;
} tetris_block;

/*
  所有可能的游戏操作，可作为输入传给游戏。
 */
typedef enum {
  TM_LEFT,    // 向左移动
  TM_RIGHT,   // 向右移动
  TM_CLOCK,   // 顺时针旋转
  TM_COUNTER, // 逆时针旋转
  TM_DROP,    // 瞬间下落
  TM_HOLD,    // 保留/交换当前方块
  TM_NONE     // 无操作
} tetris_move;

/*
  游戏对象！
 */
typedef struct {
  /*
    游戏板相关数据：
   */
  int rows;          // 行数
  int cols;          // 列数
  char *board;       // 游戏板数据（一维数组模拟二维）

  /*
    得分信息：
   */
  int points;        // 当前得分
  int level;         // 当前关卡

  /*
    falling：当前正在下落的方块。
    next：下一个即将出现的方块。
    stored：玩家可以交换使用的保留方块。
   */
  tetris_block falling;
  tetris_block next;
  tetris_block stored;

  /*
    距离方块自动下落还剩多少游戏“滴答”（ticks）。
   */
  int ticks_till_gravity;

  /*
    距离升级还需消除多少行。
   */
  int lines_remaining;
} tetris_game;

/*
  该数组存储了每种俄罗斯方块在各种朝向下的所有单元格位置信息。
  第一维索引表示方块类型（如 I、J、Z 等），
  第二维索引表示朝向（0-3），
  第三维数组包含 4 个 tetris_location 对象，
  每个对象表示相对于该方块“原点”（通常为左上角）的偏移量。
 */
extern tetris_location TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS];

/*
  该数组表示每个关卡对应的重力速度（即每多少 tick 下落一次）。
  随着关卡提升，数值减小，难度增加。
 */
extern int GRAVITY_LEVEL[MAX_LEVEL+1];

// 数据结构操作函数。
void tg_init(tetris_game *obj, int rows, int cols);        // 初始化游戏对象
tetris_game *tg_create(int rows, int cols);                // 创建并返回新游戏对象
void tg_destroy(tetris_game *obj);                         // 销毁游戏对象（释放内部资源）
void tg_delete(tetris_game *obj);                          // 删除整个游戏对象（含自身内存）
tetris_game *tg_load(FILE *f);                             // 从文件加载游戏状态
void tg_save(tetris_game *obj, FILE *f);                   // 将游戏状态保存到文件

// 与内存管理无关的公共方法：
char tg_get(tetris_game *obj, int row, int col);           // 获取指定位置的单元格值
bool tg_check(tetris_game *obj, int row, int col);         // 检查指定位置是否有效且非空
bool tg_tick(tetris_game *obj, tetris_move move);          // 执行一帧游戏逻辑（处理输入和重力）
void tg_print(tetris_game *obj, FILE *f);                  // 将游戏板打印到指定文件（如 stdout）

#endif // TETRIS_H