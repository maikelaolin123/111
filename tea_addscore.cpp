#include "tea_addscore.h"
#include "ui_tea_addscore.h"
#include "myvector.h"

#include "QString"
#include "QFile"
#include "QTextStream"
#include "QMessageBox"

tea_addscore::tea_addscore(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tea_addscore)
{
    ui->setupUi(this);
}

tea_addscore::~tea_addscore()
{
    delete ui;
}

void tea_addscore::on_btn_addscore_add_clicked()
{
    QString name = this->ui->le_addscore_name->text().trimmed();
    QString id = this->ui->le_addscore_id->text().trimmed();
    QString courseName = this->ui->le_addscore_class->text().trimmed();
    QString score = this->ui->le_addscore_math->text().trimmed();

    if (name.isEmpty() || id.isEmpty() || courseName.isEmpty() || score.isEmpty())
    {
        QMessageBox::critical(this, "错误", "信息填写不完整，请检查！", "确定");
        return;
    }

    // 学号按当前测试数据规则检查，避免录入明显错误账号
    if (id.length() != 11)
    {
        QMessageBox::critical(this, "错误", "学号格式错误，请检查！", "确定");
        return;
    }

    bool ok = false;
    int scoreValue = score.toInt(&ok);

    if (!ok || scoreValue < 0 || scoreValue > 100)
    {
        QMessageBox::critical(this, "错误", "成绩必须是 0 到 100 之间的整数！", "确定");
        return;
    }

    QString messagebox_out =
            "学号：" + id + "\n" +
            "姓名：" + name + "\n" +
            "课程名称：" + courseName + "\n" +
            "成绩：" + score + "\n";

    // score.txt 格式：学号 姓名 课程名称 成绩
    QString information = id + "\t" + name + "\t" + courseName + "\t" + score;

    int ret = QMessageBox::question(this, "请确认", messagebox_out, "确认", "取消");

    if (ret == 0)
    {
        writeIn(information);
        QMessageBox::information(this, "通知", "成绩录入成功！", "确认");
    }
}

void tea_addscore::writeIn(QString information)
{
    this->ui->le_addscore_name->clear();
    this->ui->le_addscore_id->clear();
    this->ui->le_addscore_class->clear();
    this->ui->le_addscore_math->clear();

    this->ui->le_addscore_name->setFocus();

    QFile file("score.txt");

    // 使用自定义模板容器保存文件所有行
    MyVector<QString> lines;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        in.setCodec("GBK");

        while (!in.atEnd())
        {
            lines.append(in.readLine());
        }

        file.close();
    }

    // 文件为空时补充表头和结束标识
    if (lines.isEmpty())
    {
        lines.append("#学号 姓名 课程名称 成绩");
        lines.append("#END");
    }

    bool inserted = false;

    // 将新成绩插入到 #END 前，保证文件结束标识始终在最后
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines.at(i).trimmed() == "#END")
        {
            lines.insert(i, information);
            inserted = true;
            break;
        }
    }

    if (!inserted)
    {
        lines.append(information);
        lines.append("#END");
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "文件打开失败，信息没有写入", "确认");
        return;
    }

    QTextStream out(&file);
    out.setCodec("GBK");

    for (int i = 0; i < lines.size(); i++)
    {
        out << lines.at(i) << "\n";
    }

    file.close();
}

void tea_addscore::on_btn_addscore_cancel_clicked()
{
    this->close();
}
