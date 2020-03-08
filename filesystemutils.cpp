#include "filesystemutils.hpp"

#include <QDebug>
#include <QDirIterator>

filesystemUtils::filesystemUtils()
{

}

QStringList filesystemUtils::getFiles(QString dir) {
  QStringList collect;
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    if (it.fileName() == "." || it.fileName() == ".." || it.fileName() == "") {
      it.next();
      continue;
    }
    collect.append(it.fileInfo().absoluteFilePath());
    it.next();
  }
  return collect;
}

QString filesystemUtils::renameFile(QString fileName, QString search,
                                    QString replace) {
  QString newFileName;

  // Was ist wenn das was ich unbennenen möchte ein Hautverzeichnis war?
  // Deswegen muss die Liste schon rückwärts kommen.

  if (fileName.contains(search, Qt::CaseInsensitive)) {
    newFileName = fileName;
    newFileName.replace(search, replace);
  }

  return newFileName;
}
