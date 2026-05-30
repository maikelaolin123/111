#ifndef STUDENT_H
#define STUDENT_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QMap>

namespace Ui {
class student;
}

class student : public QDialog
{
    Q_OBJECT

public:
    explicit student(QWidget *parent = nullptr);
    // 新增：带当前登录学生学号的构造函数
    explicit student(QString id, QWidget *parent = nullptr);
    int readfile();
    int readCourseCreditFile();
    int readTeacherFile();
    ~student();

private slots:
    void on_btn_stu_query_clicked();

    void on_btn_stu_select_clicked();

    void on_pushButton_clicked();

    void on_btn_return_clicked();

    void on_btn_stu_evaluate_clicked();

private:
    Ui::student *ui;

    // 原来的成绩文件行缓存
    QList<QString> score_line;
    // 新增：保存当前登录学生的学号
    QString currentStudentId;
    // 课程名称 -> 学分
    QMap<QString, double> courseCreditMap;
    // 课程名称 -> 教师工号
    QMap<QString, QString> courseTeacherIdMap;
    // 课程名称 -> 教师姓名
    QMap<QString, QString> courseTeacherNameMap;

    void showCurrentStudentInfo();
};

#endif // STUDENT_H
