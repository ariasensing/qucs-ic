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
  m_tech(nullptr),
  ui(new Ui::icLayout),
  a_Schematic(nullptr),
  m_layout(nullptr),
  m_technologyFile(techfile),
  m_shapeDrawer(nullptr)
{

  ui->setupUi(this);

  initMenuBar();

  // Technology
  if (!techfile.isEmpty())
    m_tech = tech::getTechFromFilename(techfile);

  if (m_tech==nullptr)
    m_tech = new tech(techfile);

  initKlayoutWidget();

  // create the visual cursor marker
  mp_cursor = new lay::ShapeMarker(m_layoutView, m_layoutView->active_cellview_index());
  mp_cursor->set_frame_color(0x00ff00);
  mp_cursor->set_line_width(1);
  mp_cursor->set_vertex_size(9);
  mp_cursor->set_dither_pattern(1);   // hollow

  // Documents
  this->setProperty("DOC_TYPE",(uint16_t)(doc_type));

  attachToSchematic(owner);

  icLayout::setName(fname);

  if (!fname.isEmpty())
    icLayout::load();


}
/**
 * @brief icLayout::initKlayoutWidget
 * @return
 */
bool  icLayout::initKlayoutWidget()
{
  m_dbManager    = new db::Manager(true);
  m_layoutWidget = new lay::LayoutViewWidget(m_dbManager, true  , nullptr);

  if (m_layoutWidget==nullptr) return false;


  // Add hierarchy
  QHBoxLayout* layout = new QHBoxLayout(ui->tabCellTree);
  layout->addWidget(m_layoutWidget->hierarchy_control_frame());
  ui->tabCellTree->setLayout(layout);
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

  m_canvas_id = m_layoutView->create_layout(m_tech->getTechname().toStdString(),false);
  m_layout    = &(m_layoutView->cellview(0)->layout());
  assert(m_layout!=nullptr);

  applyTechToView();

  // The real canvas that receives mouse events

  m_layoutWidget->installEventFilter(this);
  // Init the cursor mode
  // after construction
  setMagnetic(true);
  setCatchDistance(0.6);
  setGrid(0.0);                 // use the view’s editor grid

  // only snap to metal1 & via
  clearMagneticLayers();
  addMagneticLayer(255, 255);
  addMagneticLayer(255, 255);
  return true;
}
/**
 * @brief icLayout::~icLayout
 */
icLayout::~icLayout() {
  delete ui;
  if (a_Schematic!=nullptr)
    a_Schematic->attachLayoutView();
  if (m_shapeDrawer!=nullptr)
    delete m_shapeDrawer;

  if (mp_cursor!=nullptr) delete mp_cursor;
  mp_cursor = nullptr;
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
QString icLayout::getTechnology() {return m_technologyFile;}
/**
 * @brief icLayout::setTechnology
 * @param ict Pointer to the technology object
 */
void  icLayout::setTechnology(QString ict)
{
  m_technologyFile=ict;

  if (m_technologyFile == m_tech->getFilename())
    return;

  // Check if we need to delete m_tech (if it was created here)
  tech* query_tech = tech::getTechFromFilename(m_tech->getFilename());
  if (query_tech == nullptr)
  {
    // It means that the current technology is not in the list of available one. We may delete it.
    delete m_tech;
    m_tech = nullptr;
  }

  // Get the new one
  query_tech = tech::getTechFromFilename(m_technologyFile);
  if (query_tech == nullptr)
    m_tech = new tech(m_technologyFile);
  else
    m_tech = query_tech;

  applyTechToView();
}

/**
 * @brief icLayout::setGridOn
 * @param value
 */
void icLayout::setGridOn(bool value)
{

}


/**
 * @brief icLayout::getGridOn
 * @return
 */
bool icLayout::getGridOn()
{
  return false;
}

/**
 * @brief icLayout::applyTechToView
 */
void  icLayout::applyTechToView()
{
  return;
  // Update technology and layers
  if (m_tech!=nullptr)
  {
   // if (db::Technologies::instance()->has_technology(m_tech->getTechname().toStdString()))
   //     m_layout->set_technology_name(m_tech->getTechname().toStdString());

    m_tech->copyLayersToView(m_layoutView, true);
  }
  m_layoutView->add_missing_layers();
  m_layoutView->update_content();

}



