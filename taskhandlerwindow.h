#ifndef TASKHANDLERWINDOW_H
#define TASKHANDLERWINDOW_H

#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include <QWidget>
#include <QRandomGenerator>
#include <QString>
#include <QTextStream>

namespace Ui {
class TaskHandlerWindow;
}

class TaskHandlerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TaskHandlerWindow(QWidget *parent = nullptr);
    ~TaskHandlerWindow();

private:
    Ui::TaskHandlerWindow *ui;
    QLabel *questionLabel;
    QLineEdit *answerInput;
    QPushButton *submitButton;
    QLabel *resultLabel;
    // 生成随机的数学题目
    void generateNewTask();
    // 获取用户输入的答案
    int getUserAnswer(const QString& question);
    // 任务处理方法，接收并验证用户答案
    bool handleTask();
    int num1;
    int num2;
    QString operation;
    int correctAnswer;
    int generateMathQuestion(QString& question);
    bool checkAnswer(int userAnswer, int correctAnswer);
signals:
    void correct();
private slots:
    void onSubmitButtonClicked();
};

#endif // TASKHANDLERWINDOW_H
