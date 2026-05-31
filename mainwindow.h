#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "myvector.h"

extern QString account;
extern QString password;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    int readstudentfile();
    int readteacherfile();
    int readcontroller();
    ~MainWindow();

private slots:
    void on_btn_denglu_clicked();

    void on_btn_tuichu_clicked();

private:
    Ui::MainWindow *ui;

    // 保存 student.txt 文件行
    MyVector<QString> student_line;

    // 保存 staff.txt 文件行
    MyVector<QString> teacher_line;

    // 保存 controller.txt 文件行
    MyVector<QString> controller_line;

    bool checkStudentLogin(const QString &account, const QString &password);
};

#endif // MAINWINDOW_H
