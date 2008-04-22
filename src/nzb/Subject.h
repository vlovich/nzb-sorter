#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <QString>
#include <QTextStream>
#include <QRegExp>
#include <stdio.h>

namespace nzb
{

class Subject
{
public:
	Subject(const QString & copy) : _str(copy)
	{
	}

	Subject(const Subject & copy) : _str(copy._str)
	{
	}

	bool operator< (const Subject & otherS) const
	{
		if (this == &otherS)
			return false;
		QTextStream qout(stdout);

		const QString other = otherS.data();

		bool thisEndsInRar = endsWithRar(), otherEndsInRar = endsWithRar(other);
		//qout << "\t" << _str << " ends with rar: " << thisEndsInRar << endl;

		if (thisEndsInRar && !otherEndsInRar || !thisEndsInRar && otherEndsInRar)
		{
			/*if (thisEndsInRar)
			qout << "\t" << _str << " before " << other << endl;
			else
			qout << "\t" << _str << " after " << other << endl;*/
			// one or the other, not both
			return thisEndsInRar;
		}

		return _str.compare(other) < 0;
	}

	QString data() const
	{
		return _str;
	}

	void clear()
	{
		_str = QString();
	}

private:
	static QRegExp _rarCheck;

	bool endsWithRar() const
	{
		return endsWithRar(_str);
	}

	static bool endsWithRar (const QString & str)
	{
		return -1 != str.lastIndexOf(".rar", -1, Qt::CaseInsensitive);
	}

	static bool endsWithRar (const Subject & subj)
	{
		return endsWithRar(subj._str);
	}

	QString _str;
};

}

#endif /* SUBJECT_H_ */

