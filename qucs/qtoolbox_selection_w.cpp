#include "qtoolbox_selection_w.h"
#include "ui_qtoolbox_selection_w.h"

#include <QMessageBox>

QToolbox_Selection_w::QToolbox_Selection_w(QWidget* parent)
    : QToolbox_helper_w(parent), ui(new Ui::QToolbox_Selection_w) {
  ui->setupUi(this);

  connect(ui->pbZoom, &QPushButton::clicked, this, &QToolbox_Selection_w::zoom_pressed);
}

QToolbox_Selection_w::~QToolbox_Selection_w() {
  delete ui;
}


void QToolbox_Selection_w::new_mouse_position(double x, double y)
{
  ui->lePointX->setText(QString::number(x));
  ui->lePointY->setText(QString::number(y));

}

void QToolbox_Selection_w::zoom_pressed()
{
  bool bok;
  double x = ui->lePointX->text().toDouble(&bok);
  if (!bok)
  {
    QMessageBox::critical(this,"Error","Provide a valid number");
    ui->lePointX->setFocus();
    return;
  }
  double y = ui->lePointY->text().toDouble(&bok);
  if (!bok)
  {
    QMessageBox::critical(this,"Error","Provide a valid number");
    ui->lePointY->setFocus();
    return;
  }

  emit zoom_on_position(x,y);
}



