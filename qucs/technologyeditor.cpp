#include "technologyeditor.h"
#include "ui_technologyeditor.h"
#include <QMessageBox>
#include <QFileDialog>

TechnologyEditor::TechnologyEditor(const QString& filename, QWidget* parent)
    : QDialog(parent), ui(new Ui::TechnologyEditor),
      m_bSaved(false),
      m_existingTech(nullptr),
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

  m_existingTech  = tech::getTechFromFilename(filename);
  // Possibile cases:
  // 1. We are opening a file and no tech is attached (m_existingTech  = null)
  // 2. We are opening a file and tech is attached    (m_existingTech != null)
  // 3. Filename is empty -> new tech
  //--------------------------------------------------------
  // CASE 1
  // 1. This is simple: create a temporary tech object, load the file, and edit the object
  //    When saving, update the db (the technology become available)
  //    When confirming, update the db (check if save is needed, if file name is empty, it will be lost)


  //---------------------------------------------------------
  // CASE 2
  // 2. We must create a temporary tech object, copy all data from the existing one (with no db override)
  //    If we accept the changes, the existing one should be copied from this
  //    If we save into a new file which is different from previous one, we should modify the filename


  m_editedTech    = new tech(filename); // <- During tech creation, if an old one is existing, it is simply copied
                                        // and no db modification is attempted. So this procedure is ok for both 1. and 2

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
  // m_editedData contains most updated data
  if (!copyDataToTech())
  {
    QMessageBox::critical(this, tr("Error"), tr("Errors in the form: ")+m_editedTech->getLastError());
    return;
  }

  if (m_editedTech->getFilename()="")
    { saveas(); return;}

  // In this case, the technology is automatically added to the available ones
  m_editedTech->save();
  // If a previous one was missing, let's create
  if (m_existingTech==nullptr) m_existingTech = new tech(m_editedTech->getFilename());
  m_existingTech->copyFrom(m_editedTech);
}

/**
 * @brief TechnologyEditor::saveas
 */

void      TechnologyEditor::saveas()
{
  QString newFile = QFileDialog::getSaveFileName(this, tr("Save tech"), "", )
  // We are saving the technology into a new filename
  // If a previous one was missing, we are creating a new
  if (m_existingTech==nullptr)
  {

  }


}
/**
 * @brief TechnologyEditor::load
 */
void    TechnologyEditor::load()
{

}


bool TechnologyEditor::copyDataToTech()
{
  return true;
}