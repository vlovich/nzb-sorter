#ifndef ENTRY_H_
#define ENTRY_H_

#include <QString>
#include <QVector>
#include <Group.h>
#include <Segment.h>
#include <QXmlStreamWriter>

namespace nzb
{

class Entry
{
public:
	Entry() : _poster(), _date(), _subject(), _groups(), _segments() {}
	Entry(const QString & poster, const QString & date, const QString & subject);
	bool hasGroups() { return !_groups.empty(); }
	Group & lastGroup() { return _groups.back(); }
	bool hasSegments() { return !_segments.empty(); }

	void addGroup(const Group & group) { _groups.push_back(group); }
	void addSegment(const Segment & segment) { _segments.push_back(segment); }
	Segment & lastSegment() { return _segments.back(); }

	void toString(QXmlStreamWriter & writer) const;

private:
	QString _poster;
	QString _date;
	QString _subject;
	QVector<Group> _groups;
	QVector<Segment> _segments;
};

}

#endif /* ENTRY_H_ */

