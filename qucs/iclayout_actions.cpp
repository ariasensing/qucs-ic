#include "iclayout.h"
#include "ui_iclayout.h"
#include "schematic.h"
#include "dbManager.h"          // optional, for undo/redo
#include "main.h"

/**
 * @brief icLayout::initActions
 */
void    icLayout::initActions()
{
  //-----------------------------------------------------
  //-----------------------------------------------------
  // File actions
  //-----------------------------------------------------
  //-----------------------------------------------------
  // File new
  fileNew = new QAction(QIcon((":/bitmaps/svg/filenew.svg")), tr("&New"), this);
  fileNew->setShortcut(QKeySequence::New);
  fileNew->setStatusTip(tr("Creates a new empty layout"));
  fileNew->setWhatsThis(
      tr("New\n\nCreates a new layout "));
  connect(fileNew, SIGNAL(triggered()), SLOT(slotFileNew()));

  // File open
  fileOpen = new QAction(QIcon((":/bitmaps/svg/open-file-folder-icon.svg")), tr("&Open"), this);
  fileOpen->setShortcut(QKeySequence::New);
  fileOpen->setStatusTip(tr("Open an existing layout file"));
  fileOpen->setWhatsThis(
      tr("Open\n\nOpen a layout file (e.g. gdsii, oasis) and attach to current layout view"));
  connect(fileOpen, SIGNAL(triggered()), SLOT(slotFileOpen()));
  //-----------------------------------------------------
  //-----------------------------------------------------
  // Insert actions
  //-----------------------------------------------------
  //-----------------------------------------------------
  insertRect = new QAction(QIcon((":/bitmaps/svg/rectangle.svg")), tr("&Rect"), this);
  insertRect->setShortcut(Qt::Key_R | Qt::CTRL);
  insertRect->setStatusTip(tr("Add a rectangle"));
  insertRect->setWhatsThis(
      tr("Rect\n\nInsert a new rect in the layout"));
  connect(insertRect, SIGNAL(triggered()), SLOT(slotInsertRect()));

  //-----------------------------------------------------
  //-----------------------------------------------------
  // Edit actions
  //-----------------------------------------------------
  //-----------------------------------------------------

  //-----------------------------------------------------
  //-----------------------------------------------------
  // View actions
  //-----------------------------------------------------
  //-----------------------------------------------------

  //-----------------------------------------------------
  //-----------------------------------------------------
  // Select actions
  //-----------------------------------------------------
  //-----------------------------------------------------

  //-----------------------------------------------------
  //-----------------------------------------------------
  // EM actions
  //-----------------------------------------------------
  //-----------------------------------------------------

  //-----------------------------------------------------
  //-----------------------------------------------------
  // Schematic actions
  //-----------------------------------------------------
  //-----------------------------------------------------



  //  on the fly coordinates numerical entry
  editCoordinates = new QAction(this);
  editCoordinates->setShortcut(QKeySequence::AddTab);
  connect(editCoordinates, SIGNAL(triggered()), SLOT(slotEditCoordinates()));
  editCoordinates->setEnabled(false);
}

/**
 * @brief initMenuBar
 */
void    icLayout::initMenuBar()
{
  initActions();

  menuBar = new QMenuBar(this);

  fileMenu = new QMenu(tr("&File")); // menuBar entry fileMenu
  fileMenu->addAction(fileNew);
  fileMenu->addAction(fileOpen);

  editMenu = new QMenu(tr("&Edit")); // menuBar entry Edit
  //fileOpen->addAction(ICplxTrans);

  viewMenu = new QMenu(tr("&View"));

  selectMenu = new QMenu(tr("Select"));

  insMenu = new QMenu(tr("&Insert")); // menuBar entry Insert
  insMenu->addAction(insertRect);

  emMenu = new QMenu(tr("E&M Sim")); // menuBar entry fileMenu
  //emMenu->addAction(emMenu);

  schematicMenu = new QMenu("&Schematic");

  menuBar->addMenu(fileMenu);
  menuBar->addMenu(editMenu);
  menuBar->addMenu(insMenu);
  menuBar->addMenu(viewMenu);
  menuBar->addMenu(selectMenu);
  menuBar->addMenu(emMenu);
  menuBar->addMenu(schematicMenu);
  layout()->setMenuBar(menuBar);


}


/*
bool icLayout::eventFilter(QObject *obj, QEvent *event)
{
  QEvent::Type type = event->type();
  if ((type == QEvent::MouseButtonPress)&&(m_shapeDrawer==nullptr))
  {
    QMessageBox::information(this,"msg","click without drawer");
  }

  if (m_shapeDrawer!=nullptr)
  {
    m_shapeDrawer->eventFilter(obj, event);
    if ((type == QEvent::MouseButtonPress))
    {
      QMessageBox::information(this,"msg","click with drawer");
    }


  }

  return QDialog::eventFilter(obj,event);

}
*/