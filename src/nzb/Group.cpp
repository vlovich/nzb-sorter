#include <Group.h>

namespace nzb
{

Group::Group(const QString & name) : _name(name)
{
}

void Group::toString(QXmlStreamWriter & writer) const
{
	writer.writeStartElement("group");
	writer.writeCharacters(_name);
	writer.writeEndElement();
}

}

