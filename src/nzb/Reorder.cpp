#include <File.h>
#include <stdio.h>
#include <QFile>

using namespace std;

int main(int argc, char ** argv)
{
	if (argc != 2)
		return 1;
	//cout << argv[1] << endl;
	nzb::File * file = new nzb::File(argv[1]);
	QFile outFile;
	outFile.open(stdout, QIODevice::WriteOnly);
	file->resave(outFile);
	outFile.close();
	return 0;
}
