#include "ictech.h"
#include <QFileInfo>
#include <QDir>
/**
 * All procedures related to the spice model decks inside ictech are here
 * */

const QString relative_stdcells_folder="./stdcells/";
/**
 * @brief tech::getSpiceModelsFolder
 * @return The complete folder for the spice models section of the technology
 */
QString           tech::getStdCellsFolder()
{
  if (m_fileName.isEmpty()) return QString("");
  QDir    baseDir         = getTechnologyBaseFolder();
  QDir    modelDir(baseDir.absoluteFilePath(relative_stdcells_folder));
  return  modelDir.absolutePath();
}

