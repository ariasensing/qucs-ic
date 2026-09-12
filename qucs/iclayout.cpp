#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "main.h"

#include "dbManager.h"          // optional, for undo/redo

#include "qtoolbox_selection_w.h"

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
  m_toolbox_dialog(nullptr)
{

  ui->setupUi(this);

  initMenuBar();

  // Technology
  if (!techfile.isEmpty())
    m_tech = tech::getTechFromFilename(techfile);

  if (m_tech==nullptr)
    m_tech = new tech(techfile);

  initKlayoutWidget();

  // Documents
  this->setProperty("DOC_TYPE",(uint16_t)(doc_type));

  attachToSchematic(owner);

  icLayout::setName(fname);

  if (!fname.isEmpty())
    icLayout::load();

  QToolbox_Selection_w* tbs = new QToolbox_Selection_w(this);

  m_toolbox_dialog = tbs;
  QVBoxLayout *toolboxLayout = new QVBoxLayout(ui->tabToolbox);
  toolboxLayout->addWidget(m_toolbox_dialog);
  ui->tabToolbox->setLayout(toolboxLayout);

  connect(this, &icLayout::new_mouse_position, tbs, &QToolbox_Selection_w::new_mouse_position);
}
/**
 * @brief icLayout::initKlayoutWidget
 * @return
 */
bool  icLayout::initKlayoutWidget()
{

  m_dbManager    = new db::Manager(true);
  m_layoutWidget = new lay::LayoutViewWidget(m_dbManager, true  , nullptr, this);
  if (m_layoutWidget==nullptr) return false;


  m_layoutView =  m_layoutWidget->view();

  if (m_layoutView == nullptr) return false;

  // Connect to the plugin

  // Add hierarchy
  QHBoxLayout* layout = new QHBoxLayout(ui->tabCellTree);
  layout->addWidget(m_layoutWidget->hierarchy_control_frame());
  ui->tabCellTree->setLayout(layout);
  // Layers
  QVBoxLayout *layersLayout = new QVBoxLayout(ui->tabLayers);
  layersLayout->addWidget(m_layoutWidget->layer_control_frame());
  ui->tabLayers->setLayout(layersLayout);


  QHBoxLayout *mainLayout = new QHBoxLayout(ui->centerFrame);
  mainLayout->addWidget(m_layoutWidget);

  int my_mode_id = -1;

  m_canvas_id = m_layoutView->create_layout(m_tech->getTechname().toStdString(),false);
  m_layout    = &(m_layoutView->active_cellview()->layout());
  assert(m_layout!=nullptr);

  std::vector<lay::Plugin*> plugins = m_layoutView->plugins();
  for (lay::Plugin* plugin : plugins)
  {
    if (plugin->plugin_declaration()->name()=="adv_layout")
    {
      my_mode_id = plugin->plugin_declaration()->id();
      break;
    }
  }

  if (my_mode_id >=0)
    m_layoutView->switch_mode(my_mode_id);



  applyTechToView();
  m_plugin = layAdvancedEditingPlugin::get_plugin_from_view(m_layoutView);

  // Connect the plugin instance to this
  connect(m_plugin, &layAdvancedEditingPlugin::update_mouse_position, this, &icLayout::update_mouse_position);

  connect((QToolbox_Selection_w*)(m_toolbox_dialog), &QToolbox_Selection_w::zoom_on_position,
          m_plugin, &layAdvancedEditingPlugin::zoom_on_new_position);

  return true;
}
/**
 * @brief icLayout::~icLayout
 */
icLayout::~icLayout() {
  delete ui;
  if (a_Schematic!=nullptr)
    a_Schematic->attachLayoutView();
  if (m_toolbox_dialog!=nullptr)
    delete m_toolbox_dialog;
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
void icLayout::setGridOn(bool )
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


void     icLayout::insert_started()
{
  // Create the dialog and put it in the toolbox
}

void     icLayout::insert_done(bool , const db::Shape& )
{

}

void     icLayout::update_mouse_position(double xdb, double ydb,  int ,  int )
{
  emit new_mouse_position(xdb,ydb);
}



