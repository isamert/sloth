#include "slothwidgets/slothminitermwidget.h"

SlothMinitermWidget::SlothMinitermWidget(QWidget *parent) :
    QToolBar(parent)
{
    this->process = new QProcess(this);
    connect(this->process, SIGNAL(readyRead()), this, SLOT(printOutput()));

    this->workingDirectory = "";
    this->currentHistoryIndex = 0;

    this->setWindowTitle(trUtf8("Mini Terminal"));

    this->lineMiniterm = new QLineEdit(this);
    this->lineMiniterm->setPlaceholderText(trUtf8("")); //TODO:
    this->lineMiniterm->installEventFilter(this);
    this->addWidget(this->lineMiniterm);

    this->txtOutput = new QPlainTextEdit;
    this->txtOutput->setWindowTitle(trUtf8("Output:"));
    this->txtOutput->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->txtOutput->setAttribute(Qt::WA_ShowWithoutActivating);
    this->txtOutput->setFocusPolicy(Qt::NoFocus);
    this->txtOutput->setFocusProxy(this->lineMiniterm);
    this->txtOutput->installEventFilter(this);

    this->addAction(Quick::getIcon("document-properties"), trUtf8("Show output"), this, SLOT(showOutput()));
    this->addAction(Quick::getIcon("exit"), trUtf8("Close"), this, SLOT(showOrHide()));

    connect(this->lineMiniterm, SIGNAL(returnPressed()), this, SLOT(executeCommand()));
}

SlothMinitermWidget::~SlothMinitermWidget()
{
    delete this->txtOutput;
}

bool SlothMinitermWidget::eventFilter(QObject *obj, QEvent *event) {
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(obj == this->lineMiniterm) {
            if(keyEvent->key() == Qt::Key_Escape) {
                this->showOrHide();
                return true;
            }

            if(!this->commandHistory.isEmpty()) {
                if(keyEvent->key() == Qt::Key_Up) {
                    int historyIndex = this->currentHistoryIndex - 1;

                    if(this->commandHistory.count() > historyIndex && historyIndex > -1) {
                        this->lineMiniterm->setText(this->commandHistory[historyIndex]);
                        this->currentHistoryIndex = historyIndex;
                        return true;
                    }
                }
                else if(keyEvent->key() == Qt::Key_Down) {
                    int historyIndex = this->currentHistoryIndex + 1;

                    if(this->commandHistory.count() > historyIndex && historyIndex > -1) {
                        this->lineMiniterm->setText(this->commandHistory[historyIndex]);
                        this->currentHistoryIndex = historyIndex;
                        return true;
                    }
                }
            }
        }
        else if(obj == this->txtOutput) {
            if(keyEvent->key() == Qt::Key_Escape) {
                this->txtOutput->hide();
                return true;
            }
        }
    }
    return QToolBar::eventFilter(obj, event);
}

void SlothMinitermWidget::setWorkingDirectory(const QString &dir) {
    this->workingDirectory = dir;
}

void SlothMinitermWidget::executeCommand() {
    QStringList args = this->lineMiniterm->text().split(' ');
    QString program = args.takeAt(0);

    if(program == "cd") {
        QString path = FileUtils::combine(this->workingDirectory,
                                          this->lineMiniterm->text().remove("cd").trimmed());
        if(QFile::exists(path)) {
            emit this->openDirRequested(path);
            this->lineMiniterm->clear();
            return;
        }
    }
    else if(program == "clear") {
        this->txtOutput->clear();
    }
    else {
        this->process->setWorkingDirectory(this->workingDirectory);
        this->process->start(program, args);
        //QProcess::startDetached(program, args, this->workingDirectory);
    }

    this->commandHistory.append(this->lineMiniterm->text());
    this->currentHistoryIndex = this->commandHistory.count();
    this->lineMiniterm->clear();
}

void SlothMinitermWidget::showOrHide() {
    this->setVisible(!this->isVisible());

    if(this->isVisible())
        this->lineMiniterm->setFocus();
    else
        this->txtOutput->hide();
}

void SlothMinitermWidget::printOutput() {
    this->txtOutput->setPlainText(this->process->readAll());
}

void SlothMinitermWidget::showOutput() {
    if(this->txtOutput->isVisible()) {
        this->txtOutput->hide();
    }
    else {
        this->txtOutput->resize(this->lineMiniterm->width(), 150);

        QPoint p;
        p.setX(this->mapToGlobal(this->lineMiniterm->pos()).x());
        p.setY(this->mapToGlobal(this->lineMiniterm->pos()).y() + this->lineMiniterm->height());

        this->txtOutput->move(p);
        this->txtOutput->show();

        this->lineMiniterm->setFocus();
    }
}
