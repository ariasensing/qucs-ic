#include "ictech.h"
#include <QFileInfo>
#include <QDir>
/**
 * All procedures related to the spice model decks inside ictech are here
 * */

const QString relative_models_folder="./models/";
/**
 * @brief tech::getSpiceModelsFolder
 * @return The complete folder for the spice models section of the technology
 */
QString           tech::getSpiceModelsFolder()
{
  if (m_fileName.isEmpty()) return QString("");
  QDir    baseDir         = getTechnologyBaseFolder();
  QDir    modelDir(baseDir.absoluteFilePath(relative_models_folder));
  return  modelDir.absolutePath();
}

/**
 * @brief tech::getSpiceModelsFilePaths
 * @return All the complete paths/filename for every spice model deck included
 */
QStringList       tech::getSpiceModelsFiles()
{
  return QStringList();

}
