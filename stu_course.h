#ifndef STU_COURSE_H
#define STU_COURSE_H

#include <QDialog>
#include <QStandardItem>
#include <QStandardItemModel>

extern float sum;

namespace Ui {
class stu_course;
}

class stu_course : public QDialog
{
    Q_OBJECT

public:
    explicit stu_course(QWidget *parent = nullptr);
    explicit stu_course(QString id, QWidget *parent = nullptr);
    int readcoursefile();
    int readselectedfile();
    void reset();
    void reset1();
    void display(int row, QStringList course_line);
    void display1(int row, QStringList selected_line);
    void writeIn(QString info);
    ~stu_course();

private slots:
    void on_tb_course_doubleClicked(const QModelIndex &index);

    void on_tb_select_doubleClicked(const QModelIndex &index);

    void showAllCourses();

private:
    Ui::stu_course *ui;
    QStandardItemModel *model1;
    QStandardItemModel *model2;
    QList<QString> course_line;
    QList<QString> selected_line;
    QString currentStudentId;
    QString currentStudentName;

    QString getStudentNameById(const QString &id);
    void initWindow();
    void loadSelectedToTable();
};

#endif // STU_COURSE_H
