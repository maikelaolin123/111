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

tea_scoreimport::tea_scoreimport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tea_scoreimport)
{
    ui->setupUi(this);

    this->model = new QStandardItemModel;
    this->ui->tableView->setModel(model);

    reset();

    // 读取成绩文件
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

// 初始化表格表头
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

// 读取 score.txt 并保存到模板容器
int tea_scoreimport::readfile()
{
    score_line.clear();
    this->model->clear();
    reset();

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
        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList subs = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (subs.length() < 4)
            continue;

        // 保存业务数据到模板容器
        score_line.append(line);

        // 显示到表格
        this->model->setItem(row, 0, new QStandardItem(subs.at(0)));
        this->model->setItem(row, 1, new QStandardItem(subs.at(1)));
        this->model->setItem(row, 2, new QStandardItem(subs.at(2)));
        this->model->setItem(row, 3, new QStandardItem(subs.at(3)));

        row++;
    }

    file.close();
    return 0;
}

// 点击排序按钮，按选择列排序
void tea_scoreimport::on_btn_dosort_clicked()
{
    if (readfile() == -1)
    {
        QMessageBox::critical(this, "错误", "score.txt 读取失败！", "确认");
        return;
    }

    int flag = this->ui->cbb_sortway->currentIndex();

    // 使用模板容器保存每行的字段列表
    MyVector<QStringList> records;

    for (int i = 0; i < score_line.size(); i++)
    {
        QString line = score_line.at(i).trimmed();
        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 4)
            continue;

        records.append(list);
    }

    // 排序 lambda
    std::sort(records.begin(), records.end(),
              [flag](const QStringList &a, const QStringList &b)
    {
        switch (flag)
        {
        case 0:
            return a.at(0) < b.at(0); // 按学号升序
        case 1:
            return a.at(2) < b.at(2); // 按课程名称升序
        default:
            return a.at(0) < b.at(0);
        }
    });

    this->model->clear();
    reset();

    // 显示排序结果
    for (int row = 0; row < records.size(); row++)
    {
        QStringList list = records.at(row);
        this->model->setItem(row, 0, new QStandardItem(list.at(0))); // 学号
        this->model->setItem(row, 1, new QStandardItem(list.at(1))); // 姓名
        this->model->setItem(row, 2, new QStandardItem(list.at(2))); // 课程名称
        this->model->setItem(row, 3, new QStandardItem(list.at(3))); // 成绩
    }
}
