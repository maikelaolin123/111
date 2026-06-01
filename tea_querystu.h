#ifndef TEA_QUERYSTU_H
#define TEA_QUERYSTU_H

#include <QDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include "myvector.h"
#include "mymap.h"

extern QString id1;
extern QString name1;
extern QString courseName1;
extern QString score1;

namespace Ui {
class tea_querystu;
}

class tea_querystu : public QDialog
{
    Q_OBJECT

public:
    explicit tea_querystu(QWidget *parent = nullptr);

    int readfile();
    int readStudentFile();

    void reset();
    void display(int row, QStringList score_line);

    ~tea_querystu();

private slots:
    void on_btn_doquery_clicked();
    void on_btn_statistics_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);

public:
    Ui::tea_querystu *ui;
    QStandardItemModel *model;

    // 用模板类容器保存成绩记录行
    MyVector<QString> score_line;

    // 用模板类容器保存学号 -> 班级
    MyMap<QString, QString> studentClassMap;
};

#endif // TEA_QUERYSTU_H
