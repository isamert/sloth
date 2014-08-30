#include "qfilecopier/slothcopier.h"

SlothCopier::SlothCopier(QObject *parent) :
    QObject(parent)
{
    copier = new QFileCopier(this);
    connect(copier, SIGNAL(stateChanged(QFileCopier::State)), SLOT(onStateChanged(QFileCopier::State)));
    connect(copier, SIGNAL(started(int)), SLOT(onStarted(int)));
    connect(copier, SIGNAL(finished(int,bool)), SLOT(onFinished(int)));
    connect(copier, SIGNAL(progress(qint64,qint64)), SLOT(onProgress(qint64,qint64)));
    connect(copier, SIGNAL(error(int, QFileCopier::Error,bool)), SLOT(onError(int, QFileCopier::Error,bool)));
}

void SlothCopier::onStateChanged(QFileCopier::State s)
{
    Q_ASSERT_X(QThread::currentThread() == qApp->thread(), "NotificationTest::onStateChanged", "slot invoked from wrong thread");
    qDebug() << "State changed" << s;

    if (s == QFileCopier::Idle) {
        qDebug() << "idle";
    }
}

void SlothCopier::onStarted(int id)
{
    Q_ASSERT_X(QThread::currentThread() == qApp->thread(), "SlothCopier::onStarted", "slot invoked from wrong thread");
    qDebug() << "    Started request with id" << id;
             //<< "(" << copier->sourceFilePath(id).mid(tempFolder.length()+1) << "->"
             //<< copier->destinationFilePath(id).mid(tempFolder.length()+1) << ")";
}

void SlothCopier::onFinished(int id)
{
    Q_ASSERT_X(QThread::currentThread() == qApp->thread(), "SlothCopier::onFinished", "slot invoked from wrong thread");
    qDebug() << "    Finished request with id" << id;
}

void SlothCopier::onProgress(qint64 w, qint64 s)
{
    Q_ASSERT_X(QThread::currentThread() == qApp->thread(), "SlothCopier::onProgress", "slot invoked from wrong thread");
    qDebug() << QString("      Progress %1 / %2 (%3)").arg(w/1024).arg(s/1024).arg(s == 0 ? 0 : 100*w/s).toLatin1().data();
    qDebug() << QString("      Total progress %1 / %2 (%3)").arg(copier->totalProgress()/1024).arg(copier->totalSize()/1024).arg(100*copier->totalProgress()/copier->totalSize()).toLatin1().data();
}

void SlothCopier::onError(int /*id*/, QFileCopier::Error error, bool stopped)
{
    Q_ASSERT_X(QThread::currentThread() == qApp->thread(), "SlothCopier::onProgress", "slot invoked from wrong thread");
    qDebug() << "      Error occured for request" << copier->currentId() << error << "stopped ="<< stopped;

    if (error == QFileCopier::DestinationExists)
        copier->overwrite();
    else
        copier->skip();
}
