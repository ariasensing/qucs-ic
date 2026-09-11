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
  if (m_plugin==nullptr) return;
  if (!m_plugin->is_idle()) m_plugin->terminate_action();
  if (m_toolbox_dialog!=nullptr) delete m_toolbox_dialog;
  m_toolbox_dialog = nullptr;

  // Enable edit coordinates
  editCoordinates->setEnabled(true);

  // Create box numerical toolbox
  m_plugin->insert_rect_start();
}


void     icLayout::slotInsertPath(){}
void     icLayout::slotInsertVia(){}
void     icLayout::slotInsertInstance(){}
void     icLayout::slotInsertPolygon(){}
void     icLayout::slotInsertCircle(){}


