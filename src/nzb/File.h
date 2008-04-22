#ifndef FILE_H_
#define FILE_H_

#include <QString>
#include <QVector>
#include <QMap>
#include <Entry.h>
#include <QIODevice>

namespace nzb
{

class NZBParser;
class Subject;

class File
{
public:
	File(const QString & path);
	void resave(QIODevice & device) const;

private:
	void init();

	friend class NZBParser;

	QString _path;
	QString _dtdName;
	QString _dtdPubId;
	QString _dtdSysId;
	QMap<Subject, Entry> _entries;
};

}

#endif /* NZB_FILE_H_ */

