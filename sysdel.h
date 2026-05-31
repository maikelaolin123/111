#ifndef SYSDEL_H
#define SYSDEL_H

#include <QDialog>
#include <QString>
#include "myvector.h"

extern QString ID;

namespace Ui {
class sysdel;
}

class sysdel : public QDialog
{
    Q_OBJECT

public:
    explicit sysdel(QWidget *parent = nullptr);

    int readstudentfile();
    int readteacherfile();
    int readcoursefile();

    void deletestudent();
    void deleteteacher();
    void deletecourse();

private slots:
    void on_btn_del_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::sysdel *ui;

    // 保存 student.txt 文件行
    MyVector<QString> student_line;

    // 保存 staff.txt 文件行
    MyVector<QString> teacher_line;

    // 保存 module.txt 文件行
    MyVector<QString> course_line;
};

#endif // SYSDEL_H
