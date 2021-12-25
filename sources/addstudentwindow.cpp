#include "./headers/addstudentwindow.h"
#include "ui_addstudentwindow.h"
#include <QDebug>
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include <iostream>

AddStudentWindow::AddStudentWindow(QWidget *parent, QSqlQuery* StudQuer) :
    QMainWindow(parent),
    ui(new Ui::AddStudentWindow)
{
    ui->setupUi(this);

    ui->AddButton->setEnabled(false);
    QRegExpValidator* validator = new QRegExpValidator(QRegExp("[a-zA-Z]{1,20}"), this);
    ui->NameLineEdit->setValidator(validator);
    ui->SurnameLineEdit->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("[1-9]{1}[0-9]{0,3}"), this);
    ui->PriceLineEdit->setValidator(validator);

    Query = StudQuer;

    QueryModel = new QSqlQueryModel();
    Query->exec("select * from Students");
    QueryModel->setQuery(*Query);
    ui->tableView->setModel(QueryModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        FillGroupsComboBox();
        FillLangComboBox();
    });
    th.detach();
}

AddStudentWindow::~AddStudentWindow() {
    delete ui;
}

void AddStudentWindow::FillGroupsComboBox() {
    Query->exec("select * from Groups");
    while(Query->next()) {
        ui->GroupComboBox->addItem(Query->value(0).toString());
    }
}

void AddStudentWindow::FillLangComboBox() {
    Query->exec("select * from Languages");
    while(Query->next()) {
        ui->LangComboBox->addItem(Query->value(0).toString());
    }
}

void AddStudentWindow::CancelButtonClicked() {
    QueryModel->~QSqlQueryModel();
    this->close();
    emit WindowClosed();
}

void AddStudentWindow::AddButtonSetEnabled() {
    ui->AddButton->setEnabled(!ui->NameLineEdit->text().isEmpty() && !ui->SurnameLineEdit->text().isEmpty() && !ui->PriceLineEdit->text().isEmpty());
}

void AddStudentWindow::AddButtonClicked() {
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->prepare("select * from Students where Name=? and Surname=? and Groups=? and Language=? and Level=? and Teacher=? and Price=?");
        Query->bindValue(0, ui->NameLineEdit->text());
        Query->bindValue(1, ui->SurnameLineEdit->text());
        Query->bindValue(2, ui->GroupComboBox->currentText());
        Query->bindValue(3, ui->LangComboBox->currentText());
        Query->bindValue(4, ui->LevelComboBox->currentText());
        Query->bindValue(5, ui->TeacherComboBox->currentText());
        Query->bindValue(6, ui->PriceLineEdit->text());
        Query->exec();
        size_t counter = 0;
        while (Query->next()) {
            ++counter;
        }
        if (counter > 0) {
            QMessageBox::warning(this, "Not added", "There is already such a student");
        } else {
            Query->prepare("insert into Students (Name, Surname, Groups, Language, Level, Teacher, Price) values (?, ?, ?, ?, ?, ?, ?)");
            Query->bindValue(0, ui->NameLineEdit->text());
            Query->bindValue(1, ui->SurnameLineEdit->text());
            Query->bindValue(2, ui->GroupComboBox->currentText());
            Query->bindValue(3, ui->LangComboBox->currentText());
            Query->bindValue(4, ui->LevelComboBox->currentText());
            Query->bindValue(5, ui->TeacherComboBox->currentText());
            Query->bindValue(6, ui->PriceLineEdit->text());
            Query->exec();
            Query->exec("select * from Students");
            QueryModel->setQuery(*Query);
            ui->tableView->setModel(QueryModel);
            emit StudentAdded();

            ui->NameLineEdit->clear();
            ui->SurnameLineEdit->clear();
            ui->PriceLineEdit->clear();
        }});
    th.detach();
}

void AddStudentWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit from adding student",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        CancelButtonClicked();
    }
}
