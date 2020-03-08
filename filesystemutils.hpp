#ifndef FILESYSTEMUTILS_HPP
#define FILESYSTEMUTILS_HPP

#include <QObject>
#include <QStringList>

class filesystemUtils : public QObject {
  Q_OBJECT
 public:
  filesystemUtils();

 public slots:
  QStringList getFiles(QString dir);
  QString renameFile(QString fileName, QString search, QString replace);
};

#endif // FILESYSTEMUTILS_HPP
