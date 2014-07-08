#ifndef QHTTPBASE_HPP
#define QHTTPBASE_HPP

#include "qhttpserverfwd.hpp"

#include <QTcpSocket>
#include <QHostAddress>
#include <QUrl>

#include "http-parser/http_parser.h"

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
///////////////////////////////////////////////////////////////////////////////

class HttpBase
{
public:
    THeaderHash         iheaders;
    QString             iversion;
};

///////////////////////////////////////////////////////////////////////////////

class HttpRequestBase : public HttpBase
{
public:
    QUrl                iurl;
    THttpMethod         imethod;
};

///////////////////////////////////////////////////////////////////////////////

class HttpResponseBase : public HttpBase
{
public:
    HttpResponseBase() {
        istatus     = ESTATUS_BAD_REQUEST;
        iversion    = "1.1";
    }

public:
    TStatusCode         istatus;
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
class HttpParserBase
{
public:
    explicit     HttpParserBase(http_parser_type type) : isocket(nullptr) {
        // create http_parser object
        iparser.data  = static_cast<T*>(this);
        http_parser_init(&iparser, type);

        memset(&iparserSettings, 0, sizeof(http_parser_settings));
        iparserSettings.on_message_begin    = onMessageBegin;
        iparserSettings.on_url              = onUrl;
        iparserSettings.on_status           = onStatus;
        iparserSettings.on_header_field     = onHeaderField;
        iparserSettings.on_header_value     = onHeaderValue;
        iparserSettings.on_headers_complete = onHeadersComplete;
        iparserSettings.on_body             = onBody;
        iparserSettings.on_message_complete = onMessageComplete;
    }

    virtual     ~HttpParserBase() {
    }

    void         parse(const char* data, size_t length) {
        http_parser_execute(&iparser, &iparserSettings,
                            data, length);
    }

public: // callback functions for http_parser_settings
    static int   onMessageBegin(http_parser* parser) {
        T *me = static_cast<T*>(parser->data);
        return me->messageBegin(parser);
    }

    static int   onUrl(http_parser* parser, const char* at, size_t length) {
        T *me = static_cast<T*>(parser->data);
        return me->url(parser, at, length);
    }

    static int  onStatus(http_parser* parser, const char* at, size_t length) {
        T *me = static_cast<T*>(parser->data);
        return me->status(parser, at, length);
    }

    static int   onHeaderField(http_parser* parser, const char* at, size_t length) {
        T *me = static_cast<T*>(parser->data);
        return me->headerField(parser, at, length);
    }

    static int   onHeaderValue(http_parser* parser, const char* at, size_t length) {
        T *me = static_cast<T*>(parser->data);
        return me->headerValue(parser, at, length);
    }

    static int   onHeadersComplete(http_parser* parser) {
        T *me = static_cast<T*>(parser->data);
        return me->headersComplete(parser);
    }

    static int   onBody(http_parser* parser, const char* at, size_t length) {
        T *me = static_cast<T*>(parser->data);
        return me->body(parser, at, length);
    }

    static int   onMessageComplete(http_parser* parser) {
        T *me = static_cast<T*>(parser->data);
        return me->messageComplete(parser);
    }

public:
    QTcpSocket*             isocket;

    // The ones we are reading in from the parser
    QByteArray              itempHeaderField;
    QByteArray              itempHeaderValue;

private:
    http_parser             iparser;
    http_parser_settings    iparserSettings;
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
class HttpWriterBase
{
public:
    explicit    HttpWriterBase(QTcpSocket* sok) : isocket(sok) {
        iheaderWritten   = false;
        ifinished        = false;
        itransmitLen     = itransmitPos    = 0;

        QObject::connect(isocket, &QTcpSocket::bytesWritten, [this](qint64 byteCount){
            updateWriteCount(byteCount);
        });
    }

    virtual    ~HttpWriterBase() {
    }

public:
    bool        addHeader(const QByteArray &field, const QByteArray &value) {
        if ( ifinished )
            return false;

        static_cast<T*>(this)->iheaders.insert(field.toLower(), value);
        return true;
    }

    bool        writeHeader(const QByteArray& field, const QByteArray& value) {
        if ( ifinished )
            return false;

        QByteArray buffer = QByteArray(field)
                            .append(": ")
                            .append(value)
                            .append("\r\n");
        writeRaw(buffer);
        return true;
    }

    bool        writeData(const QByteArray& data) {
        if ( ifinished )
            return false;

        static_cast<T*>(this)->ensureWritingHeaders();
        writeRaw(data);
        return true;
    }

    bool        endPacket(const QByteArray& data) {
        if ( !writeData(data) )
            return false;

        isocket->flush();
        ifinished = true;
        return true;
    }

protected:
    void        updateWriteCount(qint64 count) {
        Q_ASSERT(itransmitPos + count <= itransmitLen);

        itransmitPos += count;

        if ( itransmitPos == itransmitLen ) {
            itransmitLen = 0;
            itransmitPos = 0;
            static_cast<T*>(this)->allBytesWritten();
        }
    }

    void        writeRaw(const QByteArray &data) {
        isocket->write(data);
        itransmitLen += data.size();
    }

public:
    QTcpSocket*         isocket;

    bool                iheaderWritten;
    bool                ifinished;

    // Keep track of transmit buffer status
    qint64              itransmitLen;
    qint64              itransmitPos;
};

///////////////////////////////////////////////////////////////////////////////
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
#endif // QHTTPBASE_HPP