#include "config.h"
#include "ui_config.h"

config::config(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::config)
{
    ui->setupUi(this);
}

config::~config()
{
    delete ui;
}
