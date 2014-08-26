#include "utils/quick.h"

void Quick::msgInfo(const QString &title, const QString &message) {
    QMessageBox::information(qApp->activeWindow(), title, message);
}

void Quick::msgWarning(const QString &title, const QString &message) {
    QMessageBox::warning(qApp->activeWindow(), title, message);
}

void Quick::msgError(const QString &title, const QString &message) {
    QMessageBox::critical(qApp->activeWindow(), title, message);
}

bool Quick::msgYesNo(const QString &title, const QString &message) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(qApp->activeWindow(), title, message, QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
        return true;
    else
        return false;
}

QString Quick::getText(const QString &title, const QString &message, bool *ok, const QString &text /* = "" */) {
    QString str = QInputDialog::getText(qApp->activeWindow(), title, message, QLineEdit::Normal, text, ok);
    return str;
}


QIcon Quick::getIcon(const QString &name) {
    if(QIcon::hasThemeIcon(name))
        return QIcon::fromTheme(name);
    else
        return QIcon(QString(":/images/%1.png").arg(name));
}
