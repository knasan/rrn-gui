#include "filesystemutils.hpp"

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QString>

filesystemUtils::filesystemUtils() {}

QStringList filesystemUtils::getFiles(QString dir) {
  QStringList collect;
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    if (it.fileName() == "" || it.fileName() == "." || it.fileName() == "..") {
      continue;
    }
    // qDebug() << "FileName: " << it.fileInfo();
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

QString filesystemUtils::renameFile(QString fileName, QString search,
                                    QString replace) {
  QString newFileName;
  QString newFileNameFullPath;

  // Was ist wenn das was ich unbennenen möchte ein Hautverzeichnis war?
  // Deswegen muss die Liste schon rückwärts kommen.

  // Get BaseName
  QFile file;
  QFileInfo fileInfo;
  QStringList stringlist;

  file.setFileName(fileName);
  fileInfo.setFile(file);

  /*qDebug() << "Search:  " << search;
  qDebug() << "Replace: " << replace;
  qDebug() << "FileName: " << fileName;*/

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

    // TODO; wie unter Windows?
    stringlist << fileInfo.absolutePath() << newFileName + "." + FileExtension;

    newFileNameFullPath = stringlist.join("/");
    stringlist.clear();
    file.rename(newFileNameFullPath);
  }

  // qDebug() << "From: " << fileName << " To: " << newFileName;
  return newFileNameFullPath;
}
