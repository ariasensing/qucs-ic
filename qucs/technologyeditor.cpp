#include "technologyeditor.h"
#include "ui_technologyeditor.h"
#include <QMessageBox>
#include <QFileDialog>
extern QString TechFileFilter;
TechnologyEditor::TechnologyEditor(const QString& filename, QWidget* parent)
    : QDialog(parent),
      m_bSaved(false),
      m_editedTech(nullptr),
      ui(new Ui::TechnologyEditor)
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
  connect(ui->tblLayers,     &QTableWidget::doubleClicked, this, &TechnologyEditor::editLayer);

  // Create dummy layout view
  m_layoutView = new lay::LayoutView(nullptr, true, nullptr,
            lay::LayoutViewBase::LV_NoHierarchyPanel +
            lay::LayoutViewBase::LV_NoEditorOptionsPanel +
            lay::LayoutViewBase::LV_NoBookmarksView +
            lay::LayoutViewBase::LV_NoZoom +
            lay::LayoutViewBase::LV_NoGrid +
            lay::LayoutViewBase::LV_NoPropertiesPopup +
            lay::LayoutViewBase::LV_NoServices);

  assert(m_layoutView!=nullptr);

  m_editedTech = new tech(filename);
  // Create dummy layoutView and layout for the layer mgmt
  m_layoutView->create_layout(m_editedTech->getTechname().toStdString(),true,true);
  m_layout =  &(m_layoutView->cellview(0)->layout());

  assert(m_layout!=nullptr);
}

TechnologyEditor::~TechnologyEditor() {

  if (m_layoutView!=nullptr) delete m_layoutView;
  m_layoutView = nullptr;

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

  // NB When saving, copy all the files in the proper location (i.e. subfolder of tech folder)
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



