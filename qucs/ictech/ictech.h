#ifndef ICTECH_H
#define ICTECH_H

#include "ictech_global.h"
#include "substrate.h"
#include <QStringList>
#include <QHash>
#include <QSet>

#include "dbTechnology.h"
#include "dbLayerProperties.h"   // LayerInfo
#include "dbLayerMapping.h"
#include "layLayoutView.h"
#include "layLayerProperties.h"
#include <QDir>


/*
 * A technlogy is organized as follows.
 * tech_file (corners, files information)
 * tech_file.data (main technology folder)
 *   |--> tech_file.layout folder
 *   |              |
 *   |              |-> lyt (techfile.lyt)
 *   |              |-> lyp (techfile.lyp)
 *   |              |-> tech (klayout) (techfile.tech)
 *   |
 *   |--> tech_file.models folder (FEOL models)
 *   |              |
 *   |              | -> deck1, 2, 3...
 *   |
 *   |--> tech_file.libraries folder
 *   |              |
 *   |              |-> symmap file (symbol mapping file)
 *   |              |-> symbol1, 2, 3 ...
 *   |
 *   |--> tech_file.stdcells folder
 *   |              |
 *   |              |->tlef file
 *   |              |->lef  file
 *   |              |->spice decks
 *   |              |->gds file
 *
 *
 *   When creating a tech (still empty), no folders are created.
 *   When saving a tech -> the folders are created if needed, each file is stored. kLayout tech is also registered
 *   When loading
*/

namespace tinyxml2
{
class XMLNode;
class XMLElement;
class XMLDocument;
}


class ICTECH_EXPORT tech {
public:
  tech(QString filename="");
  ~tech();

  void  addModelCorner(const QString& corner);
  void  addSubstrateCorner(const QString& corner);
  void  removeModelCorner(const QString& corner);
  void  removeSubstrateCorner(const QString& corner);
  QStringList getModelCorners();
  QStringList getSubstrateCorners();
  bool  assignSubstrateCornerToModelCorner(QString modelCorner, QString subCorner);
  bool  hasModelCorner(const QString& modelCorner);
  bool  hasSubstrateCorner(const QString& substrateCorner);
  bool  loadSubstrateData(const QString& corner, const QString& filename);
  bool  save(bool make_available);
  bool  load();
  bool  load(const QString& filename);
  bool  saveToFile(const QString& filename, bool make_available);
  QString getFilename();
  QString getLastError() {return m_lastError;}
  QString getTechname()    {return m_techName;}
  void   copyFrom(tech* t2);

private:

  void        clean();

  bool        m_isEmpty;
  QString     m_techName;
  QString     m_fileName;
  // We need to add all different substrate corners
  // They are stored as pure identifiers
  QStringList               m_modelCorners;
  QStringList               m_substrateCorners;
  QHash<QString,Substrate*> m_Substrates;
  QString                   m_lastError;

  db::Technology*         m_ktech;           // Definition of layers
  lay::LayoutView*        m_layoutView;   // Dummy view
  db::Layout*             m_layout;       // Dummy layout

  // All files here are the basename
  QString            m_layout_tech_file;  // Storage point of layout technologies
  QString            m_layout_lyp_file;   // Storage point of layout properties
  QSet<QString>      m_model_files;       // Stored model files
  QSet<QString>      m_substrate_files;   // Substrate files
  QSet<QString>      m_symbol_files;      // Symbol files
  QHash<QString, QString> m_subcktSymbols;// Mapping from subckt and symbols

  QDir               getTechnologyBaseFolder();

  void               saveLayoutData(class tinyxml2::XMLElement* rootLayout, class tinyxml2::XMLDocument* doc);
  void               saveModelData(class tinyxml2::XMLElement* rootLayout, class tinyxml2::XMLDocument* doc) {}
  void               saveLibData(class tinyxml2::XMLElement* rootLayout, class tinyxml2::XMLDocument* doc) {}
  void               saveStdCellData(class tinyxml2::XMLElement* rootLayout, class tinyxml2::XMLDocument* doc) {}
  void               saveEMData(class tinyxml2::XMLElement* rootLayout, class tinyxml2::XMLDocument* doc) {}

  void   createDefaultFileNames();
// Saving

public:

  // klayout
  void              create_klayout_tech();
  void              remove_klayout_tech();
  bool              import_klayout_tech_file();
  bool              import_klayout_layerdefs(const QString& newLypFile="");


  QString           getLayoutFolder();
  QString           getSpiceModelsFolder();
  QString           getEMFolder();
  QString           getLibrariesFolder();
  QString           getStdCellsFolder();

  QStringList       getSpiceModelsFiles();
  QString           getLayoutFilepath();

private:
  // Here we keep the list of saved technologies
  static QSet<tech*> m_availableTechs;

public:
  void                  rename(const QString& newname);
  static tech*          getTechFromFilename(const QString& filename);
  static tech*          getTechFromName(const QString& techname);
  static QStringList    getAvailableTechs();
  void                  makeAvailableForTheProject();
  void                  removeFromProject();
  void                  setDescription(const QString& descr);
  QString               getDescription();
  lay::LayoutView*      getLayoutView() {return m_layoutView;}

  double                dbu() {return m_ktech->dbu();}
  void                  dbu(double dbu) {if (dbu>0) m_ktech->set_dbu(dbu);}
  QString               grid() {return QString(m_ktech->default_grids());}
  void                  grid(QString grid_list) {m_ktech->set_default_grids(grid_list.toStdString());}
  QString               getLypFile() {return m_layout_lyp_file;}
  QString               getLytFile() {return m_layout_tech_file;}

};

#endif // ICTECH_H
