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

#include <QCoreApplication>
#include <QStringList>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QTextStream>
#include <QtDebug>

#include "NzbFile.h"

static void printUsage(const QString &progName, QTextStream &out)
{
	out << progName << " <<input nzb 1> <nzb 1 out>> [<<input nzb 2> <nzb 2 out>> ...]" << endl;
	out << progName << " <input nzb>" << endl;
}

static bool sortNzb(const QString &inputFile, QTextStream &qout, QTextStream &error)
{
	QList<QByteArray> sorted;
	try {
		NzbFile nf(inputFile);
//		sorted = nf.sorted();
		nf.printSorted(qout);
	} catch(const char *msg) {
		error << "Failure to process " << inputFile << ": " << msg << endl;
		return false;
	} catch(...) {
		error << "Unexpected exception processing " << inputFile << endl;
		return false;
	}

//	if (sorted.size() == 0)
//		return false;

//	foreach (QByteArray part, sorted)
//		qout << part;

	return true;
}

static bool sortNzb(const QString &inputFile, const QString &outputFile, QTextStream &errStream)
{
	QFile output(outputFile);
	if (!output.open(QIODevice::WriteOnly))
		return false;

	QTextStream outStream(&output);
	return sortNzb(inputFile, outStream, errStream);
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QTextStream qout(stdout, QIODevice::WriteOnly);
	QTextStream qerr(stderr, QIODevice::WriteOnly);
	QStringList arguments = QCoreApplication::arguments();
	QString progName;
	QStringList nzbList;

	Q_ASSERT(arguments.size() > 0);

	progName = arguments.first();
	if (arguments.size() > 1)
		nzbList = arguments.mid(1);

	if (nzbList.size() == 0) {
		qerr << "No files given" << endl;
		printUsage(progName, qerr);
		return 1;
	}

	if (nzbList.size() != 1 && nzbList.size() % 2 != 0) {
		qerr << "Invalid Arguments" << endl;
		printUsage(progName, qerr);
		return 2;
	}

	bool allFailed = true;
	bool partialFailed = false;

	if (nzbList.size() == 1) {
		allFailed = sortNzb(nzbList.first(), qout, qerr);
	} else {
		for (int i = 0; i < nzbList.size(); i+= 2) {
			bool failed = sortNzb(nzbList.at(i), nzbList.at(i + 1), qerr);
			allFailed = allFailed && failed;
			partialFailed = partialFailed || failed;
		}
	}
	if (allFailed)
		return 3;
	if (partialFailed)
		return 4;
	return 0;
}

