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
#include <QtAlgorithms>

#include "NzbParser.h"

enum NzbTag {
	Nzb,
	File,
	Groups,
	Group,
	Segments,
	Segment,
	NumTags,
};

static const char * nzbTagNames [] = { "nzb", "file", "groups", "group", "segments", "segment" };
static const int nzbTagLens[] = {
	sizeof("nzb") - 1,
       	sizeof("file") - 1,
       	sizeof("groups") - 1,
       	sizeof("group") - 1,
       	sizeof("segments") - 1,
       	sizeof("segment") - 1
};

static void checkClosingTag(const QByteArray &text, int offset, bool &foundS, bool supportS)
{
	foundS = false;
	if (offset >= text.size() || text.at(offset) != '>') {
		if (supportS && text.at(offset) == 's') {
			checkClosingTag(text, offset + 1, foundS, false);
			foundS = true;
		}
		else {
#ifdef _DEBUG
			qDebug() << offset << text.mid(std::max(offset - 10, 0), std::min(15, text.size() - std::max(offset - 10, 0)));
#endif /* _DEBUG */
			throw "Invalid closing tag";
		}
	}
}

static void checkClosingTag(const QByteArray &text, int offset)
{
	bool foundS;
	checkClosingTag(text, offset, foundS, false);
}

static void checkOpeningTag(const QByteArray &text, int offset, bool &foundS, bool supportS = false)
{
	foundS = false;

	if (offset >= text.size())
		throw "Invalid opening tag - abrupt end";

	switch (text.at(offset)) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return;
		case 's':
			if (supportS) {
				checkOpeningTag(text, offset + 1, foundS, false);
				foundS = true;
				break;
			}
		default:
			throw "Invalid opening tag - invalid character";
	}
}

static void checkOpeningTag(const QByteArray &text, int offset)
{
	bool foundS;
	checkOpeningTag(text, offset, foundS, false);
}

static bool isOpeningComment(const QByteArray &text, int offset)
{
#if 1
	// <!--
	if (text.size() - offset - 4 < 0)
		return false;
	return text.at(offset++) == '<' && text.at(offset++) == '!'
		&& text.at(offset++) == '-' && text.at(offset) == '-';
#else
	return text.mid(offset).startsWith("<!--");
#endif
}

static bool isClosingComment(const QByteArray &text, int offset)
{
#if 1
	// -->
	if (text.size() - offset - 3 < 0)
		return false;
	return text.at(offset++) == '-' && text.at(offset++) == '-'
		&& text.at(offset) == '>';
#else
	return text.mid(offset).startsWith("-->");
#endif
}

static NzbTag tagType(const QByteArray &text, int start, bool closing)
{
	if (text.at(start++) != '<') {
#ifdef _DEBUG
		qDebug() << text.mid(std::max(0, start - 3), 3);
#endif /* _DEBUG */
		throw "Attempting to find tag name of non-tag";
	}
	if (closing && text.at(start++) != '/')
		throw "Attempting to find closing tag name of non-closing tag";

	bool found;
	switch (text.at(start)) {
		case 'n':
			if (closing)
				checkClosingTag(text, start + nzbTagLens[Nzb]);
			else
				checkOpeningTag(text, start + nzbTagLens[Nzb]);
			return Nzb;
		case 'f':
			if (closing)
				checkClosingTag(text, start + nzbTagLens[File]);
			else
				checkOpeningTag(text, start + nzbTagLens[File]);
			return File;
		case 'g':
			if (closing)
				checkClosingTag(text, start + nzbTagLens[Group], found, true);
			else
				checkOpeningTag(text, start + nzbTagLens[Group], found, true);
			return !found ? Group : Groups;
		case 's':
			if (closing)
				checkClosingTag(text, start + nzbTagLens[Segment], found, true);
			else
				checkOpeningTag(text, start + nzbTagLens[Segment], found, true);
			return !found ? Segment : Segments;
		default:
#ifdef _DEBUG
			qDebug() << text.left(start) << "!!!AT NEXT CHAR!!!" << text.right(text.size() - start);
#endif /* _DEBUG */
			throw "Unknown nzb tag";
	}
	throw "Illegal - should've been handled by default case label";
}

static int findNextTag(const QByteArray &text, int start, bool &nonWhitespace, bool firstTag)
{
	int result;
	nonWhitespace = false;
	// don't need to check for string because we're searching for things between the closing of 1 tag
	// and the opening of the next
	for (result = start; result < text.size(); result++) {
		switch (text.at(result))
		{
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				break;
			case '<':
				if (!isOpeningComment(text, result)) {
					if (result + 1 >= text.size())
						return result;
					switch(text.at(result + 1))
					{
						case '?':
						case '!':
							if (!firstTag)
								throw "Unexpected characters after < - should only appear before opening nzb tag";
						case '/':
							break;
						default:
							return result;
					}
					result += 2;
				} else 
					result += 4;
			default:
				nonWhitespace = true;
				break;
		}
	}
	return -1;
}

