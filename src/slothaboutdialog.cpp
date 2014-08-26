#include "slothaboutdialog.h"
#include "ui_slothaboutdialog.h"

SlothAboutDialog::SlothAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlothAboutDialog)
{
    ui->setupUi(this);
}

SlothAboutDialog::~SlothAboutDialog()
{
    delete ui;
}
