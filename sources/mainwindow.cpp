#include "./headers/mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());
    QPoint center = rect.center();
    center.setX(center.x() - (window()->width()/2));
    center.setY(center.y() - (window()->height()/2));
    this->move(center);

    StudentsDB = QSqlDatabase::addDatabase("QSQLITE", "connection to Students");
    StudentsDB.setDatabaseName("./Students.db");
    StudentsDB.open();
    StudentsQuery = new QSqlQuery(StudentsDB);

    StudentsQM = new QSqlQueryModel();
    StudentsQuery->exec("select * from Students");
    StudentsQM->setQuery(*StudentsQuery);
    ui->tableView->setModel(StudentsQM);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReloadTable() {
    StudentsQuery->exec("select * from Students");
    StudentsQM->setQuery(*StudentsQuery);
    ui->tableView->setModel(StudentsQM);
}

void MainWindow::NewWindowOpenedSlot() {
    ui->AddGroupsButton->setEnabled(false);
    ui->AddStudentButton->setEnabled(false);
    ui->EditButton->setEnabled(false);
    ui->AddLangButton->setEnabled(false);
}

void MainWindow::NewWindowClosed() {
    ui->AddGroupsButton->setEnabled(true);
    ui->AddStudentButton->setEnabled(true);
    ui->EditButton->setEnabled(true);
    ui->AddLangButton->setEnabled(true);
}

void MainWindow::on_AddStudentButton_clicked() {
    AddStudentWindow* StudWind = new AddStudentWindow(this, StudentsQuery);
    connect(StudWind, &AddStudentWindow::StudentAdded, this, &MainWindow::ReloadTable);
    connect(this, &MainWindow::NewWindowOpenedSignal, this, &MainWindow::NewWindowOpenedSlot);
    connect(StudWind, &AddStudentWindow::WindowClosed, this, &MainWindow::NewWindowClosed);
    emit NewWindowOpenedSignal();
    StudWind->show();
}

void MainWindow::on_EditButton_clicked() {
    EditWindow* EditWind = new EditWindow(this, StudentsQuery);
    connect(this, &MainWindow::NewWindowOpenedSignal, this, &MainWindow::NewWindowOpenedSlot);
    connect(EditWind, &EditWindow::WindowClosed, this, &MainWindow::NewWindowClosed);
    connect(EditWind, &EditWindow::StudentEdited, this, &MainWindow::ReloadTable);
    connect(EditWind, &EditWindow::StudentDeleted, this, &MainWindow::ReloadTable);
    emit NewWindowOpenedSignal();
    EditWind->show();
}

void MainWindow::on_AddGroupsButton_clicked() {
    AddGroupsWindow* AddGroups = new AddGroupsWindow(this, StudentsQuery);
    connect(AddGroups, &AddGroupsWindow::GroupsDeleted, this, &MainWindow::ReloadTable);
    connect(this, &MainWindow::NewWindowOpenedSignal, this, &MainWindow::NewWindowOpenedSlot);
    emit NewWindowOpenedSignal();
    connect(AddGroups, &AddGroupsWindow::WindowClosed, this, &MainWindow::NewWindowClosed);
    AddGroups->show();
}

void MainWindow::on_AddLangButton_clicked() {
    AddLangWindow* AddLangW = new AddLangWindow(this, StudentsQuery);
    connect(this, &MainWindow::NewWindowOpenedSignal, this, &MainWindow::NewWindowOpenedSlot);
    connect(AddLangW, &AddLangWindow::LangDeleted, this, &MainWindow::ReloadTable);
    emit NewWindowOpenedSignal();
    connect(AddLangW, &AddLangWindow::WindowClosed, this, &MainWindow::NewWindowClosed);
    AddLangW->show();
}
