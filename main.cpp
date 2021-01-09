#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile fileCssExists(QString("://style/stylesheet.css"));
    if (fileCssExists.open(QFile::ReadOnly))
    {
        QString strStyleSheet = fileCssExists.readAll();

        w.setStyleSheet(strStyleSheet);
    }

    w.show();
    return a.exec();
}
