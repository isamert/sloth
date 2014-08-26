#ifndef SLOTHABOUTDIALOG_H
#define SLOTHABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class SlothAboutDialog;
}

class SlothAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SlothAboutDialog(QWidget *parent = 0);
    ~SlothAboutDialog();

private:
    Ui::SlothAboutDialog *ui;
};

#endif // SLOTHABOUTDIALOG_H
