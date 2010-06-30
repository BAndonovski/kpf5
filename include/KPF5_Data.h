#ifndef KPF5_DATA_H
#define KPF5_DATA_H

#include <QMetaType>

namespace kPF5{

enum Commands
{
    CMD_DIAGNOSTICS     =   0x47,
    CMD_HEADER          =   0x2b,
    CMD_CHECK_OPEN      =   0x30,
    CMD_CHECK_REG       =   0x31,
    CMD_CHECK_TOTAL     =   0x35,
    CMD_CHECK_CLOSE     =   0x38,
    CMD_SET_DATETIME    =   0x3D,
    CMD_DAILY_REPORT    =   0x45,
    CMD_SERVICE_MONEY   =   0x46,
    CMD_STORE_OPEN      =   0x55,
    CMD_STORE_CLOSE     =   0x56,
    CMD_PERIOD_REPORT   =   0x5E,
    CMD_REPORT_NUMBER   =   0x49
};

enum TaxGroup
{
    TAX_A = 0xC0,
    TAX_B = 0xC1,
    TAX_C = 0xC2,
    TAX_D = 0xC3
};

enum PeriodicalReportType
{
    PERIODICALREPORT_SUM_BY_TAX_GROUP               =   0x34,
    PERIODICALREPORT_NET_BY_TAX_GROUP               =   0x35,
    PERIODICALREPORT_ACCUMULATED_TAXES              =   0x41,
    PERIODICALREPORT_STORED_SUM_BY_TAX_GROUP        =   0x42,
    PERIODICALREPORT_STORED_NET_BY_TAX_GROUP        =   0x43,
    PERIODICALREPORT_STORED_ACCUMULATED_TAXES       =   0x44
};

struct CheckEntry
{
    //CheckEntry(){}
    CheckEntry(const QString& NewProductCode = "", const QString& NewProduct = "", const double& NewQuantity = 1, const double& NewPrice = 0, const TaxGroup& NewTax = TAX_A,
               const double& NewPurchasePrice = 0)
            :ProductCode(NewProductCode)\
            ,Product(NewProduct)
            ,Quantity(NewQuantity)
            ,Price(NewPrice)
            ,Tax(NewTax)
            ,PurchasePrice(NewPurchasePrice)
    {}
    QString ProductCode;
    QString Product;
    double Quantity;
    double Price;
    TaxGroup Tax;
    double PurchasePrice;
    double Amount(){ return Quantity*Price; }
};
}
#define CHECK_ENTRY_COLUMN_COUNT 7

Q_DECLARE_METATYPE(kPF5::CheckEntry)        
#endif // KPF5_DATA_H
