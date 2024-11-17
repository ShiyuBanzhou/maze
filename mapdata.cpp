#include "mapdata.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

MapData::MapData()
{
    // 初始化地图数组
    for(int i = 0; i < this->maxSize; i++){
        for(int j = 0; j < this->mapSize; j++){
            this->mazeMap[i][j] = 0;
        }
    }
}

MapData::MapData(const int &mapSize, const int mazeMap[31][31])
{
    this->mapSize = mapSize;
    // 初始化地图数组
    for(int i = 0; i < mapSize; i++){
        for(int j = 0; j < mapSize; j++){
            this->mazeMap[i][j] = 0;
        }
    }
    // 地图赋值
    for(int i = 0; i < mapSize; i++){
        for(int j = 0; j < mapSize; j++){
            this->mazeMap[i][j] = mazeMap[i][j];
        }
    }
}

// 加载地图
void MapData::loadMap()
{
    QFile data(this->mapPath);
    if (data.isOpen()) {
        data.close(); // 先关闭再重新打开
    }
    if (!data.open(QFile::ReadOnly)) {
        // 处理文件打开失败
        qDebug() << "无法打开文件:" << this->mapPath;
        return;
    }
    if (data.isOpen()) {
        data.close(); // 先关闭再重新打开
    }
    if (data.open(QFile::ReadOnly))
    {
        QTextStream in(&data);
        this->mapSize = in.readLine().toInt();
        for(int i = 0; i < mapSize; i++)
        {
            for(int j = 0; j < mapSize; j++)
            {
                mazeMap[i][j] = in.read(1).toInt();
            }
            // 跳过行结尾回车 用于换行
            in.read(1);
        }
    }
}

// 保存地图
void MapData::saveMap()
{

    QFile data(this->mapPath);
    if(data.open(QFile::WriteOnly | QIODevice::Truncate)){
        /* QIODevice::Truncate
         * 打开的文件若已存在，则截取其文件长度为0
         * 即清空已存在文件内容
        */
        QTextStream out(&data);
        out << this->mapSize << "\n";
        for(int i = 0; i < mapSize; i++){
            for(int j = 0; j < mapSize; j++){
                out << mazeMap[i][j];
            }
            out << "\n";
        }
    }
}

// 获取地图尺寸
int MapData::getMapSize()
{
    return this->mapSize;
}

// 设置地图路径
void MapData::setMapPath(QString mapPath)
{
    this->mapPath = mapPath;
}

void MapData::setTaskPointCount(int taskPointCount)
{
    this->taskPointCount = taskPointCount;
}

int MapData::getTaskPointCount()
{
    return this->taskPointCount;
}

void MapData::setIsFinished(int isFinished)
{
    this->isFinished = isFinished;
}

int MapData::getIsFinished()
{
    return this->isFinished;
}
