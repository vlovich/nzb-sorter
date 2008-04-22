#include <Segment.h>

namespace nzb
{

Segment::Segment(const QString & numBytes, const QString & number) :
	_numBytes(numBytes), _number(number), _name("")
{
}

void Segment::toString(QXmlStreamWriter & writer) const
{
	writer.writeStartElement("segment");
	writer.writeAttribute("bytes", _numBytes);
	writer.writeAttribute("number", _number);
	writer.writeCharacters(_name);
	writer.writeEndElement();
}

}

