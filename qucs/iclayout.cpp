#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo


icLayout::icLayout(QucsApp* app, const QString& fname) : QDialog(nullptr), QucsDoc(app, fname, LAYOUT),
  ui(new Ui::icLayout),
  a_Schematic(nullptr)
{
  ui->setupUi(this);
  this->setProperty("DOC_TYPE",(uint16_t)(doc_type));
}

icLayout::~icLayout() {
  delete ui;
  if (a_Schematic!=nullptr)
    a_Schematic->attachLayoutView();
}

void  icLayout::setName(const QString& Name_)
{
  a_DocName = Name_;
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