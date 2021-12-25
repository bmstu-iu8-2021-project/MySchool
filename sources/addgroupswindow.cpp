#include "./headers/addgroupswindow.h"
#include "ui_addgroupswindow.h"
#include <QDebug>
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>

AddGroupsWindow::AddGroupsWindow(QWidget *parent, QSqlQuery* query) :
    QMainWindow(parent),
    ui(new Ui::AddGroupsWindow)
{
    ui->setupUi(this);
    ui->AddButton->setEnabled(false);

    QRegExpValidator* validator = new QRegExpValidator(QRegExp("[A-Z]{1,3}[1-9]{0,1}[0-9]{0,1}[\-][1-9]{1}[0-9]{1,2}[A-Z]{0,1}"), this);
    ui->GroupsLineEdit->setValidator(validator);

    Query = query;
    QueryModel = new QSqlQueryModel();
    Query->exec("select * from Groups");
    QueryModel->setQuery(*Query);
    ui->tableView->setModel(QueryModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

AddGroupsWindow::~AddGroupsWindow() {
    delete ui;
}

void AddGroupsWindow::on_AddButton_clicked() {
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->exec("select * from Groups where Groups='"+ui->GroupsLineEdit->text()+"'");
        size_t counter = 0;
        while (Query->next()) {
            ++counter;
        }
        if (counter > 0) {
            QMessageBox::warning(this, "Not added", "There is already such a group");
        } else {
            Query->exec("insert into Groups (Groups) values ('"+ui->GroupsLineEdit->text()+"')");
            Query->exec("select * from Groups");
            QueryModel->setQuery(*Query);
            ui->tableView->setModel(QueryModel);
            ui->GroupsLineEdit->clear();
        }
    });
    th.detach();
}





void AddGroupsWindow::on_GroupsLineEdit_textChanged(const QString &arg1) {
    ui->AddButton->setEnabled(!arg1.isEmpty());
}


void AddGroupsWindow::on_DeleteButton_clicked() {
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->exec("select * from Students where Groups='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
        size_t counter = 0;
        while (Query->next()) {
            ++counter;
        }
        if (counter > 0) {
            QMessageBox MSG;
            MSG.setText("There are students in this group");
            MSG.setInformativeText("Their group will be set to 'deleted'");
            MSG.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            int ret = MSG.exec();
            if (ret == 1024) {
                Query->exec("update Students set Groups='deleted' where Groups='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
                Query->exec("delete from Groups where Groups='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");

                Query->exec("select * from Groups");

                QueryModel->setQuery(*Query);

                ui->tableView->setModel(QueryModel);
                emit GroupsDeleted();

            }
        } else {
            Query->exec("delete from Groups where Groups='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
            Query->exec("select * from Groups");
            QueryModel->setQuery(*Query);
            ui->tableView->setModel(QueryModel);
        }
        ui->GroupsLineEdit->clear();
    });
    th.detach();
}

void AddGroupsWindow::closeEvent(QCloseEvent *event) {
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

void AddGroupsWindow::CloseButtonClicked() {
    QueryModel->~QSqlQueryModel();
    this->close();
    emit WindowClosed();
}
