#include "ExcelManger.h"
#include <QDebug>
#if defined(Q_OS_WIN)
#include <QAxObject>
#include <windows.h>
#include <QVariant>
#endif // Q_OS_WIN


ExcelManger::ExcelManger(QObject *parent) : QObject(parent)
{
    // 在后台线程中使用QAxObject必须先初始化
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

}

bool ExcelManger::Test(QString &path)
{
    QAxObject *excel = NULL;    //本例中，excel设定为Excel文件的操作对象
    QAxObject *workbooks = NULL;
    QAxObject *workbook = NULL;  //Excel操作对象
    excel = new QAxObject("Excel.Application");
    excel->dynamicCall("SetVisible(bool)", true); //true 表示操作文件时可见，false表示为不可见
    workbooks = excel->querySubObject("WorkBooks");
    //按文件路径打开文件
    workbook = workbooks->querySubObject("Open(QString&)", path);
    QAxObject * worksheets = workbook->querySubObject("WorkSheets"); // 获取打开的excel文件中所有的工作sheet

    //Excel文件中表的个数:
    qDebug() << QString("Excel文件中表的个数: %1").arg(QString::number(worksheets->property("Count").toInt()));

    // 获取第n个工作表 querySubObject("Item(int)", n);
    QAxObject * worksheet = worksheets->querySubObject("Item(int)", 1);

    //获取该sheet的使用到的范围（可以理解为有数据的矩形区域）
    QAxObject * usedrange = worksheet->querySubObject("UsedRange");

    //获取行数
    QAxObject * rows = usedrange->querySubObject("Rows");
    qDebug() << QString("行数为: %1").arg(QString::number(rows->property("Count").toInt()));

    //获取列数
    QAxObject * columns = usedrange->querySubObject("Columns");
    qDebug() << QString("列数为: %1").arg(QString::number(columns->property("Count").toInt()));

    //起始行
    qDebug() << QString("起始行为: %1").arg(QString::number(rows->property("Row").toInt()));
    //起始列
    qDebug() << QString("起始列为: %1").arg(QString::number(columns->property("Column").toInt()));


    //——————————————————————————————读数据——————————————————————————————————————————
    //获取第i行第j列的数据
    //假如是第6行，第6列 对应表中F列6行，即F6
    QAxObject *range1 = worksheet->querySubObject("Range(QString)", "F6");

    QString strRow6Col6 = "";
    strRow6Col6 = range1->property("Value").toString();//QVariant(worksheet->querySubObject("Range(QString)", "F6")->property("Value")).toString();
    qDebug() << "第6行，第6列的数据为：" + strRow6Col6;

    //待添加一个转换函数，即第6行，第6列，66转为F6


    //——————————————————————————————写数据——————————————————————————————————————————
    //获取F6的位置
    QAxObject *range2 = worksheet->querySubObject("Range(QString)", "F6");
    //写入数据, 第6行，第6列
    range2->setProperty("Value", "中共十九大");
    QString newStr = "";
    newStr = range2->property("Value").toString();
    qDebug() << "写入数据后，第6行，第6列的数据为：" + newStr;

    //!!!!!!!一定要记得close，不然系统进程里会出现n个EXCEL.EXE
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    if (excel)
    {
        delete excel;
        excel = NULL;
    }

    return true;
}