#ifndef GROUP_H_
#define GROUP_H_

#include <QString>
#include <QVector>
#include <cassert>
#include <QOut.h>
#include <QXmlStreamWriter>

namespace nzb
{

class Group
{
public:
	Group() : _name() {}
	Group(const QString & name);
	bool setName(const QString & name) { /*qout << "Setting group name to " << name << endl;*/ assert(_name.isEmpty()); if (_name.isEmpty()) {_name = name; return true; } return false; }
	void toString(QXmlStreamWriter & writer) const;
private:
	QString _name;
};

}

#endif /* GROUP_H_ */

