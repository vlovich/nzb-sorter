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

#ifndef NZB_PARSER_H_
#define NZB_PARSER_H_

#include <QByteArray>
#include <QList>

/**
 * Abstraction for the <file> tag.
 * m_file contains the complete data chunk between <file> & </file> up to the last >.
 * m_subject is populated as well with the contents of the file subject attribute.
 * It's filled in during parsing because we're already going to scan for </file> - 
 * look for subject along the way so that file access is as linear as possible.
 *
 * Not 100% because we're going to be comparing the m_subjects of various files.
 * TODO: use madvise on m_subject with MADV_SEQUENTIAL | MADV_WILLNEED
 * TODO: use madvise on all non-subject portions with MADV_SEQUENTIAL
 * TODO: use madvise to reset the advise for the entire nzb with MADV_SEQUENTIAL | MADV_WILLNEED
 */
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

/**
 * Really light-weight parser supporting nzb's only as
 * best as possible.  Shortcuts where possible assuming
 * fairly well-formed input.
 *
 * Generally, bad input will be caught and an exception thrown.
 * The parser should only fail to miss invalid tag names 
 * that start with the same letter as a valid tag: i.e. "<f! $>" will be 
 * interpreted as "<file>".
 *
 * The resultant NzbCollection is already sorted - I'm pretty sure the
 * performance is better to maintain a sorted list, rather than create the
 * list and then sort (big O is obviously the same).
 *
 * The only uncertainty is if it's still better in the worst case where the
 * input is in such a way that the n'th element always needs to be put into 
 * index 1 of the list.  This will cause n - 2 elements to be shifted.
 *
 * However, input from nzb search engines, as far as I can tell, usually come in
 * fairly sorted.  In this case, we'll definitely have better performance because
 * it'll just be insertion with the odd adjustment.
 */
NzbCollection parse(QByteArray nzbFile);

#endif /* NZB_PARSER_H_ */

