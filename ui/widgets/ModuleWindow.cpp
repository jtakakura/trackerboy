
#include "widgets/ModuleWindow.hpp"

#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>

const char* ModuleWindow::MODULE_FILE_FILTER = QT_TR_NOOP("Trackerboy module (*.tbm)");


ModuleWindow::ModuleWindow(Trackerboy &app, ModuleDocument *doc, QWidget *parent) :
    QWidget(parent),
    mApp(app),
    mDocument(doc),
    mLayout(),
    mTabs(),
    mTabSettings(*doc),
    mTabPatterns(app.config.keyboard().pianoInput, *doc),
    mTabInstruments(*doc),
    mTabWaveforms(*doc)
{

    mTabs.addTab(&mTabSettings, tr("General settings"));
    mTabs.addTab(&mTabPatterns, tr("Patterns"));
    mTabs.addTab(&mTabInstruments, tr("Instruments"));
    mTabs.addTab(&mTabWaveforms, tr("Waveforms"));


    mLayout.setMargin(0);
    mLayout.addWidget(&mTabs);
    setLayout(&mLayout);




    updateWindowTitle();
    connect(doc, &ModuleDocument::modifiedChanged, this, &QWidget::setWindowModified);
}

void ModuleWindow::applyConfiguration(Config::Categories categories) {
    if (categories.testFlag(Config::CategoryAppearance)) {
        auto &appearance = mApp.config.appearance();
        mTabPatterns.setColors(appearance.colors);
        mDocument->orderModel().setRowColor(appearance.colors[+Color::row]);
    }
}

ModuleDocument* ModuleWindow::document() noexcept {
    return mDocument;
}

bool ModuleWindow::save() {
    if (mDocument->hasFile()) {
        commit();
        return mDocument->save();
    } else {
        return saveAs();
    }
}

bool ModuleWindow::saveAs() {

    auto path = QFileDialog::getSaveFileName(
        this,
        tr("Save module"),
        "",
        tr(MODULE_FILE_FILTER)
        );

    if (path.isEmpty()) {
        return false;
    }

    commit();
    auto result = mDocument->save(path);
    if (result) {
        updateWindowTitle();
    }
    return result;
}

void ModuleWindow::closeEvent(QCloseEvent *evt) {
    if (maybeSave()) {
        emit documentClosed(mDocument);
        evt->accept();
    } else {
        evt->ignore();
    }

    
}

bool ModuleWindow::maybeSave() {

    if (!mDocument->isModified()) {
        return true;
    }

    auto const result = QMessageBox::warning(
        this,
        tr("Trackerboy"),
        tr("Save changes to %1?").arg(mDocument->name()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    switch (result) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }

    return true;
}

void ModuleWindow::commit() {
    mTabSettings.commit();
}

void ModuleWindow::updateWindowTitle() {
    setWindowTitle(QStringLiteral("%1[*]").arg(mDocument->name()));
}