static int findClosingTag(const QByteArray &text, NzbTag type, int start, bool inTag)
{
	int result;
	bool inString = false;
	bool inComment = false;
	if (type < 0 || type >= NumTags)
		throw "Internal error - tag type is invalid";

	for (result = start; result < text.size(); result++) {
		char c = text.at(result);
		if (c == '"')
			inString = !inString;
		if (inString || (inComment && c != '-'))
			continue;
		switch (c)
		{
			case '-':
				if (isClosingComment(text, result)) {
					if (!inComment)
						throw "Closing comment without opening one";
					inComment = false;
				}
				continue;
			case '<':
				if (isOpeningComment(text, result)) {
					if (inComment)
						throw "nested comments not supported";
					inComment = true;
					result += 4;
				}
				continue;
			case '>':
				inTag = false;
				continue;
			case '/':
				if (inTag) {
					if (++result > text.size())
						return -1;
					if (text.at(result) == '>')
						return result;
				}

				if (text.at(result - 1) == '<') {
					if (inString || inTag)
						throw "Corrupt nzb file - unexpected /";
					break;
				} 
				throw "Unexpected / outside of string";
			default:
				continue;
		}
		// currently, we are at </tag
		//                       ^--
		// tag type expects to  ^
		// be ------------------|
		if (type == tagType(text, result - 1, true))
			return result + nzbTagLens[type] + 1;
	}
	return -1;
}

static int findTagAttributesEnd(const QByteArray &text, int start)
{
	int result;
	bool textOpen = false;
	for (result = start; result < text.size(); result++) {
		switch (text.at(result)) {
			case '"':
				textOpen = !textOpen;
				break;
			case '>':
				if (!textOpen)
					return result;
		}
	}
	return -1;
}

static int findAttributeClose(const QByteArray &tag, int start)
{
	int result;
	for (result = start; result < tag.size(); result++) {
		if (tag.at(result) == '"')
			return result;
	}
	return -1;
}

static int findAttribute(const QByteArray &text, const QByteArray &attrName, int start)
{
	int matching = 0;
	int result;
	bool textOpen = false;
	int searchPos = start;
	char nextC = attrName.at(0);

	for (result = start; result < text.size(); result++) {
		char qc = text.at(result);

		if (!textOpen && qc == nextC) {
			if (++matching == attrName.size())
				return result;
			nextC = attrName.at(matching);
			continue;
		}

		switch (qc) {
			case '"':
				textOpen = !textOpen;
				break;
			case '>':
				return result;
		}
	}
	return -1;
}

NzbCollection parse(const QByteArray &remaining)
{
	NzbCollection result;
	int i, j;
	//QByteArray remaining = nzbFile;
	bool nonWhitespace;

	i = findNextTag(remaining, 0, nonWhitespace, true);
	if (i == -1)
		throw "Not a valid nzb file";
	if (Nzb != tagType(remaining, i, false))
		throw "<nzb> tag should be the first opening tag";

	result.m_xmlMagic = remaining.left(i);
	j = i;
	i = findTagAttributesEnd(remaining, i + (sizeof("<nzb") - 1));
	if (i == -1)
		throw "Not a valid nzb file";
	i++;
	result.m_openingTag = remaining.mid(j, i - j);
	Q_ASSERT(result.m_openingTag.startsWith("<nzb"));
	Q_ASSERT(result.m_openingTag.endsWith(">"));

	while (remaining.size()) {
		NzbCollectionFile file;
		int fileStart;

		j = i;
		fileStart = i = findNextTag(remaining, j, nonWhitespace, false);
#ifdef _DEBUG
		//qDebug() << remaining.mid(i, 10);
#endif /* _DEBUG */
		if (i == -1) {
			i = j;
			break;
		}
		if (nonWhitespace)
			result.m_nonFiles += remaining.mid(j, i - j);
		if (File != tagType(remaining, i, false))
			throw "Illegal structure in nzb file";
		i = findAttribute(remaining, "subject=", i + sizeof("<file ") - 1);
		if (i == -1)
			throw "File tag not closed (while searching for subject attribute)";
		Q_ASSERT(i < remaining.size());
		if (remaining.at(i) == '>')
			i--;
		else if (i + 1 < remaining.size() && remaining.at(i + 1) == '"') {
			j = i + 2;
			i = findAttributeClose(remaining, j);
			if (i == -1)
				throw "Subject attribute not closed";
			Q_ASSERT(remaining.at(i) == '"');
			file.m_subject = remaining.mid(j, i - j);
			Q_ASSERT(file.m_subject.at(file.m_subject.size() - 1) != '"');
			i++;
		} else
			throw "Illegal subject attribute";

		j = i;
		i = findClosingTag(remaining, File, j, true);
		if (i == -1)
			throw "File tag not closed (searching for contents of <file ...>...</file>)";
		i++;
		file.m_file = remaining.mid(fileStart, i - fileStart);
#ifdef _DEBUG
//		qDebug() << "!!!!!!\n" << file.m_file << "\n!!!!!!!!";
#endif /* _DEBUG */
		Q_ASSERT(file.m_file.startsWith("<file"));
		Q_ASSERT(file.m_file.endsWith("</file>"));
		NzbFileList::iterator pos = qUpperBound(result.m_files.begin(), result.m_files.end(), file);
		result.m_files.insert(pos, file);
	}
	j = i;
	i = findClosingTag(remaining, Nzb, j, false);
	if (i == -1)
		throw "nzb tag not closed";
	i -= (sizeof("</nzb>") - 1);
	Q_ASSERT(i >= j);
	if (i > j)
		result.m_nonFiles += remaining.mid(j, i - j);
	return result;
}

static bool isRar(const QByteArray &subject)
{
	return subject.contains(".rar&quot;") ||
		subject.contains(".rar yEnc") ||
		subject.contains(".rar - yEnc");
}

static bool operator< (const NzbCollectionFile &f1, const NzbCollectionFile &f2)
{
	bool f1Rar = isRar(f1.m_subject);
	bool f2Rar = isRar(f2.m_subject);
	if (f1Rar ^ f2Rar)
		return f1Rar;
	return f1.m_subject < f2.m_subject;
}

