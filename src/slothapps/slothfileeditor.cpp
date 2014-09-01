#include "slothapps/slothfileeditor.h"

SlothFileEditor::SlothFileEditor(QWidget *parent, const QString &filePath /* = "" */) :
    QWidget(parent)
{
    //TODO: add find/replace bar to bottom
    this->loadUi();

    if(!filePath.isEmpty())
        this->setFile(filePath);
}

void SlothFileEditor::loadUi() {
    this->vlayout = new QVBoxLayout(this);
    this->toolbar = new QToolBar(this);
    this->textEdit = new QPlainTextEdit(this);

    this->vlayout->setSpacing(0);
    this->vlayout->setMargin(0);

    this->vlayout->addWidget(this->toolbar);
    this->vlayout->addWidget(this->textEdit);

    //toolbar
    this->actSave = new QAction(Quick::getIcon("document-save"), trUtf8("Save"), this->toolbar);
    this->actCut = new QAction(Quick::getIcon("edit-cut"), trUtf8("Cut"), this->toolbar);
    this->actCopy = new QAction(Quick::getIcon("edit-copy"), trUtf8("Copy"), this->toolbar);
    this->actPaste = new QAction(Quick::getIcon("edit-paste"), trUtf8("Paste"), this->toolbar);
    this->actUndo = new QAction(Quick::getIcon("edit-undo"), trUtf8("Undo"), this->toolbar);
    this->actRedo = new QAction(Quick::getIcon("edit-redo"), trUtf8("Redo"), this->toolbar);

    this->toolbar->addAction(this->actSave);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->actCut);
    this->toolbar->addAction(this->actCopy);
    this->toolbar->addAction(this->actPaste);
    this->toolbar->addSeparator();
    this->toolbar->addAction(this->actUndo);
    this->toolbar->addAction(this->actRedo);

    this->actCut->setEnabled(false);
    this->actCopy->setEnabled(false);
    this->actSave->setEnabled(textEdit->document()->isModified());
    this->actUndo->setEnabled(textEdit->document()->isUndoAvailable());
    this->actRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(textEdit->document(), SIGNAL(modificationChanged(bool)), this->actSave, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(modificationChanged(bool)),this, SLOT(setWindowModified(bool)));
    connect(textEdit->document(), SIGNAL(undoAvailable(bool)), this->actUndo, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(redoAvailable(bool)), this->actRedo, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), this->actCut, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), this->actCopy, SLOT(setEnabled(bool)));

    connect(this->actUndo, SIGNAL(triggered()), textEdit, SLOT(undo()));
    connect(this->actRedo, SIGNAL(triggered()), textEdit, SLOT(redo()));
    connect(this->actCut, SIGNAL(triggered()), textEdit, SLOT(cut()));
    connect(this->actCopy, SIGNAL(triggered()), textEdit, SLOT(copy()));
    connect(this->actPaste, SIGNAL(triggered()), textEdit, SLOT(paste()));
    connect(this->actSave, SIGNAL(triggered()), this, SLOT(saveFile()));
}

bool SlothFileEditor::setFile(const QString &filePath) {
    this->file = filePath;

    if (!QFile::exists(filePath))
        return false;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QString str = QString::fromLocal8Bit(data);
    this->textEdit->setPlainText(str);

    return true;
}

bool SlothFileEditor::saveFile() {
    QFile sfile(this->file);
    if (!sfile.open(QFile::WriteOnly | QFile::Text))
        return false;

    QTextStream out(&sfile);
    out << this->textEdit->toPlainText();
    sfile.close();

    Quick::msgInfo(trUtf8("Save"), trUtf8("File saved successfully."));
    this->textEdit->document()->setModified(false);

    return true;
}

QString SlothFileEditor::getCurrentFile() {
    return this->file;
}

QStringList SlothFileEditor::getSupportedMimeTypes() {
    QStringList list;
    list << "application/x-php" << "application/x-shellscript" << "application/x-zerosize"
         << "application/x-desktop";
    return list;
}
