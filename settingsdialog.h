#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "spectrogramwidget.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(SpectrogramWidget *spectrogram, QWidget *parent = 0);
    ~SettingsDialog();
    Ui::SettingsDialog *getUi();

public slots:
    void toggleVisible();

private:
    Ui::SettingsDialog *ui;

private slots:
    void setMaxDecibelsSpinBoxMinimum(int);
    void setMinDecibelsSpinBoxMaximum(int);
    void setMaxFrequencySpinBoxMinimum(int);
    void setMinFrequencySpinBoxMaximum(int);
    void setDisplayMode(int);
};

#endif // SETTINGSDIALOG_H
