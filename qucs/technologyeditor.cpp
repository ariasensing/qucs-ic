#include "technologyeditor.h"
#include "ui_technologyeditor.h"

TechnologyEditor::TechnologyEditor(const QString& filename, QWidget* parent)
    : QDialog(parent), ui(new Ui::TechnologyEditor),
      m_bSaved(false)
{
  ui->setupUi(this);
}

TechnologyEditor::~TechnologyEditor() {
  delete ui;
}

/**
 * @brief TechnologyEditor::fileSaved
 * @return true if the technology file has been saved. this would require a refresh in the
 */
bool TechnologyEditor::fileSaved()
{
  return m_bSaved;
}