#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H
#include <QDir>

namespace mf {
	void removeDir(const QString &path);
	QString getSize(const uint32_t val);
#if __WORDSIZE == 64
	QString getSize(const uint64_t val);
#endif
	QByteArray toBigEndianInt(const int val);
	QByteArray toBigEndianShort(const short val);
	unsigned int getRand(const int min, const int max);
	/**
	 * @brief md5
	 * Get md5 summ for string
	 * @param string
	 * @return
	 */
	QByteArray md5(const QByteArray &string);
	QByteArray sha1(const QByteArray &string);
	QByteArray fileChecksum_MD5(const QString &file);
	QByteArray fileChecksum_SHA1(const QString &file);
	bool startDetached(const QString &cmd, const QStringList &args = QStringList(), const QString &workPath = QDir::homePath());
	bool checkFile(const char *fileName);
	bool checkFile(const QString &path);
	bool strFind(const QString &inStr, const QString &dataStr);
	QByteArray toLittleEndianShort(const short val);
	void XOR(QByteArray &data, const QByteArray &key);
	int pidOfProc(const QString &procName);
}

#endif // MYFUNCTIONS_H
