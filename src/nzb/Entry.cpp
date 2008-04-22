#include <Entry.h>

namespace nzb
{

Entry::Entry(const QString & poster, const QString & date, const QString & subject) :
	_poster(poster), _date(date), _subject(subject)
{
}

void Entry::toString(QXmlStreamWriter & writer) const
{
	writer.writeStartElement("file");
	writer.writeAttribute("poster", _poster);
	writer.writeAttribute("date", _date);
	writer.writeAttribute("subject", _subject);
	
	writer.writeStartElement("groups");
	for (QVector<Group>::const_iterator i = _groups.begin(); i != _groups.end(); ++i)
	{
		(*i).toString(writer);
	}
	writer.writeEndElement();

	writer.writeStartElement("segments");
	for(QVector<Segment>::const_iterator i = _segments.begin(); i != _segments.end(); ++i)
	{
		(*i).toString(writer);
	}
	writer.writeEndElement(); // segments

	writer.writeEndElement(); // file
}

}

