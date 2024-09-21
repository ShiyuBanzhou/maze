#include "mazegenerator.h"

// 构造函数
MazeGenerator::MazeGenerator(int size)
{
    this->size = size;
    // 生成地图存取
    this->mazeMap = createMaze();
}

// 地图生成
QVector<QVector<int> > MazeGenerator::createMaze()
{
    // 初始化地图节点全0
    QVector<QVector<int>>mp(size, QVector<int>(size, 0));
    // 随机种子
    QRandomGenerator *generator = QRandomGenerator::global();

    // 随机起始点(确保起始点为奇数坐标)
    int startX = (generator->bounded((size - 1) / 2)) * 2 + 1;
    int startY = (generator->bounded((size - 1) / 2)) * 2 + 1;

    // 起点设置为路径
    mp[startX][startY] = 1;

    QStack<QPair<int, int>> st; // 用于路径回溯
    st.push(QPair<int, int>(startX, startY));

    bool isNotConnected = 1; //是否不连通

    while(isNotConnected && !st.isEmpty()){
        // 当前查询节点坐标
        int x = st.top().first;
        int y = st.top().second;
        // 查询当前节点周围复合条件的邻居集合
        QVector<QPair<int, int>> neighbors = getValidNeighbors(mp, x, y);

        if(neighbors.isEmpty()){
            st.pop(); // 没有邻居时 回溯节点
        }else{
            // 设置随机方向
            int direction = generator->bounded(neighbors.size());
            // 选择下一个联通方向
            QPair<int, int>nextPoint = neighbors[direction];

            // 打通当前查询节点与下一个节点直接的墙壁
            breakWall(mp, x, y, nextPoint.first, nextPoint.second);

            // 下一节点入栈
            mp[nextPoint.first][nextPoint.second] = 1;
            st.push(nextPoint);
        }
        //计算地图是否全部连通，考虑如果奇数行奇数列都为路则全部连通，否则没有连通
        int tempx = 0;
        isNotConnected = 0;
        for(QVector<int> y : mp)
        {
            int tempy;
            if(tempx % 2 == 1) //奇数行再遍历
            {
                tempy=0;
                for(int x : y)
                {
                    if(tempy % 2 == 1 && x == 0) //看奇数行奇数列是否为墙
                    { // 如果为墙则不连通
                        isNotConnected=1;
                        break;
                    }
                    tempy++;
                }
            }
            if(isNotConnected==1)
                break;
            tempx++;
        }
    }

    return mp;
}

// 获取地图信息
QVector<QVector<int> > MazeGenerator::getMazeMap()
{
    return this->mazeMap;
}

QPair<int, int> MazeGenerator::getEndPoint()
{
    return this->endPoint;
}

// 获取起始点
QPair<int, int> MazeGenerator::getStartPoint()
{
    return this->startPoint;
}

// 获取当前查询节点有效邻居集合
QVector<QPair<int, int> > MazeGenerator::getValidNeighbors(const QVector<QVector<int> > &mp, int x, int y)
{
    /*
     *符合要求的邻居：
     *1. 以2为步长检索当前查询节点（上下左右）四个方向的节点
     *2. 如果被访问节点的四周都是墙，则该节点复合要求，插入到邻居集合中
    */
    QVector<QPair<int, int>> neighbors;

    // 边界检测及条件判断
    if (x - 2 >= 1 && isSurroundingWalls(mp, x - 2, y)) {
        neighbors.append(QPair<int, int>(x - 2, y)); // 上方邻居
    }
    if (x + 2 < size - 1 && isSurroundingWalls(mp, x + 2, y)) {
        neighbors.append(QPair<int, int>(x + 2, y)); // 下方邻居
    }
    if (y - 2 >= 1 && isSurroundingWalls(mp, x, y - 2)) {
        neighbors.append(QPair<int, int>(x, y - 2)); // 左边邻居
    }
    if (y + 2 < size - 1 && isSurroundingWalls(mp, x, y + 2)) {
        neighbors.append(QPair<int, int>(x, y + 2)); // 右边邻居
    }

    return neighbors;
}

// 打破当前查询节点和随机选取邻居之间的墙壁
void MazeGenerator::breakWall(QVector<QVector<int> > &mp, int x1, int y1, int x2, int y2)
{
    if (x2 == x1 - 2) {
        mp[x1 - 1][y1] = 1; // 打通上方的墙
    } else if (x2 == x1 + 2) {
        mp[x1 + 1][y1] = 1; // 打通下方的墙
    } else if (y2 == y1 - 2) {
        mp[x1][y1 - 1] = 1; // 打通左边的墙
    } else if (y2 == y1 + 2) {
        mp[x1][y1 + 1] = 1; // 打通右边的墙
    }
}

// 检查当前节点是否复合邻居的要求
bool MazeGenerator::isSurroundingWalls(const QVector<QVector<int> > &mp, int x, int y)
{
    // 检查上方
    if (mp[x - 1][y] != 0) return false;
    // 检查下方
    if (mp[x + 1][y] != 0) return false;
    // 检查左方
    if (mp[x][y - 1] != 0) return false;
    // 检查右方
    if (mp[x][y + 1] != 0) return false;

    // 如果所有检查都通过，则返回 true（周围都是墙）
    return true;
}
