#ifndef TEACHER_H
#define TEACHER_H

#include <QDialog>

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

    void showCurrentTeacherInfo();
};

#endif // TEACHER_H
