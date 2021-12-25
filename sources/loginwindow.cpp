#include "./headers/loginwindow.h"
#include "ui_loginwindow.h"
#include "./headers/mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QRegExp>
#include <QRegExpValidator>
#include <QPixmap>
#include <QByteArray>
#include <QCryptographicHash>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    ui->pic_lab->setPixmap(ui->pic_lab->pixmap()->scaled(250, 250, Qt::KeepAspectRatio));

    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());
    QPoint center = rect.center();
    center.setX(center.x() - (window()->width()/2));
    center.setY(center.y() - (window()->height()/2));
    this->move(center);

    ui->OKButton->setEnabled(false);
    ui->OKButton->setDefault(true);
    ui->LoginLineEdit->setFocus();

    QRegExp RegExp("[0-9a-zA-Z]{1,20}");
    QRegExpValidator* validator = new QRegExpValidator(RegExp, this);
    ui->LoginLineEdit->setValidator(validator);
    ui->PasswordLineEdit->setValidator(validator);
    ui->PasswordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

    DB = QSqlDatabase::addDatabase("QSQLITE", "connection to logins and passwords");
    DB.setDatabaseName("./LoginsAndPasswords.db");
    DB.open();
    Query = new QSqlQuery(DB);
}

LoginWindow::~LoginWindow() {
    DB.close();
    Query->~QSqlQuery();
    delete ui;
}


void LoginWindow::on_OKButton_clicked() {
    std::thread th([&](){
        QString str = ui->PasswordLineEdit->text() + ui->LoginLineEdit->text() + "salt";
        QByteArray qbar = str.toUtf8();
        QString hash = QCryptographicHash::hash(qbar, QCryptographicHash::Sha256).toHex();

        Query->prepare("select * from LoginsAndPasswords where Login=? and Hash=?");
        Query->bindValue(0, ui->LoginLineEdit->text());
        Query->bindValue(1, hash);
        Query->exec();});

    th.join();
    size_t counter = 0;
    while(Query->next()) {
        ++counter;
    }
    if (counter == 1) {
        MainWindow* mw = new MainWindow();
        mw->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Warning", "Wrong login or password");

    }
}

void LoginWindow::OKButtonSetEnabled() {
    ui->OKButton->setEnabled(!ui->LoginLineEdit->text().isEmpty() && !ui->PasswordLineEdit->text().isEmpty());
}

void LoginWindow::on_ShowPasswordCheckBox_stateChanged(int arg1) {
    if (arg1) {
        ui->PasswordLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
    } else {
        ui->PasswordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
    }
}
