#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainReader = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_readyButton_clicked()
{
    if(mainReader != NULL)
    {
        disconnect(mainReader, SIGNAL(sendMessage(QString)), this, SLOT(recieveMessage(QString)));
        delete mainReader;
        mainReader = NULL;
    }
    QString text = ui->deviceLine->text();
    mainReader = new Reader(text, this);
    connect(mainReader, SIGNAL(sendMessage(QString)), this, SLOT(recieveMessage(QString)));
}


void MainWindow::recieveMessage(QString mess)
{
    ui->idBrowser->append(mess);
}
