#ifndef MAPDATA_H
#define MAPDATA_H

#include <QString>

class MapData{
public:
    static const int maxSize = 31;
    /*
     * 地图容纳最大空间
     * 0:墙
     * 1:普通道路
     * 2:起点
     * 3:终点
    */
    int mazeMap[maxSize][maxSize];

    // 构造函数
    MapData();
    MapData(const int &mapSize, const int mazeMap[31][31]);
    void loadMap(); // 加载地图
    void saveMap(); // 保存地图
    int getMapSize(); // 获取地图尺寸
    void setMapPath(QString mapPath); // 设置地图路径
    void setTaskPointCount(int taskPointCount);
    int getTaskPointCount();
    void setIsFinished(int isFinished);
    int getIsFinished();
    int startX, startY;

private:
    int mapSize = 0; // 地图大小
    int taskPointCount;
    int isFinished = 0;
    QString mapPath; // 地图路径
};

#endif // MAPDATA_H
