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

#include <limits>
#include <QtDebug>

#include "NzbFile.h"
#include "NzbParser.h"

NzbFile::NzbFile(QString path)
	: m_backingFile(path)
{
	init();
}

void NzbFile::init()
{
	const char *mapped;
	qint64 mapSize;

	if (!m_backingFile.open(QIODevice::ReadOnly))
		throw "Unable to open file";

	mapSize = m_backingFile.size();

	if (mapSize <= 0)
		throw "File is too small";

	if (mapSize > std::numeric_limits<int>::max())
		throw "File is too big";

	mapped = reinterpret_cast<char *>(m_backingFile.map(0, mapSize));

	if (NULL == mapped)
		throw "Unable to map file";

	m_mmap = QByteArray::fromRawData(mapped, mapSize);
}

QList<QByteArray> NzbFile::sorted() const
{
	QList<QByteArray> result;
	NzbCollection collection = parse(m_mmap);
	result += collection.m_xmlMagic;
	result += collection.m_openingTag;
	// No need to sort because the parser stores things sorted
	// as it's parsing
	//qSort(collection.m_files);
	for (int i = 0; i < collection.m_files.size(); i++) {
		result += collection.m_files.at(i).m_file;
	}

	result += collection.m_nonFiles;
	result += "</nzb>";
	return result;
}

