#ifndef TILETEXTURE_H
#define TILETEXTURE_H

#include <QPushButton>
#include <QString>
#include <map>

// 枚举定义
enum TileStatus {
    WALL = 0,   // 0:墙
    PATH,       // 1:路
    STARTING,   // 2:起点
    ENDING,      // 3:终点
    TILE_STATUS_COUNT // 总状态数量
};

class TileTexture : public QPushButton
{
public:
    // 构造方法
    TileTexture(TileStatus status);
    void setSize(int Size);
    void changeStatus(); // 循环切换
    void changeStatus(TileStatus status); // 指定状态
private:
    TileStatus status; // 当前状态
    std::map<TileStatus, QString> statusToPath; // 状态到路径的映射
};

#endif // TILETEXTURE_H
