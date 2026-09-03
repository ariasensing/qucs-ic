#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo
#include "main.h"

extern QString TechFileFilter;
extern QString LayoutImportFilter;

/**
 * @brief icLayout::icLayout
 * @param app
 * @param owner
 * @param fname
 */
icLayout::icLayout(QucsApp* app, Schematic* owner, const QString& fname,const QString& techfile) : QDialog(nullptr), QucsDoc(app, fname, LAYOUT),
  ui(new Ui::icLayout),
  a_Schematic(nullptr),
  m_technology(nullptr)
{

  QString filename = fname;

  ui->setupUi(this);
  initKlayoutWidget();
  // Documents
  this->setProperty("DOC_TYPE",(uint16_t)(doc_type));

  attachToSchematic(owner);

  if (!filename.isEmpty())
  {
    icLayout::load();
  }
}

bool  icLayout::initKlayoutWidget()
{
  m_dbManager    = new db::Manager(true);
  m_layoutWidget = new lay::LayoutViewWidget(m_dbManager, false, nullptr);

  if (m_layoutWidget==nullptr) return false;
  // Add hierarchy
  QHBoxLayout* layout = new QHBoxLayout(ui->tabCellTree);
  layout->addWidget(m_layoutWidget->hierarchy_control_frame());
  ui->tabNets->setLayout(layout);
  // Layers
  QVBoxLayout *layersLayout = new QVBoxLayout(ui->tabLayers);
  layersLayout->addWidget(m_layoutWidget->layer_control_frame());
  ui->tabLayers->setLayout(layersLayout);

  // Toolbox


  // Layout widget
  QHBoxLayout *mainLayout = new QHBoxLayout(ui->centerFrame);
  mainLayout->addWidget(m_layoutWidget,1);

  m_layoutView = m_layoutWidget->view();
  if (m_layoutView == nullptr) return false;
  m_canvas_id = m_layoutView->create_layout(false);

  // Connections
  connect(ui->btnLoad, &QPushButton::clicked, this, &icLayout::loadLayoutClicked);
  connect(ui->btnSave, &QPushButton::clicked, this, &icLayout::saveLayoutClicked);

  return true;
}
/**
 * @brief icLayout::~icLayout
 */
icLayout::~icLayout() {
  delete ui;
  if (a_Schematic!=nullptr)
    a_Schematic->attachLayoutView();
}
/**
 * @brief icLayout::setName
 * @param Name_
 */
void  icLayout::setName(const QString& Name_)
{
  a_DocName = Name_;
  if (a_Schematic!=nullptr)
    a_Schematic->setLayoutFilename(a_DocName);
}

/**
 * @brief icLayout::attachToSchematic
 * @param schematic
 */
void icLayout::attachToSchematic(Schematic *schematic)
{
  if ((a_Schematic!=nullptr)&&(schematic==nullptr))
  {
    Schematic* oldSchem = a_Schematic;
    if (oldSchem == nullptr) return;
    a_Schematic = nullptr;
    oldSchem->attachLayoutView();
    return;
  }

  if ((a_Schematic!=nullptr)&&(schematic!=nullptr))
    return;

  if ((a_Schematic==nullptr)&&(schematic==nullptr))
    return;

  if ((a_Schematic==nullptr)&&(schematic!=nullptr))
  {
    a_Schematic = schematic;
    a_Schematic->attachLayoutView(this);
  }
}

/**
 * @brief icLayout::load
 * @return
 */
bool  icLayout::load()
{

  // If everything was ok, we may signal the owner that we have a valid filename
  if (a_Schematic!=nullptr)
    a_Schematic->setLayoutFilename(a_DocName);

  return true;
}

/**
 * @brief icLayout::save
 * @return
 */
int   icLayout::save()
{

  // update name according to saved file
  if (a_Schematic!=nullptr) a_Schematic->setLayoutFilename(a_DocName);
  return 0;
}
/**
 * @brief icLayout::loadLayoutClicked Load a layout file to be attached to this QucsDoc
 */
void icLayout::loadLayoutClicked()
{
  QString layoutFile = QFileDialog::getOpenFileName(this,"Load layout file",lastDir, LayoutImportFilter);
  if (layoutFile.isEmpty()) return;

  if ((m_layoutWidget==nullptr)||(m_layoutView==nullptr)) return;
  try
  {
    m_layoutView->load_layout(layoutFile.toStdString(),false);
    m_layoutView->add_missing_layers();

    m_layoutView->max_hier();
    m_layoutView->zoom_fit();
  }
  catch(...)
  {
    return;
  }

  a_DocName = layoutFile;
}
/**
 * @brief icLayout::saveLayoutClicked
 */

void icLayout::saveLayoutClicked()
{
  QString layoutFile = QFileDialog::getSaveFileName(this,"Save layout",lastDir, LayoutImportFilter);
  if (layoutFile.isEmpty()) return;


}
/**
 * @brief icLayout::selectAll
 */
void icLayout::selectAll()
{
  if (m_layoutView==nullptr) return;
  m_layoutView->select_all();
}

/**
 * @brief icLayout::getTechnology
 * @return
 */
tech* icLayout::getTechnology() {return m_technology;}
/**
 * @brief icLayout::setTechnology
 * @param ict Pointer to the technology object
 */
void  icLayout::setTechnology(tech* ict)
{
  m_technology=ict;

}


