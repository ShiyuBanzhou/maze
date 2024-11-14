#include "mazegenerator.h"

// 构造函数
MazeGenerator::MazeGenerator(int size)
{
    this->size = size;
    // 生成地图存取
    this->mazeMap = createMaze();
    // 生成随机起点和终点
    generateStartAndEndPoints();
}

// 地图生成
QVector<QVector<int>> MazeGenerator::createMaze()
{
    // 初始化地图节点全0
    QVector<QVector<int>> mp(size, QVector<int>(size, 0));
    // 随机种子
    QRandomGenerator *generator = QRandomGenerator::global();

    // 随机起始点(确保起始点为奇数坐标)
    int startX = (generator->bounded((size - 1) / 2)) * 2 + 1;
    int startY = (generator->bounded((size - 1) / 2)) * 2 + 1;

    // 起点设置为路径
    mp[startX][startY] = 1;

    QStack<QPair<int, int>> st; // 用于路径回溯
    st.push(QPair<int, int>(startX, startY));

    bool isNotConnected = true; // 是否不连通

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
            QPair<int, int> nextPoint = neighbors[direction];

            // 打通当前查询节点与下一个节点直接的墙壁
            breakWall(mp, x, y, nextPoint.first, nextPoint.second);

            // 下一节点入栈
            mp[nextPoint.first][nextPoint.second] = 1;
            st.push(nextPoint);
        }

        // 检查地图的连通性：如果所有奇数行和奇数列的单元格都是路径（1），则地图连通；否则，不连通。
        int tempx = 0;
        isNotConnected = false;
        for(QVector<int> y : mp)
        {
            int tempy;
            if(tempx % 2 == 1) // 奇数行索引
            {
                tempy=0;
                for(int x : y)
                {
                    if(tempy % 2 == 1 && x == 0) // 看奇数行奇数列是否为墙
                    { // 如果为墙则不连通
                        isNotConnected = true;
                        break;
                    }
                    tempy++;
                }
            }
            if(isNotConnected == true)
                break;
            tempx++;
        }
    }

    return mp;
}

// 生成随机起点和终点
void MazeGenerator::generateStartAndEndPoints()
{
    QRandomGenerator *generator = QRandomGenerator::global();

    // 随机起点（已经在创建迷宫时生成）
    startPoint = QPair<int, int>(1, 1); // 起点可以是固定值，也可以从生成的路径中选择

    // 生成随机终点，确保终点与起点有足够的距离
    int endX, endY;
    const int MIN_DISTANCE = 10; // 起点和终点的最小曼哈顿距离

    // 如果迷宫尺寸非常小，放宽曼哈顿距离限制
    int maxDistance = (size < 5) ? 1 : MIN_DISTANCE;

    do {
        endX = (generator->bounded((size - 1) / 2)) * 2 + 1; // 随机选择奇数行
        endY = (generator->bounded((size - 1) / 2)) * 2 + 1; // 随机选择奇数列
    } while (size > 3 && distance(startPoint.first, startPoint.second, endX, endY) < maxDistance); // 确保距离足够大

    endPoint = QPair<int, int>(endX, endY);
    mazeMap[startPoint.first][startPoint.second] = 2;
    mazeMap[endPoint.first][endPoint.second] = 3;
}

// 获取起点和终点之间的曼哈顿距离
int MazeGenerator::distance(int x1, int y1, int x2, int y2)
{
    return qAbs(x1 - x2) + qAbs(y1 - y2); // 曼哈顿距离
}

// 获取地图信息
QVector<QVector<int>> MazeGenerator::getMazeMap()
{
    return this->mazeMap;
}

// 获取起点
QPair<int, int> MazeGenerator::getStartPoint()
{
    return this->startPoint;
}

// 获取终点
QPair<int, int> MazeGenerator::getEndPoint()
{
    return this->endPoint;
}

// 获取当前查询节点有效邻居集合
QVector<QPair<int, int>> MazeGenerator::getValidNeighbors(const QVector<QVector<int>>& mp, int x, int y)
{
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
void MazeGenerator::breakWall(QVector<QVector<int>>& mp, int x1, int y1, int x2, int y2)
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
bool MazeGenerator::isSurroundingWalls(const QVector<QVector<int>>& mp, int x, int y)
{
    // 检查上方
    if (mp[x - 1][y] != 0) return false;
    // 检查下方
    if (mp[x + 1][y] != 0) return false;
    // 检查左方
    if (mp[x][y - 1] != 0) return false;
    // 检查右方
    if (mp[x][y + 1] != 0) return false;

    return true;
}
