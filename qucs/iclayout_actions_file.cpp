#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo
#include "main.h"

extern QString LayoutImportFilter;
/**
 * @brief icLayout::slotFileNew. Create a new layout
 */
void     icLayout::slotFileNew()
{

}
/**
 * @brief slotFileOpen
 */
void     icLayout::slotFileOpen()
{
  QString layoutFile = QFileDialog::getOpenFileName(this,"Load layout file",lastDir, LayoutImportFilter);
  if (layoutFile.isEmpty()) return;

  if ((m_layoutWidget==nullptr)||(m_layoutView==nullptr)) return;
  try
  {
    m_layoutView->load_layout(layoutFile.toStdString(), m_tech->getTechname().toStdString(), false);
    applyTechToView();
    m_layoutView->add_missing_layers();

    m_layoutView->max_hier();
    m_layoutView->zoom_fit();
    m_layoutView->update_content();

  }
  catch(...)
  {
    return;
  }

  a_DocName = layoutFile;
}
/**
 * @brief icLayout::slotFileSave
 */
void     icLayout::slotFileSave()
{
  QString layoutFile = QFileDialog::getSaveFileName(this,"Save layout",lastDir, LayoutImportFilter);
  if (layoutFile.isEmpty()) return;
}
/**
 * @brief icLayout::slotFileSaveAs
 */
void     icLayout::slotFileSaveAs()
{

}
/**
 * @brief icLayout::slotFileClose
 */
void     icLayout::slotFileClose()
{

}
/**
 * @brief icLayout::slotFileSettings
 */
void     icLayout::slotFileSettings()
{

}

/**
 * @brief icLayout::slotFilePrint
 */
void     icLayout::slotFilePrint()
{

}