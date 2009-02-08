#ifndef NZB_FILE_H_
#define NZB_FILE_H_

#include <QByteArray>
#include <QFile>
#include <QList>

class NzbFile
{
public:
	NzbFile(QString path);

	QList<QByteArray> sorted() const;

protected:
	void init();

private:
	QFile m_backingFile;
	QByteArray m_mmap;
};

#endif /* NZB_FILE_H_ */

