#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <QOut.h>
#include <QString>
#include <QVector>
#include <cassert>
#include <QXmlStreamWriter>

namespace nzb
{

using namespace std;

class Segment
{
public:
	Segment() : _numBytes(), _number(), _name() {}
	Segment(const QString & numBytes, const QString & number);
	bool setName(const QString & name) { assert(_name.isEmpty()); if (!_name.isEmpty()) { return false; } _name = name; return true; }
	void toString(QXmlStreamWriter & writer) const;
private:
	QString _numBytes;
	QString _number;
	QString _name;
};

}

#endif /* SEGMENT_H_ */

