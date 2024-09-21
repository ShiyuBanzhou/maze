#ifndef USER_H
#define USER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class User{
public:
    User(const QString &username, const QString &password);
    // 静态函数：初始化用户信息文件
    static bool initializeUserFile();
    // 处理用户信息
    bool isRegistered(); // 检查用户是否已注册
    bool login();  // 登录，验证用户名和密码
    bool registerUser(); // 注册用户
    void setUsername(QString &username); // 设置用户名
    void setPassword(QString &password); // 设置密码
    int getMaxAttempts(); // 获取最大尝试次数

private:
    QString username;
    QString password;
    static QString usersFilePath; // 存储用户信息文件地址
    int loginAttempts;      // 登录错误次数
    const int maxAttempts = 3; // 最大尝试次数
};

#endif // USER_H
