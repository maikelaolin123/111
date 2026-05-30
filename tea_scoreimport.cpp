#include "tea_scoreimport.h"
#include "ui_tea_scoreimport.h"
#include "QFile"
#include "QTextStream"
#include "QStringList"
#include "QDebug"
#include "QMessageBox"
#include <QRegExp>
#include <QStandardItem>
#include <algorithm>
#include <QVector>

tea_scoreimport::tea_scoreimport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tea_scoreimport)
{
    ui->setupUi(this);

    this->model = new QStandardItemModel;
    this->ui->tableView->setModel(model);

    reset();

    if (readfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "文件读取失败", "确认");
    }
}

tea_scoreimport::~tea_scoreimport()
{
    delete ui;
}

void tea_scoreimport::reset()
{
    this->model->setHorizontalHeaderItem(0, new QStandardItem("学号"));
    this->model->setHorizontalHeaderItem(1, new QStandardItem("姓名"));
    this->model->setHorizontalHeaderItem(2, new QStandardItem("课程名称"));
    this->model->setHorizontalHeaderItem(3, new QStandardItem("成绩"));

    this->ui->tableView->setColumnWidth(0, 120);
    this->ui->tableView->setColumnWidth(1, 80);
    this->ui->tableView->setColumnWidth(2, 220);
    this->ui->tableView->setColumnWidth(3, 80);
}

int tea_scoreimport::readfile()
{
    score_line.clear();

    this->model->clear();

    this->model->setHorizontalHeaderItem(0, new QStandardItem("学号"));
    this->model->setHorizontalHeaderItem(1, new QStandardItem("姓名"));
    this->model->setHorizontalHeaderItem(2, new QStandardItem("课程名称"));
    this->model->setHorizontalHeaderItem(3, new QStandardItem("成绩"));

    QFile file("score.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    int row = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line.startsWith("#"))
            continue;

        QStringList subs = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // score.txt 新格式：学号 姓名 课程名称 成绩
        if (subs.length() < 4)
            continue;

        score_line.append(line);

        this->model->setItem(row, 0, new QStandardItem(subs.at(0)));
        this->model->setItem(row, 1, new QStandardItem(subs.at(1)));
        this->model->setItem(row, 2, new QStandardItem(subs.at(2)));
        this->model->setItem(row, 3, new QStandardItem(subs.at(3)));

        row++;
    }

    file.close();
    return 0;
}

void tea_scoreimport::on_btn_dosort_clicked()
{
    if (readfile() == -1)
    {
        QMessageBox::critical(this, "错误", "score.txt 读取失败！", "确认");
        return;
    }

    int flag = this->ui->cbb_sortway->currentIndex();

    QVector<QStringList> records;

    for (int i = 0; i < score_line.length(); i++)
    {
        QString line = score_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // score.txt 格式：学号 姓名 课程名称 成绩
        if (list.size() < 4)
            continue;

        records.append(list);
    }

    std::sort(records.begin(), records.end(),
              [flag](const QStringList &a, const QStringList &b)
    {
        switch (flag)
        {
        case 0:
            // 按学号升序
            return a.at(0) < b.at(0);

        case 1:
            // 按课程名称升序
            return a.at(2) < b.at(2);

        default:
            return a.at(0) < b.at(0);
        }
    });

    this->model->clear();
    reset();

    for (int row = 0; row < records.size(); row++)
    {
        QStringList list = records.at(row);

        this->model->setItem(row, 0, new QStandardItem(list.at(0))); // 学号
        this->model->setItem(row, 1, new QStandardItem(list.at(1))); // 姓名
        this->model->setItem(row, 2, new QStandardItem(list.at(2))); // 课程名称
        this->model->setItem(row, 3, new QStandardItem(list.at(3))); // 成绩
    }
}
