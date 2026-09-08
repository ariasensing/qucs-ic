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
  //
  fileNew = new QAction(QIcon((":/bitmaps/svg/filenew.svg")), tr("&New"), this);
  fileNew->setShortcut(QKeySequence::New);
  fileNew->setStatusTip(tr("Creates a new empty layout"));
  fileNew->setWhatsThis(
      tr("New\n\nCreates a new layout "));
  connect(fileNew, SIGNAL(triggered()), SLOT(slotFileNew()));

  //
  fileOpen = new QAction(QIcon((":/bitmaps/svg/open-file-folder-icon.svg")), tr("&Open"), this);
  fileOpen->setShortcut(QKeySequence::New);
  fileOpen->setStatusTip(tr("Open an existing layout file"));
  fileOpen->setWhatsThis(
      tr("Open\n\nOpen a layout file (e.g. gdsii, oasis) and attach to current layout view"));
  connect(fileOpen, SIGNAL(triggered()), SLOT(slotFileOpen()));

  insertRect = new QAction(QIcon((":/bitmaps/svg/rectangle.svg")), tr("&Rect"), this);
  fileOpen->setShortcut(QKeySequence::);
  fileOpen->setStatusTip(tr("Add a rectangle"));
  fileOpen->setWhatsThis(
      tr("Rect\n\nInsert a new rect in the layout"));
  connect(insertRect, SIGNAL(triggered()), SLOT(slotInsertRect()));


  //
  editCoordinates = new QAction(this);
  fileOpen->setShortcut(QKeySequence::AddTab);
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

  editMenu = new QMenu(tr("&Edit")); // menuBar entry Edit
  fileMenu->addAction(fileNew);

  insMenu = new QMenu(tr("&Insert")); // menuBar entry Insert
  fileMenu->addAction(fileNew);

  emMenu = new QMenu(tr("E&M Sim")); // menuBar entry fileMenu
  fileMenu->addAction(fileNew);

  schematicMenu = new QMenu("&Schematic");
  menuBar->addMenu(fileMenu);
  layout()->setMenuBar(menuBar);


}
