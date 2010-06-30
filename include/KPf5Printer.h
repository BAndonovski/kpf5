#ifndef KPF5PRINTER_H
#define KPF5PRINTER_H

#include <sys/signal.h>
#include <sys/types.h>

#include <QObject>
#include <QDateTime>
#include <SerialPort.h>
#include "KPF5_Data.h"

namespace kPF5{

class kPF5Printer : public QObject
{
    Q_OBJECT

public:
    static QString RoundMKD(double Value);
    kPF5Printer(const QString& PortName = 0x0, const int& Lag = 200);
    virtual ~kPF5Printer();

    void PrintDiagnostics();
    void SetHeader(const QList<QString> &Lines);
    void PrintCheck(QList<CheckEntry> &Check, const bool& Store = false);
    void SetDateTime();
    void PrintDailyFinancialReport(const bool& toNull = false);
    void PrintPeriodicalReport(const QDate& From, const QDate& To);
    void ServiceMoney(const long& Amount);
    void PrintReportNumber(const unsigned &from, const unsigned &to);
    void repeatLastCommand();
private:
    static int _count_seq;    
    QString _port_name;
    int _lag;
    SerialPort *_serial;
    QByteArray _lastCommand;

    int get_seq();
    void get_bcc(QByteArray& bytes);
    bool open_port();
    void exec(const QByteArray& cmd);

    void send_cmd(const Commands &cmd, const QByteArray& bytes = 0x0);
};
}
#endif // KPF5PRINTER_H
