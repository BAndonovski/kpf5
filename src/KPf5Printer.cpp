#include "KPf5Printer.h"

#include <QFile>
#include <QByteArray>
#include <SerialPort.h>

namespace kPF5{

int kPF5Printer::_count_seq = 0x31;

kPF5Printer::kPF5Printer(const QString& PortName, const int& Lag)
        :_port_name(PortName)
        ,_lag(Lag)
{
    _serial = new SerialPort(PortName.toStdString());
}

kPF5Printer::~kPF5Printer()
{    
    if(_serial != 0x0)
    {

        if(_serial->IsOpen()) _serial->Close();
        delete _serial;
        _serial = 0x0;
    }
}

bool kPF5Printer::open_port()
{
    if(_serial->IsOpen()) return true;

    if(_port_name.isEmpty()) return false;

    try{

        _serial->Open(SerialPort::BAUD_9600, SerialPort::CHAR_SIZE_8, SerialPort::PARITY_NONE, SerialPort::STOP_BITS_1, SerialPort::FLOW_CONTROL_NONE);
    }
    catch(...) { return false; }
    return true;
}

int kPF5Printer::get_seq()
{
    _count_seq = _count_seq == 0x7f ? 0x20 : _count_seq + 0x1;
    return _count_seq;
}

void kPF5Printer::get_bcc(QByteArray& bytes)
{            
    unsigned long long sum = 0;
    for(int i=1;i<bytes.length();++i)
    {
        unsigned int tmp = (unsigned char) bytes[i];
        sum += tmp;
    }    
    QString bcc_bytes = QString::number(sum,16).rightJustified(4,'0');

    for(int i=0;i<bcc_bytes.length();++i)
        bytes.append(bcc_bytes.mid(i,1).toUInt(0x0,16) + 0x30);
}

void kPF5Printer::exec(const QByteArray& cmd)
{    
    if(open_port())
    {        
        _lastCommand = cmd;
        //nekogas u buduce ova treba da se naprai da funkcionira dobro
        bool sent = false;
        std::string str = QString(cmd).toStdString()+"\0";
        SerialPort::DataBuffer buff;
        while(!sent)
        {
            _serial->Write(str);
            try
            {
                _serial->Read(buff,101,_lag);
                sent = true;
            }
            catch(...)
            {
                //ova tuka bi trebalo da bide false
                //ali printerot sere mnogu
                //i ne uspevam da procitam nisto
                //pa taka cekam, go koristam samo kao time out
                sent = true;
            }
        }
    }
}

void kPF5Printer::send_cmd(const Commands &cmd, const QByteArray& bytes)
{
    QByteArray cmd_bytes;
    //preamble <01>
    cmd_bytes.append(0x1);
    //len
    cmd_bytes.append(bytes.length()+ 4 + 0x20);
    //seq
    cmd_bytes.append(get_seq());
    //cmd
    cmd_bytes.append(cmd);
    //data
    if(bytes != (QByteArray)0x0) cmd_bytes.append(bytes);
    //postamble
    cmd_bytes.append(0x5);
    //bcc
    get_bcc(cmd_bytes);
    //terminator
    cmd_bytes.append(0x3);
    exec(cmd_bytes);
}

void kPF5Printer::PrintDiagnostics() { send_cmd(CMD_DIAGNOSTICS); }

void kPF5Printer::SetHeader(const QList<QString> &Lines)
{
//    if(Lines.length() < 2 || Lines.length() > 6) return;

    QByteArray cmd;
    for(int i=0;i<Lines.length();++i)
    {
        cmd.clear();
        cmd.append(i + 0x30);
        cmd.append(Lines[i]);        
        send_cmd(CMD_HEADER,cmd);
    }
}

void kPF5Printer::PrintCheck(QList<CheckEntry> &Check, const bool& Store)
{
    if(Check.length() < 1) return;
    QByteArray line;
    //QList<unsigned char> line;
    //otvori
    send_cmd(Store?CMD_STORE_OPEN:CMD_CHECK_OPEN, "1,0000,1");
    //prodazba
    double sum = 0;
    for(int i=0;i<Check.length();++i)
    {
        if(Check[i].Amount() == 0) continue;

        line.clear();
        //prv red L1
        line.append(Check[i].Product.left(25));
        //line feed
        line.append(0x0A);
        //vtor red L2
        //line.append(QString::number(Check[i].Quantity,'g',2));
        line.append("");
        //tab
        line.append(0x09);
        //TaxCode
        line.append(Check[i].Tax);
        //Sign
        //line.append('+');
        //Price        
        line.append(QString("%1*%2").arg(
                //QString::number(Check[i].Price,'f',2),
                RoundMKD(Check[i].Price),
                QString::number(Check[i].Quantity,'f',2)));
        //suma.. za posle
        sum += Check[i].Price * Check[i].Quantity;
        send_cmd(CMD_CHECK_REG, line);
    }

    //total
    line.clear();
    //line 1
    line.append("Total *** ");
    //tab
    line.append(0x09);
    //sign
    line.append('+');
    //amount    
    line.append(RoundMKD(sum));
    send_cmd(CMD_CHECK_TOTAL,line);
    //zatvori
    send_cmd(Store?CMD_STORE_CLOSE:CMD_CHECK_CLOSE);
}

void kPF5Printer::SetDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QByteArray bytes;
    bytes.append(QDate::currentDate().toString("dd-MM-yy"));
    bytes.append(" ");
    bytes.append(QTime::currentTime().toString("HH:mm:ss"));
    send_cmd(CMD_SET_DATETIME, bytes);
}

void kPF5Printer::PrintDailyFinancialReport(const bool& toNull)
{
    QByteArray bytes;
    bytes.append(toNull?0x31:0x33);
    send_cmd(CMD_DAILY_REPORT, bytes);
}

void kPF5Printer::ServiceMoney(const long& Amount)
{
    QByteArray bytes;
    bytes.append(QString::number(Amount));
    send_cmd(CMD_SERVICE_MONEY, bytes);
}

void kPF5Printer::PrintPeriodicalReport(const QDate& From, const QDate& To)
{
    QByteArray bytes;
    bytes.append(From.toString("ddMMyy"));
    bytes.append(',');
    //bytes.append(Type);
    //bytes.append(',');
    bytes.append(To.toString("ddMMyy"));
    send_cmd(CMD_PERIOD_REPORT, bytes);
}

QString kPF5Printer::RoundMKD(double Value)
{
    double b = Value - (int) Value;
    if(b<0.25) Value -= b;
    else if(b>0.25 && b<0.5) Value+=0.5-b;
    else if(b>0.5 && b<0.75) Value-=b-0.5;
    else if(b>0.75) Value += 1-b;
    return QString::number(Value,'f',2);
}

void kPF5Printer::PrintReportNumber(const unsigned &from, const unsigned &to) {
    QByteArray bytes;
    bytes.append(QString::number(from));
    bytes.append(',');
    bytes.append(QString::number(to));
    send_cmd(CMD_REPORT_NUMBER, bytes);
}

void kPF5Printer::repeatLastCommand() {
    if (!_lastCommand.isNull()) {
        exec(_lastCommand);
    }
}



}
