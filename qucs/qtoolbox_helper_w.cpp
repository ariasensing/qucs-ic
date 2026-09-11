#include "qtoolbox_helper_w.h"
#include "ui_qtoolbox_helper_w.h"

QToolbox_helper_w::QToolbox_helper_w(QWidget* parent)
    : QFrame(parent), ui(new Ui::QToolbox_helper_w) {
  ui->setupUi(this);
}

QToolbox_helper_w::~QToolbox_helper_w() {
  delete ui;
}
