#include "filesystemutils.hpp"

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QString>

filesystemUtils::filesystemUtils() {}

/**
 * @brief filesystemUtils::getFiles
 * @param dir
 * @return
 */
QStringList filesystemUtils::getFiles(QString dir) {
  QStringList collect;
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    if (it.fileName() == "" || it.fileName() == "." || it.fileName() == "..") {
      continue;
    }
    if (it.fileInfo().absolutePath().contains(dir)) {
      collect.append(it.fileInfo().absoluteFilePath());
    } else {
      qDebug() << it.fileInfo();
    }
  }
  collect.removeDuplicates();
  collect.sort(Qt::CaseInsensitive);
  return collect;
}

/**
 * @brief filesystemUtils::renameFile
 * @param fileName
 * @param search
 * @param replace
 * @return
 */
QString filesystemUtils::renameFile(QString fileName, QString search,
                                    QString replace) {
  QString newFileName;
  QString newFileNameFullPath;
  QFile file;
  QFileInfo fileInfo;
  QStringList stringlist;

  file.setFileName(fileName);
  fileInfo.setFile(file);

  if (fileInfo.isDir()) {
    if (fileName.contains(search, Qt::CaseInsensitive)) {
      newFileName = fileName;
      newFileName.replace(search, replace);
      if (newFileName == "") {
        return newFileName;
      }
      file.rename(newFileName);
      return newFileName;
    }
    return fileName;
  }

  QString BaseNameFromFileName = fileInfo.baseName();
  QString FileExtension = fileInfo.suffix();

  if (BaseNameFromFileName.contains(search, Qt::CaseInsensitive)) {
    newFileName = BaseNameFromFileName;
    newFileName.replace(search, replace);
    stringlist << fileInfo.absolutePath() << newFileName + "." + FileExtension;
    newFileNameFullPath = stringlist.join("/");
    stringlist.clear();
    file.rename(newFileNameFullPath);
  }

  qDebug() << "From: " << fileName << " To: " << newFileName;
  return newFileNameFullPath;
}
