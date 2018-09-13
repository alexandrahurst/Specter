#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settingsDialog = new SettingsDialog(ui->spectrogramDisplay);

    connect(ui->settingsButton, SIGNAL(clicked()), settingsDialog, SLOT(toggleVisible()));
    connect(ui->recordButton, SIGNAL(clicked(bool)), ui->spectrogramDisplay, SLOT(setRecording(bool)));
}

MainWindow::~MainWindow() {
    delete settingsDialog;
    delete ui;
}
