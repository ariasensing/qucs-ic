#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo
#include "main.h"
#include "layLayoutView_qt.h"
void     icLayout::slotInsertRect()
{
  if (m_layoutViewWidget==nullptr) return;
  // Enable edit coordinates
  editCoordinates->setEnabled(true);

  m_layoutViewWidget


}


void     icLayout::slotInsertPath(){}
void     icLayout::slotInsertVia(){}
void     icLayout::slotInsertInstance(){}
void     icLayout::slotInsertPolygon(){}
void     icLayout::slotInsertCircle(){}