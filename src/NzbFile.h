#ifndef NZB_FILE_H_
#define NZB_FILE_H_

#include <QByteArray>
#include <QFile>
#include <QList>

/**
 * Abstraction class for nzb file.  Relies on m_mmap & 0-copy feature of
 * QByteArray to ensure that the only copy of the file that resides in
 * memory is whatever the OS pages in from disk.
 */
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

