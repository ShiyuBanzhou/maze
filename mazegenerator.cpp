#include "mazegenerator.h"
#include "qdebug.h"

#include <QQueue>

// 构造函数
MazeGenerator::MazeGenerator(int size)
{
    this->size = size;
    // 生成地图存取
    this->mazeMap = createMaze();
}

// 生成任务点
void MazeGenerator::generateTaskPoints()
{
    QRandomGenerator *generator = QRandomGenerator::global();
    QVector<QPair<int, int>> validPoints;
    taskPointCount = generator->bounded(1, size / 3 + 1);  // 生成1到size/3之间的随机数;
    qDebug() << QString::number(taskPointCount);
    // 遍历迷宫，寻找所有可达的奇数坐标点
    for (int x = 1; x < size; x += 2) {
        for (int y = 1; y < size; y += 2) {
            if (mazeMap[x][y] == 1 && (x != startPoint.first || y != startPoint.second) && (x != endPoint.first || y != endPoint.second)) {
                validPoints.append(QPair<int, int>(x, y));
            }
        }
    }
    // 随机选择任务点
    for (int i = 0; i < taskPointCount && !validPoints.isEmpty(); ++i) {
        int index = generator->bounded(validPoints.size());
        QPair<int, int> taskPoint = validPoints[index];
        mazeMap[taskPoint.first][taskPoint.second] = 5; // 用数字5标记任务点
        validPoints.remove(index); // 移除已选择的任务点，避免重复
    }
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

// 广度优先搜索（BFS），查找距离起点最远的点
QPair<int, int> MazeGenerator::findFarthestPoint(const QVector<QVector<int>>& mp, QPair<int, int> start)
{
    int rows = mp.size();
    int cols = mp[0].size();

    // BFS 使用的队列，存储当前位置和当前距离
    QQueue<QPair<QPair<int, int>, int>> queue;  // (坐标, 当前距离)

    // 初始化访问数组，标记每个点是否访问过
    QVector<QVector<bool>> visited(rows, QVector<bool>(cols, false));

    // 起点入队
    queue.enqueue(qMakePair(start, 0));  // 起点和初始距离0
    visited[start.first][start.second] = true;

    // 最远点和距离
    QPair<int, int> farthestPoint = start;
    int maxDistance = 0;

    // BFS 遍历
    QVector<QPair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};  // 上下左右方向
    while (!queue.isEmpty()) {
        QPair<QPair<int, int>, int> current = queue.dequeue();
        QPair<int, int> currentPos = current.first;
        int currentDist = current.second;

        // 如果当前点是路径且未被访问过
        if (mp[currentPos.first][currentPos.second] == 1) {
            // 更新最远点
            if (currentDist > maxDistance) {
                maxDistance = currentDist;
                farthestPoint = currentPos;
            }

            // 遍历邻居
            for (const auto& direction : directions) {
                int newX = currentPos.first + direction.first;
                int newY = currentPos.second + direction.second;

                // 确保新位置在地图范围内且未被访问过
                if (newX >= 0 && newX < rows && newY >= 0 && newY < cols &&
                    !visited[newX][newY] && mp[newX][newY] == 1) {
                    visited[newX][newY] = true;
                    queue.enqueue(qMakePair(QPair<int, int>(newX, newY), currentDist + 1));
                }
            }
        }
    }

    return farthestPoint;  // 返回最远的点
}

// 生成随机起点和终点
void MazeGenerator::generateStartAndEndPoints(bool isTaskMaze)
{
    QRandomGenerator *generator = QRandomGenerator::global();
    // 随机起点（已经在创建迷宫时生成）
    startPoint = QPair<int, int>(1, 1); // 起点可以是固定值，也可以从生成的路径中选择
    // 使用BFS查找最远点作为终点
    endPoint = findFarthestPoint(mazeMap, startPoint);
    if(!isTaskMaze){
        mazeMap[startPoint.first][startPoint.second] = 2;
    }
    mazeMap[endPoint.first][endPoint.second] = 3;

    // 随机选取一个有效的点作为任务迷宫入口
    QVector<QPair<int, int>> validPoints;

    // 遍历迷宫，寻找所有可达的奇数坐标点
    for (int x = 1; x < size; x += 2) {
        for (int y = 1; y < size; y += 2) {
            if (mazeMap[x][y] == 1
                && (x != startPoint.first || y != startPoint.second)
                && (x != endPoint.first || y != endPoint.second)\
                && (mazeMap[x][y] != 5)) {
                validPoints.append(QPair<int, int>(x, y));
            }
        }
    }

    if (!validPoints.isEmpty()) {
        int index = generator->bounded(validPoints.size());
        QPair<int, int> taskMazeEntrance = validPoints[index];
        mazeMap[taskMazeEntrance.first][taskMazeEntrance.second] = 4;  // 用数字4标记任务迷宫的入口
    }
}

int MazeGenerator::getTaskPointCount()
{
    return taskPointCount;
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
