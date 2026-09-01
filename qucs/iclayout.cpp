#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo

/**
 * @brief icLayout::icLayout
 * @param app
 * @param owner
 * @param fname
 */
icLayout::icLayout(QucsApp* app, Schematic* owner, const QString& fname) : QDialog(nullptr), QucsDoc(app, fname, LAYOUT),
  ui(new Ui::icLayout),
  a_Schematic(nullptr)
{
  QString filename = fname;

  ui->setupUi(this);

  initKlayoutWidget();

  // Documents
  this->setProperty("DOC_TYPE",(uint16_t)(doc_type));

  attachToSchematic(owner);

  if (fname.isEmpty())
    if ((owner!=nullptr)&&(!owner->getDocName().isEmpty()))
    {
      QFileInfo Info(owner->getDocName());
      QString base = Info.completeBaseName();
      filename = base.append(".lay");
    }

  if (fname.isEmpty())
      filename = QString("[NONAME]");

  if (!filename.isEmpty())
  {
    icLayout::load();
  }
}

bool  icLayout::initKlayoutWidget()
{
  m_dbManager = new db::Manager(true);
  m_layPlugin = new lay::Plugin();
  m_layoutView = new lay::LayoutView(m_dbManager, true,m_layPlugin);
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

  return false;

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
