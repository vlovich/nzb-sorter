#include <File.h>
#include <QXmlDefaultHandler>
#include <QTextStream>
#include <Subject.h>
#include <QStack>
#include <cassert>
#include <QOut.h>
#include <QXmlStreamWriter>

namespace nzb
{

class NZBParser : public QXmlDefaultHandler
{
public:
	NZBParser(File * file) : _lastSubject(""), _indent(0), _file(file), _tagHistory()
	{
	}
	
	bool startDocument()
	{
	//	qout << "Document started" << endl;
		_file->_entries.clear();
		_tagHistory.clear();
		_indent = 0;
		_lastSubject.clear();
		return true;
	}

	bool inTag(const QString & tag) const
	{
		return _tagHistory.top().compare(tag, Qt::CaseInsensitive) == 0;
	}

	bool isTag(const QString & tag, const QString & recieved) const
	{
		return tag.compare(recieved, Qt::CaseInsensitive) == 0;
	}

#define EXPAND(MACRO) #MACRO
#define IN_TAG_CHECK(TAG_TYPE) \
	bool in##TAG_TYPE##Tag() const \
	{ \
		return inTag(TAG_TYPE##_TAG); \
	}

#define IS_TAG_CHECK(TAG_TYPE) \
	bool is##TAG_TYPE##Tag(const QString & tag) const \
	{ \
		return isTag(TAG_TYPE##_TAG, tag); \
	}

#define TAG_CHECK(TAG_TYPE) \
	IN_TAG_CHECK(TAG_TYPE);\
	IS_TAG_CHECK(TAG_TYPE);

	TAG_CHECK(NZB)
	TAG_CHECK(FILE)
	TAG_CHECK(GROUPS)
	TAG_CHECK(GROUP)
	TAG_CHECK(SEGMENTS)
	TAG_CHECK(SEGMENT)

#define CHECK_ALLOWED(TAG_TYPE) \
	bool is##TAG_TYPE##ChildAllowed(const QString & child) const \
	{ \
		for (size_t i = 0; i < sizeof(TAG_TYPE##_ALLOWED) / sizeof(TAG_TYPE##_ALLOWED[0]); ++i) \
		{ \
			if (TAG_TYPE##_ALLOWED[i].compare(child, Qt::CaseInsensitive) == 0) \
			{ \
				return true; \
			} \
		} \
		return false; \
	}

	CHECK_ALLOWED( )
	CHECK_ALLOWED(NZB)
	CHECK_ALLOWED(FILE)
	CHECK_ALLOWED(GROUPS)
	CHECK_ALLOWED(SEGMENTS)
	//CHECK_ALLOWED(GROUP) // false
	//CHECK_ALLOWED(SEGMENT) // false

	Subject getSubject(const QXmlAttributes &attrs)
	{
		return Subject(attrs.value(SUBJECT_ATTR));
	}

	QString getPoster(const QXmlAttributes &attrs)
	{
		return attrs.value(POSTER_ATTR);
	}

	QString getDate(const QXmlAttributes & attrs)
	{
		return attrs.value(DATE_ATTR);
	}

	QString getNumBytes(const QXmlAttributes & attrs)
	{
		return attrs.value(BYTES_ATTR);
	}

	QString getNumber(const QXmlAttributes & attrs)
	{
		return attrs.value(NUMBER_ATTR);
	}

	QTextStream & indent() const
	{
		QString tabs;
		tabs.fill('\t', _indent);
		return qout << tabs;
	}

	bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
	{
		//indent() << "<" << name << ">" << endl;
		//indent() << "Current tag is " << name << ", parent is " << (_tagHistory.empty() ? "" : _tagHistory.top()) << endl;
		++_indent;
		if (_tagHistory.empty())
		{
			if (!isTag(name, NZB_TAG))
				return false;
		}
		else
		{
			if (inNZBTag())
			{
				if (isNZBChildAllowed(name))
				{
					assert(isTag(name, FILE_TAG));
	
					_lastSubject = getSubject(attrs);
					//qout << "Inserting subject " << _lastSubject.data() << endl;
					_file->_entries.insert(_lastSubject, Entry(getPoster(attrs), getDate(attrs), _lastSubject.data()));
				}
				else
				{
					errChild(name, _tagHistory.top());
					return false;
				}
			}
			else if (inFILETag())
			{
				if (!isFILEChildAllowed(name))
				{
					errChild(name, _tagHistory.top());
					return false;
				}
			}
			else if (inGROUPSTag())
			{
				if (isGROUPSChildAllowed(name))
				{
					assert(!_file->_entries.empty());
					if (isTag(name, GROUP_TAG))
					{
						//qout << "Inserting group" << endl;
						_file->_entries[_lastSubject].addGroup(Group());
					}
				}
				else
				{
					errChild(name, _tagHistory.top());
					return false;
				}
			}
			else if (inSEGMENTSTag())
			{
				if (isSEGMENTSChildAllowed(name))
				{
					if (isTag(name, SEGMENT_TAG))
					{
						//qout << "Inserting segment" << endl;
						_file->_entries[_lastSubject].addSegment(Segment(getNumBytes(attrs), getNumber(attrs)));
					}
				}
				else
				{
					errChild(name, _tagHistory.top());
					return false;
				}
	
			}
			else
			{
				errNoChildren(name, _tagHistory.top());
				return false;
			}
		}
		_tagHistory.push(name);
		return true;
	}

	bool characters(const QString & ch)
	{
		if (inGROUPTag())
		{
			//qout << "setting group value to " << ch << endl;
			assert(_file->_entries[_lastSubject].hasGroups());
			_file->_entries[_lastSubject].lastGroup().setName(ch);
		}
		else if (inSEGMENTTag())
		{
			//qout << "setting segment value to " << ch << endl;
			assert(_file->_entries[_lastSubject].hasSegments());
			_file->_entries[_lastSubject].lastSegment().setName(ch);
		}
		return true;
	}

	bool endElement( const QString&, const QString&, const QString &name )
	{
		if (_tagHistory.empty() || !isTag(name, _tagHistory.top()))
		{
			return false;
		}

		--_indent;
		//indent() << "</" << name << ">" << endl;
		_tagHistory.pop();
		return true;
	}

private:
	static const QString NZB_TAG;
	static const QString FILE_TAG;
	static const QString GROUPS_TAG;
	static const QString GROUP_TAG;
	static const QString SEGMENTS_TAG;
	static const QString SEGMENT_TAG;
	
	static const QString _ALLOWED [1];
	static const QString NZB_ALLOWED [1];
	static const QString FILE_ALLOWED [2];
	static const QString GROUPS_ALLOWED [1];
	static const QString GROUP_ALLOWED [0];
	static const QString SEGMENTS_ALLOWED [1];
	static const QString SEGMENT_ALLOWED [0];
	
	static const QString POSTER_ATTR;
	static const QString DATE_ATTR;
	static const QString SUBJECT_ATTR;
	static const QString BYTES_ATTR;
	static const QString NUMBER_ATTR;

	Subject _lastSubject;

	int _indent;
	void errNoChildren(const QString & childTag, const QString & parent) const
	{
		qerr << "Cannot have a child element (" << childTag << ") within " << parent << endl;
	}

	void errChild(const QString & childTag, const QString & parent) const
	{
		qerr << "<" << childTag << "> tag not allowed as child of <" << parent << ">" << endl;
	}

	File * _file;
	QStack<QString> _tagHistory;
};

const QString NZBParser::NZB_TAG = "nzb";
const QString NZBParser::FILE_TAG = "file";
const QString NZBParser::GROUPS_TAG = "groups";
const QString NZBParser::GROUP_TAG = "group";
const QString NZBParser::SEGMENTS_TAG = "segments";
const QString NZBParser::SEGMENT_TAG = "segment";

const QString NZBParser::_ALLOWED [] = { NZB_TAG };
const QString NZBParser::NZB_ALLOWED [] = { FILE_TAG };
const QString NZBParser::FILE_ALLOWED [] = { GROUPS_TAG, SEGMENTS_TAG };
const QString NZBParser::GROUPS_ALLOWED [] = { GROUP_TAG };
const QString NZBParser::GROUP_ALLOWED [] = {};
const QString NZBParser::SEGMENTS_ALLOWED [] = { SEGMENT_TAG };
const QString NZBParser::SEGMENT_ALLOWED [] = {};

const QString NZBParser::POSTER_ATTR = "poster";
const QString NZBParser::DATE_ATTR = "date";
const QString NZBParser::SUBJECT_ATTR = "subject";
const QString NZBParser::BYTES_ATTR = "bytes";
const QString NZBParser::NUMBER_ATTR = "number";

File::File(const QString & path) : _path(path)
{
	init();
}

void File::init()
{
	NZBParser handler(this);

	QXmlSimpleReader reader;
	QFile file(_path);
	QXmlInputSource input (&file);
	reader.setContentHandler(&handler);
	
	if(!reader.parse(&input, false))
	{
		throw QXmlParseException("XML document incomplete");
	}
}

void File::resave(QIODevice & device) const
{
	QXmlStreamWriter writer(&device);
	writer.setAutoFormatting(true);
	writer.setCodec("iso 8859-1");

	writer.writeStartDocument();
	writer.writeDTD("<!DOCTYPE nzb PUBLIC \"-//newzBin//DTD NZB 1.0//EN\" \"http://www.newzbin.com/DTD/nzb/nzb-1.0.dtd\">");
	writer.writeStartElement("nzb");
	writer.writeAttribute("xmlns", "http://www.newzbin.com/DTD/2003/nzb");

	for(QMap<Subject, Entry>::const_iterator i = _entries.constBegin(); i != _entries.constEnd(); ++i)
	{
		i.value().toString(writer);
	}

	writer.writeEndDocument();
}

}

