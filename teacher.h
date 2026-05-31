#ifndef TEACHER_H
#define TEACHER_H

#include <QDialog>
#include "myvector.h"  // 自定义模板类
#include "mymap.h"     // 可选：以后扩展用

namespace Ui {
class teacher;
}

class teacher : public QDialog
{
    Q_OBJECT

public:
    explicit teacher(QWidget *parent = nullptr);
    explicit teacher(QString id, QWidget *parent = nullptr);
    ~teacher();

private slots:
    void on_btn_close_clicked();
    void on_btn_tea_scoreInport_clicked();
    void on_btn_tea_querystu_clicked();
    void on_btn_return_clicked();
    void on_btn_tea_evaluation_clicked();

private:
    Ui::teacher *ui;
    QString currentTeacherId;

    // 教师授课列表，用自定义模板容器保存
    MyVector<QString> courseList;

    void showCurrentTeacherInfo();
};

#endif // TEACHER_H
