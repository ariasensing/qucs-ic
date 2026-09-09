#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo
#include "main.h"
#include "layLayoutView_qt.h"

/**
 * @brief icLayout::slotInsertRect Start the insertion of a rectangle
 */
void     icLayout::slotInsertRect()
{
  if (m_layoutWidget==nullptr) return;

  // Terminate any previous editing
  terminatePreviousInsertion();

  // Enable edit coordinates
  editCoordinates->setEnabled(true);

  m_shapeDrawer = new RectangleDrawer(m_layoutView, this);
  m_shapeDrawer->start();
}


void     icLayout::slotInsertPath(){}
void     icLayout::slotInsertVia(){}
void     icLayout::slotInsertInstance(){}
void     icLayout::slotInsertPolygon(){}
void     icLayout::slotInsertCircle(){}




void     icLayout::terminatePreviousInsertion()
{
  if (m_shapeDrawer!=nullptr)
    delete m_shapeDrawer;

  m_shapeDrawer = nullptr;
}