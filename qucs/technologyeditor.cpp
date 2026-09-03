#include "technologyeditor.h"
#include "ui_technologyeditor.h"
#include <QMessageBox>
#include <QFileDialog>
extern QString TechFileFilter;
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
  if (m_editedTech->getFilename()=="")
    { saveas(); return;}

         // m_editedData contains most updated data
  if (!copyDataToTech())
  {
    QMessageBox::critical(this, tr("Error"), tr("Errors in the form: ")+m_editedTech->getLastError());
    return;
  }

  // In this case, exisitingTech must be created (this is the one staying in the database)
  if (m_existingTech==nullptr) m_existingTech = new tech();
  m_existingTech->copyFrom(m_editedTech);
  m_existingTech->save();
}

/**
 * @brief TechnologyEditor::saveas
 */

void      TechnologyEditor::saveas()
{
  QString newFile = QFileDialog::getSaveFileName(this, tr("Save tech"), "", TechFileFilter);
  if (newFile.isEmpty()) return;

  if (!copyDataToTech())
  {
    QMessageBox::critical(this, tr("Error"), tr("Errors in the form: ")+m_editedTech->getLastError());
    return;
  }
  // Here editedTech contains the most updated data

  // If a previous one was missing, we are creating a new technology
  if (m_existingTech==nullptr)
  {
    m_existingTech = new tech();
    m_existingTech->copyFrom(m_editedTech);
    m_existingTech->save();
    return;
  }

  // We are modifying an existing tech which is required to move into a new filename
  if (m_existingTech->getFilename()!=newFile)
  {
    // We want to save the same technology into a new filename. This will lead to a conflict when loading
    // a project, but we'll take care of it during project loading. For the moment, just assign the new file

    if (m_editedTech->getTechname()==m_existingTech->getTechname())
    {
      if (QMessageBox::question(this, tr("Warning"),tr("Saving the same technology into a different file could lead to errors\n "
                                                        "when loading a project.\n Do you want to continue"))
          ==QMessageBox::No) return;

      m_existingTech->copyFrom(m_editedTech);
      m_existingTech->saveToFile(newFile);
      return;
    }
    else
    {
      // Here we have a different file and a different techname. Create

      // Create a new backup. Previous is untouched
      m_existingTech = new tech();
      m_existingTech->copyFrom(m_editedTech);
      m_existingTech->saveToFile(newFile);
      return;
    }
  }
  else
  {
    // Same file but new tech name. We should remove previous one
    if (m_editedTech->getTechname()!=m_existingTech->getTechname())
    {
      // Rename the previous one so that previous entry are discarded
      m_existingTech->rename(m_editedTech->getTechname());
      m_existingTech->copyFrom(m_editedTech);
      // Create a new tech and
      m_existingTech->saveToFile(newFile);
    }
    else
    {
      // Same file, same tech name. It is an update
      m_existingTech->copyFrom(m_editedTech);
      m_existingTech->save();
    }
  }
}
/**
 * @brief TechnologyEditor::load
 */
void    TechnologyEditor::load()
{
  // load a file.
  if (QMessageBox::question(this, tr("Confirm"), tr("This will override current data. Continue?"))==QMessageBox::No) return;

  QString filename = QFileDialog::getOpenFileName(this, "Select tech file", "", TechFileFilter);
  if (filename== nullptr) return;
  // If we have a previous file and associated tech, update previous
  tech* temp = tech::getTechFromFilename(filename);

  m_existingTech = temp;

  m_existingTech->load();
}


bool TechnologyEditor::copyDataToTech()
{
  return true;
}