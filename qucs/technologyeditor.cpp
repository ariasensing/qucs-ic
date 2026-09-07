#include "technologyeditor.h"
#include "ui_technologyeditor.h"
#include <QMessageBox>
#include <QFileDialog>
extern QString TechFileFilter;
TechnologyEditor::TechnologyEditor(const QString& filename, QWidget* parent)
    : QDialog(parent),
      m_bSaved(false),
      m_editedTech(nullptr),
      ui(new Ui::TechnologyEditor),
      m_lastError(""),
      m_WidgetWithError(nullptr)
{
  ui->setupUi(this);

  connect(ui->btnOk,         &QPushButton::clicked, this, &TechnologyEditor::ok);
  connect(ui->btnCancel,     &QPushButton::clicked, this, &TechnologyEditor::cancel);
  connect(ui->btnSave,       &QPushButton::clicked, this, &TechnologyEditor::save);
  connect(ui->btnSaveAs,     &QPushButton::clicked, this, &TechnologyEditor::saveas);
  connect(ui->btnLoad,       &QPushButton::clicked, this, &TechnologyEditor::load);
  connect(ui->btnLoadLayers, &QPushButton::clicked, this, &TechnologyEditor::loadLayers);
  connect(ui->btnSaveLayers, &QPushButton::clicked, this, &TechnologyEditor::saveLayers);
  connect(ui->btnNewLayer,   &QPushButton::clicked, this, &TechnologyEditor::createLayer);
  connect(ui->btnRemoveLayer,&QPushButton::clicked, this, &TechnologyEditor::removeLayer);
  connect(ui->btnImportLayerMap,
                             &QPushButton::clicked, this, &TechnologyEditor::importLayerMap);
  connect(ui->tblLayers,     &QTableWidget::doubleClicked, this, &TechnologyEditor::editLayer);
  connect(ui->btnMakeAvailable,
                            &QPushButton::clicked, this, &TechnologyEditor::makeTechAvailable);


  m_editedTech = new tech(filename);

  if (!filename.isEmpty())
    copyDataFromTech();

}

TechnologyEditor::~TechnologyEditor() {

  if (m_editedTech!=nullptr) delete m_editedTech;

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
  if (!copyDataToTech())
  {
    QMessageBox::critical(this, tr("Error"), tr("Errors in the form: ")+m_lastError);
    if (m_WidgetWithError!=nullptr) m_WidgetWithError->setFocus();
    return;
  }

  if (m_editedTech->getFilename()=="")
  { saveas(); return;}

  // Check for any preexisting tech
  tech* prev = tech::getTechFromFilename(m_editedTech->getFilename());

  // NB When saving, copy all the files in the proper location (i.e. subfolder of tech folder)
  m_editedTech->save(prev==nullptr);

  // Update also the pre-loaded
  if (prev!=nullptr)
    prev->copyFrom(m_editedTech);

  updateGUIAfterSave();
}

/**
 * @brief TechnologyEditor::saveas
 */

void      TechnologyEditor::  saveas()
{
  QString selectedFilter;
  QString newFile = QFileDialog::getSaveFileName(this, tr("Save tech"), "", TechFileFilter, &selectedFilter);
  if (newFile.isEmpty()) return;
  QFileInfo fi(newFile);
  if (fi.completeSuffix()!="tech")
    newFile=QFileInfo(fi.absolutePath(),fi.baseName()+".tech").absoluteFilePath();

  tech* prev = tech::getTechFromFilename(m_editedTech->getFilename());
  // If this is a new technology, make it immediately available
  m_editedTech->saveToFile(newFile,prev==nullptr);

  // Update also the pre-loaded
  if (prev!=nullptr)
    prev->copyFrom(m_editedTech);

  updateGUIAfterSave();
}
/**
 * @brief TechnologyEditor::load
 */
void    TechnologyEditor::load()
{
  if (QMessageBox::question(this, tr("Confirm"), tr("This will override current data. Continue?"))==QMessageBox::No) return;

  QString filename = QFileDialog::getOpenFileName(this, "Select tech file", "", TechFileFilter);
  if (filename== nullptr) return;

  if (!m_editedTech->load(filename))
  {
    QMessageBox::critical(this,tr("Error"), m_editedTech->getLastError());
    return;
  }

  copyDataFromTech();
}

/**
 * @brief TechnologyEditor::copyDataToTech
 * Copy the content of the dialog into the tech
 * @return
 */
bool TechnologyEditor::copyDataToTech()
{
  m_lastError.clear();
  m_WidgetWithError = nullptr;
  // Tech name
  QString newName = ui->leTechName->text();
  if (newName.isEmpty())
  { m_lastError = tr("Error: empty tech name"); m_WidgetWithError = ui->leTechFile; return false;}

  m_editedTech->rename(newName);

  // Description
  m_editedTech->setDescription(ui->teTechDescription->toPlainText());

  // LYP/LYT files are handled internally. No need to update
  // DBU, Grid

  bool bok;
  double dbu = ui->leDBU->text().toDouble(&bok);
  if ((!bok)||(dbu<0))
  {
    if (!bok)
      m_lastError = tr("DBU must be a valid number");
    else
      m_lastError = tr("DBU must be positive");
    m_WidgetWithError = ui->leDBU;
    return false;
  }

  m_editedTech->dbu(dbu);

  // grid
  QString grid = ui->leGrid->text();

  if (grid.isEmpty())
    grid.number(5 * dbu);

  m_editedTech->grid(grid);

  return true;
}


/**
 * @brief TechnologyEditor::copyDataFromTech. This is an update of the GUI
 * @return
 */
bool TechnologyEditor::copyDataFromTech()
{
  // Tech name
  ui->leTechName->setText( m_editedTech->getTechname());
  // Description
  ui->teTechDescription->setPlainText(m_editedTech->getDescription());
  // DBU
  ui->leDBU->setText(QString::number(m_editedTech->dbu()));
  // Grid
  ui->leGrid->setText(m_editedTech->grid());
  // Lyp/Lyt (clear them)
  ui->leLypFile->clear();
  ui->leTechFile->clear();
  // Last saved
  ui->lblLastModified->setText(m_editedTech->getLastModified());

  // Layers
  listLayers();

  return true;
}
/**
 * @brief TechnologyEditor::updateGUIAfterSave
 */
void TechnologyEditor::updateGUIAfterSave()
{
  ui->lblLastModified->setText(m_editedTech->getLastModified());
  ui->leLypFile->clear();
  ui->leTechFile->clear();
}
/**
 * @brief TechnologyEditor::makeTechAvailable
 */
void TechnologyEditor::makeTechAvailable()
{
  // Check if we have a previous tech with the same name
  // Check for any preexisting tech
  tech* prev = tech::getTechFromFilename(m_editedTech->getFilename());

  // Update also the pre-loaded
  if (prev!=nullptr)
  {
    if (QMessageBox::question(this, tr("Confirm"), tr("The current technology will be overwritten by the current one\n Continue?"))
        ==QMessageBox::No) return;

    prev->copyFrom(m_editedTech);
  }
  else
  {
    tech* new_tech = new tech();
    new_tech->copyFrom(m_editedTech);
    new_tech->makeAvailableForTheProject();
  }
  return;
}


