#include "addcourse.h"
#include "ui_addcourse.h"
#include "myvector.h"

#include "QString"
#include "QDebug"
#include "QMessageBox"
#include "QFile"
#include "QTextStream"

addcourse::addcourse(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addcourse)
{
    ui->setupUi(this);
}

addcourse::~addcourse()
{
    delete ui;
}

void addcourse::on_btn_sys_addcou_clicked()
{
    QString id = this->ui->le_course_id->text().trimmed();
    QString name = this->ui->le_course_name->text().trimmed();
    QString point = this->ui->cbb_point->currentText().trimmed();
    QString hour = this->ui->le_course_hour->text().trimmed();
    QString nature = this->ui->cbb_nature->currentText().trimmed();

    QString messagebox_out = "课程编号：" + id + "\n" +
                             "课程名称：" + name + "\n" +
                             "学分：" + point + "\n" +
                             "学时：" + hour + "\n" +
                             "课程类别：" + nature;

    // module.txt 格式：课程编号 课程名称 学分 学时 课程类别
    QString info = id + " " + name + " " + point + " " + hour + " " + nature;

    // 检查输入完整性和课程编号长度
    bool charge = id.length() != 8 ||
                  name.length() < 1 ||
                  point.length() < 1 ||
                  hour.length() < 1 ||
                  nature.length() < 1;

    if(charge)
    {
        QMessageBox::critical(this, "错误", "信息填写不完整，请检查；课程编号应为8位", "确定");
    }
    else
    {
        int ret = QMessageBox::question(this, "请确认", messagebox_out, "确认", "取消");
        if(ret == 0)
        {
            writeIn(info);
        }
    }
}

void addcourse::writeIn(QString info)
{
    this->ui->le_course_id->clear();
    this->ui->le_course_name->clear();
    this->ui->le_course_hour->clear();
    this->ui->cbb_point->setCurrentIndex(0);
    this->ui->cbb_nature->setCurrentIndex(0);
    this->ui->le_course_id->setFocus();

    QFile file("module.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "课程文件打开失败，信息没有写入", "确认");
        return;
    }

    // 使用模板容器保存文件所有行
    MyVector<QString> lines;

    QTextStream in(&file);
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    file.close();

    int endIndex = -1;

    // 查找 #END，将新增课程插入到 #END 前
    for (int i = 0; i < lines.size(); ++i) {
        if (lines.at(i).trimmed() == "#END") {
            endIndex = i;
            break;
        }
    }

    if (endIndex == -1) {
        lines.append(info);
        lines.append("#END");
    } else {
        lines.insert(endIndex, info);
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, "错误", "课程文件写入失败，信息没有写入", "确认");
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < lines.size(); ++i) {
        out << lines.at(i) << "\n";
    }

    file.close();

    QMessageBox::information(this, "成功", "课程信息添加成功！", "确认");
}

void addcourse::on_btn_sys_addcoucancel_clicked()
{
    this->close();
}
