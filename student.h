#ifndef STUDENT_H
#define STUDENT_H

#include <QDialog>
#include <QString>

#include "myvector.h"
#include "mymap.h"

namespace Ui {
class student;
}

class student : public QDialog
{
    Q_OBJECT

public:
    explicit student(QWidget *parent = nullptr);
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

    MyVector<QString> score_line;

    QString currentStudentId;

    MyMap<QString, double> courseCreditMap;

    MyMap<QString, QString> courseTeacherIdMap;

    MyMap<QString, QString> courseTeacherNameMap;

    void showCurrentStudentInfo();
};

#endif // STUDENT_H
