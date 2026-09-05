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


/*
 * A technlogy is organized as follows.
 * tech_file (corners, files information)
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
 *   |--> tech_file.symbols folder
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

  db::Technology    *m_laydefs;           // Definition of layers
  // All files here are the basename
  QString            m_layout_tech_file;  // Storage point of layout properties
  QString            m_layout_lyp_file;   // Storage point of layout properties
  QSet<QString>      m_model_files;       // Stored model files
  QSet<QString>      m_substrate_files;   // Substrate files
  QSet<QString>      m_symbol_files;      // Symbol files
  QHash<QString, QString> m_subcktSymbols;// Mapping from subckt and symbols

  void   createDefaultFileNames();
public:

  // klayout
  void              create_klayout_tech();
  void              remove_klayout_tech();
  bool              import_klayout_tech_file();
  bool              import_klayout_layerdefs(const QString& newLypFile="");


  QString           getLayoutFolder();
  QString           getLayoutFilepath();

  // Here we keep the list of saved technologies
  static QSet<tech*> m_availableTechs;

public:
  void                  rename(const QString& newname);
  static tech*          getTechFromFilename(const QString& filename);
  static tech*          getTechFromName(const QString& techname);
  static QStringList    getAvailableTechs();
  void                  makeAvailableForTheProject();
  void                  removeFromProject();
};

#endif // ICTECH_H
