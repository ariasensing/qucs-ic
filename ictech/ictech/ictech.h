#ifndef ICTECH_H
#define ICTECH_H

#include "ictech_global.h"


#include "Substrate.h"


#include <QStringList>
class ICTECH_EXPORT tech {
public:
  tech(QString name);
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
  bool  loadSubstrateData(const String& corner, const QString& filename);
private:
  QString     m_techName;
  // We need to add all different substrate corners
  // They are stored as pure identifiers
  QStringList m_modelCorners;
  QStringList m_substrateCorners;
  QHash<QString,Substrate*> m_Substrates;
};

#endif // ICTECH_H
