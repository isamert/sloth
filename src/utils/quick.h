#ifndef QUICK_H
#define QUICK_H

#include <QtWidgets>

class Quick
{

private:
    Quick() { }

public:
    static void msgInfo(const QString &title, const QString &message);
    static void msgWarning(const QString &title, const QString &message);
    static void msgError(const QString &title, const QString &message);
    static bool msgYesNo(const QString &title, const QString &message);
    static QString getText(const QString &title, const QString &message, bool *ok, const QString &text = "");
    static QIcon getIcon(const QString &name);
};

#endif // QUICK_H
