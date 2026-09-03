#ifndef ICTECH_H
#define ICTECH_H

#include "ictech_global.h"


#include "substrate.h"


#include <QStringList>
#include <QHash>
#include <QSet>
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
  bool  save();
  bool  load();
  bool  saveToFile(const QString& filename);
  QString getFilename();
private:
  void        copyFrom(const tech& t2);
  void        clean();

  bool        m_isEmpty;
  QString     m_techName;
  QString     m_fileName;
  // We need to add all different substrate corners
  // They are stored as pure identifiers
  QStringList m_modelCorners;
  QStringList m_substrateCorners;
  QHash<QString,Substrate*> m_Substrates;
  // Here we keep the list of saved technologies
  //            filename, tech*
  static QHash<QString,tech*> m_availableTechs;
  //            techname, filename
  static QHash<QString, QString> m_mapNameToFiles;

  void   appendThisToAvailable();
  void   removeThisFromAvailable();
  void   renameThis(const QString& newname);

public:

  static tech*  getTechFromFilename(QString& filename);
  static tech*  getTechFromName(QString& techname);
  static QStringList getAvailableTechs();
  static QString getFilenameFromTech(const QString &tech);
  static QString getTechFromFilename(const QString &filename);

};

#endif // ICTECH_H
