#include "qtoolbox_selection_w.h"
#include "ui_qtoolbox_selection_w.h"

QToolbox_Selection_w::QToolbox_Selection_w(QWidget* parent)
    : QToolbox_helper_w(parent), ui(new Ui::QToolbox_Selection_w) {
  ui->setupUi(this);
}

QToolbox_Selection_w::~QToolbox_Selection_w() {
  delete ui;
}


void QToolbox_Selection_w::new_mouse_position(double x, double y)
{
  ui->lePointX->setText(QString::number(x));
  ui->lePointY->setText(QString::number(y));

}



