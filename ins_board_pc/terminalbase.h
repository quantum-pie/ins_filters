#ifndef TERMINALBASE_H
#define TERMINALBASE_H

#include "udpsender.h"
#include "udpreceiver.h"

#include <QObject>

class TerminalBase : public QObject
{
    Q_OBJECT

public:
    TerminalBase(const QString & client_ip, uint16_t client_port,
                 const QString & server_ip, uint16_t server_port);

public slots:
    void send_text(const std::string & str);

signals:
    void text_received(const std::string & ar);

private:
    UDPSender output_sock;
    UDPReceiver input_sock;
};

#endif // TERMINALBASE_H