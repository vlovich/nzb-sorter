#ifndef NZB_PARSER_H_
#define NZB_PARSER_H_

#include <QByteArray>
#include <QList>

struct NzbCollectionFile {
	QByteArray m_file;
	QByteArray m_subject;
};

typedef QList<NzbCollectionFile> NzbFileList;

struct NzbCollection {
	QByteArray m_xmlMagic;
	QByteArray m_openingTag;
	NzbFileList m_files;
	QList<QByteArray> m_nonFiles;
};

NzbCollection parse(QByteArray nzbFile);

#endif /* NZB_PARSER_H_ */

