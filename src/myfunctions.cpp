#include "myfunctions.h"

#include <QDataStream>
#include <QIODevice>
#include <QCryptographicHash>
#include <QProcess>
#include <QDateTime>
#include <QFileInfo>
#include <unistd.h>

namespace mf {

	void removeDir(const QString &path)
	{
		if( !QDir( path ).exists() ) return;
		QDir dir = QDir( path );
		for( auto elem:dir.entryList( QStringList() << "*", QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot ) ){
			if( QDir( path + "/" + elem ).exists() ){
				mf::removeDir( path + "/" + elem );
				continue;
			}
			QFile::remove( path + "/" + elem );
		}
		QDir().rmdir( path );
	}

	QString getSize(const uint32_t val)
	{
		QStringList postFixs = QStringList()<<"b"<<"Kb"<<"Mb"<<"Gb"<<"Tb";
		float size = static_cast< float >( val );
		uint8_t index = 0;
		while( size > 1023.0 ){
			size /= 1024.0;
			index++;
			if( index >= postFixs.size() ) break;
		};
		return QString( "%1 %2" ).arg( size ).arg( postFixs[index] );
	}

#if __WORDSIZE == 64
	QString getSize(const uint64_t val)
	{
		QStringList postFixs = QStringList()<<"b"<<"Kb"<<"Mb"<<"Gb"<<"Tb";
		float size = static_cast< float >( val );
		uint8_t index = 0;
		while( size > 1023.0 ){
			size /= 1024.0;
			index++;
			if( index >= postFixs.size() ) break;
		};
		return QString( "%1 %2" ).arg( size ).arg( postFixs[index] );
	}
#endif

	QByteArray toBigEndianInt(const int val)
	{
		QByteArray bytes(4, 0x00);
		QDataStream stream(&bytes, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::BigEndian);
		stream << val;
		bytes.resize(4);
		return bytes;
	}

	QByteArray toBigEndianShort(const short val)
	{
		QByteArray bytes(2, 0x00);
		QDataStream stream(&bytes, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::BigEndian);
		stream << val;
		bytes.resize(2);
		return bytes;
	}

	unsigned int getRand(const int min, const int max)
	{
		return min + rand() % max;
	}

	QByteArray md5(const QByteArray &string)
	{
		return QCryptographicHash::hash(string, QCryptographicHash::Md5).toHex();
	}

	QByteArray sha1(const QByteArray &string)
	{
		return QCryptographicHash::hash(string, QCryptographicHash::Sha1).toHex();
	}

	QByteArray fileChecksum_MD5(const QString &file)
	{
		QFile f(file);
		if( f.open( QFile::ReadOnly ) ){
			QCryptographicHash hash( QCryptographicHash::Md5 );
			if (hash.addData(&f)) {
				return hash.result().toHex();
			}
		}
		return QByteArray();
	}

	QByteArray fileChecksum_SHA1(const QString &file)
	{
		QFile f(file);
		if( f.open( QFile::ReadOnly ) ){
			QCryptographicHash hash( QCryptographicHash::Sha1 );
			if (hash.addData(&f)) {
				return hash.result().toHex();
			}
		}
		return QByteArray();
	}

	bool startDetached(const QString &cmd, const QStringList &args, const QString &workPath)
	{
		QString str = cmd + " " + args.join(" ");
		auto res = QProcess::startDetached( cmd, args, workPath );
		return res;
	}

	bool checkFile(const char *fileName)
	{
		FILE *f;
		f = fopen(fileName,"r");
		if(f == nullptr) return false;
		fclose(f);
		delete f;
		return true;
	}

	bool checkFile(const QString &path)
	{
		return QFileInfo::exists(path) && QFileInfo(path).isFile();
	}

	bool strFind(const QString &inStr, const QString &dataStr)
	{
		bool ret = false;

		if( dataStr.isEmpty() || inStr.isEmpty() ) return ret;
		if( inStr.left(1) == "*" && inStr.right(1) == "*" ){
			QString findStr = inStr;
			findStr.remove( 0, 1 );
			findStr.remove( findStr.length() - 1, 1 );
			if( dataStr.contains( findStr, Qt::CaseInsensitive ) ) ret = true;
		}
		if( inStr.left(1) == "*" && inStr.right(1) != "*" ){
			QString findStr = inStr;
			findStr.remove( 0, 1 );
			if( dataStr.right( findStr.length() ) == findStr ) ret = true;
		}
		if( inStr.left(1) != "*" && inStr.right(1) == "*" ){
			QString findStr = inStr;
			findStr.remove( findStr.length() - 1, 1 );
			if( dataStr.indexOf( findStr, Qt::CaseInsensitive ) == 0 ) ret = true;
		}
		if( inStr.left(1) != "*" && inStr.right(1) != "*" ){ if( dataStr == inStr ) ret = true; }

		return ret;
	}

	QByteArray toLittleEndianShort(const short val)
	{
		QByteArray bytes(2, 0x00);
		QDataStream stream(&bytes, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::LittleEndian);
		stream << val;
		bytes.resize(2);
		return bytes;
	}

	void XOR(QByteArray &data, const QByteArray &key)
	{
		if( key.size() == 0 ) return;

		for (int i = 0; i < data.size(); i++) {
			data[i] = data[i] ^ key[ i % key.size() ];
		}
	}

	int pidOfProc(const QString &procName)
	{
		int res = -1;

		QDir dir = QDir( "/proc" );
		for( auto elem:dir.entryList( QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot ) ){
			bool test = false;
			int pid = elem.toInt( &test, 10 );
			if( !test ) continue;
			QString cmd = "";

			QFile file( QString( "/proc/%1/cmdline" ).arg( elem ) );
			if( file.open( QIODevice::ReadOnly ) ){
				cmd = file.read( 1024 );
				QStringList tmp = cmd.split( "/" );
				cmd = tmp.last();
				file.close();
			}

			if( cmd.isEmpty() || cmd != procName || pid == getpid() ){
				continue;
			}

			res = pid;
			break;
		}

		return res;
	}
}
