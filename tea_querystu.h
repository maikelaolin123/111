#ifndef TEA_QUERYSTU_H
#define TEA_QUERYSTU_H

#include <QDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMap>
#include <QStringList>

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
    QList<QString> score_line;

    // 学号 -> 班级
    QMap<QString, QString> studentClassMap;
};

#endif // TEA_QUERYSTU_H
