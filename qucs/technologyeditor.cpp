#include "technologyeditor.h"
#include "ui_technologyeditor.h"

TechnologyEditor::TechnologyEditor(QWidget* parent)
    : QDialog(parent), ui(new Ui::TechnologyEditor) {
  ui->setupUi(this);
}

TechnologyEditor::~TechnologyEditor() {
  delete ui;
}
