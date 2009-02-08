/*
 *   Copyright 2009 Vitali Lovich
 *
 *   This file is part of nzb-sorter.
 *
 *   nzb-sorter is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   nzb-sorter is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with nzb-sorter.  If not, see <http://www.gnu.org/licenses/>.
*/

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

	void printSorted(QTextStream &destination) const;
	QList<QByteArray> sorted() const;

protected:
	void init();

private:
	QFile m_backingFile;
	QByteArray m_mmap;
};

#endif /* NZB_FILE_H_ */

