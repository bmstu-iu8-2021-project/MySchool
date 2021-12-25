#include "./headers/editwindow.h"
#include "ui_editwindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

EditWindow::EditWindow(QWidget *parent, QSqlQuery* StudQuery) :
    QMainWindow(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);

    ui->SaveButton->setEnabled(false);
    ui->DeleteButton->setEnabled(false);
    QRegExpValidator* validator = new QRegExpValidator(QRegExp("[a-zA-Z]{1,20}"), this);
    ui->NameLineEdit->setValidator(validator);
    ui->SurnameLineEdit->setValidator(validator);
    validator = new QRegExpValidator(QRegExp("[1-9]{1}[0-9]{0,3}"), this);
    ui->PriceLineEdit->setValidator(validator);


    Query = StudQuery;
    QueryModel = new QSqlQueryModel();
    Query->exec("select * from Students");
    QueryModel->setQuery(*Query);
    ui->tableView->setModel(QueryModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

EditWindow::~EditWindow() {
    delete ui;
}

void EditWindow::CloseButtonClicked() {
    QueryModel->~QSqlQueryModel();
    this->close();
    emit WindowClosed();
}

void EditWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit from adding groups",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        CloseButtonClicked();
    }
}

void EditWindow::SaveButtonSetEnabled() {
    ui->SaveButton->setEnabled(!ui->NameLineEdit->text().isEmpty() && !ui->SurnameLineEdit->text().isEmpty() && !ui->PriceLineEdit->text().isEmpty());
    ui->DeleteButton->setEnabled(!ui->NameLineEdit->text().isEmpty() && !ui->SurnameLineEdit->text().isEmpty() && !ui->PriceLineEdit->text().isEmpty());
}

void EditWindow::on_tableView_clicked(const QModelIndex &index) {

    OldValues.clear();
    size_t row = index.row();
    ui->NameLineEdit->setText(QueryModel->index(row, 0).data().toString());
    OldValues.push_back(QueryModel->index(row, 0).data().toString());
    ui->SurnameLineEdit->setText(QueryModel->index(row, 1).data().toString());
    OldValues.push_back(QueryModel->index(row, 1).data().toString());

    ui->GroupsComboBox->clear();
    ui->GroupsComboBox->addItem(QueryModel->index(row, 2).data().toString());
    OldValues.push_back(QueryModel->index(row, 2).data().toString());

    ui->LangComboBox->clear();
    ui->LangComboBox->addItem(QueryModel->index(row, 3).data().toString());
    OldValues.push_back(QueryModel->index(row, 3).data().toString());

    ui->LevelComboBox->setCurrentText(QueryModel->index(row, 4).data().toString());
    OldValues.push_back(QueryModel->index(row, 4).data().toString());
    ui->TeacherComboBox->setCurrentText(QueryModel->index(row, 5).data().toString());
    OldValues.push_back(QueryModel->index(row, 5).data().toString());
    ui->PriceLineEdit->setText(QueryModel->index(row, 6).data().toString());
    OldValues.push_back(QueryModel->index(row, 6).data().toString());

    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        FillGroupsComboBox(row);
        FillLangComboBox(row);
    });
    th.detach();
}

void EditWindow::FillGroupsComboBox(const size_t row) {
    Query->exec("select * from Groups");
    while (Query->next()) {
        if (Query->value(0).toString() != QueryModel->index(row, 2).data().toString()) {
            ui->GroupsComboBox->addItem(Query->value(0).toString());
        }
    }
    Query->clear();
}

void EditWindow::FillLangComboBox(const size_t row) {
    Query->exec("select * from Languages");
    while (Query->next()) {
        if (Query->value(0).toString() != QueryModel->index(row, 3).data().toString()) {
            ui->LangComboBox->addItem(Query->value(0).toString());
        }
    }
    Query->clear();
}


void EditWindow::on_SaveButton_clicked()
{
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->prepare("select * from Students where Name=? and Surname=? and Groups=? and Language=? and Level=? and Teacher=? and Price=?");
        Query->bindValue(0, ui->NameLineEdit->text());
        Query->bindValue(1, ui->SurnameLineEdit->text());
        Query->bindValue(2, ui->GroupsComboBox->currentText());
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
            QMessageBox::warning(this, "Not saved", "There is already such a student");
        } else {
            Query->prepare("update Students set Name=?, Surname=?, Groups=?, Language=?, Level=?, Teacher=?, Price=? where Name=? and Surname=? and Groups=? and Language=? and Level=? and Teacher=? and Price=?");
            Query->bindValue(0, ui->NameLineEdit->text());
            Query->bindValue(1, ui->SurnameLineEdit->text());
            Query->bindValue(2, ui->GroupsComboBox->currentText());
            Query->bindValue(3, ui->LangComboBox->currentText());
            Query->bindValue(4, ui->LevelComboBox->currentText());
            Query->bindValue(5, ui->TeacherComboBox->currentText());
            Query->bindValue(6, ui->PriceLineEdit->text());
            for (int i = 7; i < OldValues.size() + 7; ++i) {
                Query->bindValue(i, OldValues[i-7]);
            }
            Query->exec();

            Query->exec("select * from Students");
            QueryModel->setQuery(*Query);
            ui->tableView->setModel(QueryModel);
            emit StudentEdited();

            ui->NameLineEdit->clear();
            ui->SurnameLineEdit->clear();
            ui->PriceLineEdit->clear();
        }});
    th.join();
}


void EditWindow::on_DeleteButton_clicked() {
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->prepare("delete from Students where Name=? and Surname=? and Groups=? and Language=? and Level=? and Teacher=? and Price=?");
        for (int i = 0; i < OldValues.size(); ++i) {
            Query->bindValue(i, OldValues[i]);
        }
        Query->exec();
        Query->exec("select * from Students");
        QueryModel->setQuery(*Query);
        ui->tableView->setModel(QueryModel);
        emit StudentDeleted();

        ui->NameLineEdit->clear();
        ui->SurnameLineEdit->clear();
        ui->PriceLineEdit->clear();
    });
    th.detach();
}

