#include "technologyeditor.h"
#include "ui_technologyeditor.h"
#include <QMessageBox>
#include <QFileDialog>
extern QString TechFileFilter;
TechnologyEditor::TechnologyEditor(const QString& filename, QWidget* parent)
    : QDialog(parent), ui(new Ui::TechnologyEditor),
      m_bSaved(false),
      m_editedTech(nullptr)
{
  ui->setupUi(this);

  connect(ui->btnOk,      &QPushButton::clicked, this, &TechnologyEditor::ok);
  connect(ui->btnCancel,  &QPushButton::clicked, this, &TechnologyEditor::cancel);
  connect(ui->btnSave,    &QPushButton::clicked, this, &TechnologyEditor::save);
  connect(ui->btnSaveAs,  &QPushButton::clicked, this, &TechnologyEditor::saveas);
  connect(ui->btnLoad,    &QPushButton::clicked, this, &TechnologyEditor::load);

  if (filename.isEmpty())
  {
    m_editedTech = new tech();
    return;
  }

  // When saving, if we are saving to a previously loaded tech, we need to update that as well
  m_editedTech    = new tech(filename);

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

/**
 * @brief TechnologyEditor::ok
 */
void      TechnologyEditor::ok()
{
    // On ok, we need to save.

    QDialog::accept();
}


void      TechnologyEditor::cancel()
{

}

/**
 * @brief TechnologyEditor::save
 * @return
 */
void      TechnologyEditor::save()         // Save the technology into the selected files
{
  if (m_editedTech->getFilename()=="")
    { saveas(); return;}

  if (!copyDataToTech())
  {
    QMessageBox::critical(this, tr("Error"), tr("Errors in the form: ")+m_editedTech->getLastError());
    return;
  }
  // Check for any preexisting tech
  tech* prev = tech::getTechFromFilename(m_editedTech->getFilename());

  m_editedTech->save(prev==nullptr);

  // Update also the pre-loaded
  if (prev!=nullptr)
    prev->copyFrom(m_editedTech);
}

/**
 * @brief TechnologyEditor::saveas
 */

void      TechnologyEditor::saveas()
{
  QString newFile = QFileDialog::getSaveFileName(this, tr("Save tech"), "", TechFileFilter);
  if (newFile.isEmpty()) return;

  tech* prev = tech::getTechFromFilename(m_editedTech->getFilename());
  // If this is a new technology, make it immediately available
  m_editedTech->saveToFile(newFile,prev==nullptr);

  // Update also the pre-loaded
  if (prev!=nullptr)
    prev->copyFrom(m_editedTech);
}
/**
 * @brief TechnologyEditor::load
 */
void    TechnologyEditor::load()
{
  if (QMessageBox::question(this, tr("Confirm"), tr("This will override current data. Continue?"))==QMessageBox::No) return;

  QString filename = QFileDialog::getOpenFileName(this, "Select tech file", "", TechFileFilter);
  if (filename== nullptr) return;

  m_editedTech->load(filename);
}

/**
 * @brief TechnologyEditor::copyDataToTech
 * @return
 */
bool TechnologyEditor::copyDataToTech()
{
  return true;
}

