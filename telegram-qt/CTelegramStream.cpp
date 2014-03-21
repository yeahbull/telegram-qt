#include "CTelegramStream.hpp"

#include <QtEndian>

#include <QIODevice>
#include <QDebug>

static const char s_nulls[4] = { 0, 0, 0, 0 };

CTelegramStream::CTelegramStream(QIODevice *d) :
    m_device(d)
{

}

void CTelegramStream::setDevice(QIODevice *newDevice)
{
    m_device = newDevice;
}

void CTelegramStream::unsetDevice()
{
    m_device = 0;
}

bool CTelegramStream::atEnd() const
{
    return m_device ? m_device->atEnd() : true;
}

CTelegramStream &CTelegramStream::operator>>(qint32 &i)
{
    return *this;
}

CTelegramStream &CTelegramStream::operator>>(qint64 &i)
{
    return *this;
}

CTelegramStream &CTelegramStream::operator<<(qint32 i)
{
    m_device->write((const char *) &i, 4);

    return *this;
}

CTelegramStream &CTelegramStream::operator<<(qint64 i)
{
    m_device->write((const char *) &i, 8);

    return *this;
}

CTelegramStream &CTelegramStream::operator<<(const QString &str)
{
    if (str.length() < 0xfe) {
        const char lengthToWrite = str.length();
        m_device->putChar(lengthToWrite);
        m_device->write(str.toUtf8());

        if ((str.length() + 1) & 3) {
            m_device->write(s_nulls, 4 - ((str.length() + 1) & 3));
        }

    } else {
        *this << quint32((str.length() << 8) + 0xfe);
        m_device->write(str.toUtf8());

        if (str.length() & 3) {
            m_device->write(s_nulls, 4 - (str.length() & 3));
        }
    }

    return *this;
}
