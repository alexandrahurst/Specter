#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "spectrogramwidget.h"

SettingsDialog::SettingsDialog(SpectrogramWidget *spectrogram, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);


    QStringList devices{};
    for(QAudioDeviceInfo device : QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        devices.push_back(device.deviceName());
    }
    ui->audioInputComboBox->addItems(devices);



    connect(ui->minFrequencySpinBox, SIGNAL(valueChanged(int)), spectrogram, SLOT(setMinFrequency(int)));
    connect(ui->maxFrequencySpinBox, SIGNAL(valueChanged(int)), spectrogram, SLOT(setMaxFrequency(int)));
    connect(ui->audioInputComboBox, SIGNAL(currentIndexChanged(QString)), spectrogram, SLOT(setInput(QString)));
    connect(ui->displayModeComboBox, SIGNAL(currentIndexChanged(int)), spectrogram, SLOT(setDisplayMode(int)));
    connect(ui->minDecibelsSpinBox, SIGNAL(valueChanged(int)), spectrogram, SLOT(setMinDecibels(int)));
    connect(ui->maxDecibelsSpinBox, SIGNAL(valueChanged(int)), spectrogram, SLOT(setMaxDecibels(int)));
    connect(ui->windowFunctionComboBox, SIGNAL(currentIndexChanged(int)), spectrogram, SLOT(setWindowingFunction(int)));



    connect(ui->maxDecibelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setMinDecibelsSpinBoxMaximum(int)));
    connect(ui->minDecibelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setMaxDecibelsSpinBoxMinimum(int)));
    setMinDecibelsSpinBoxMaximum(ui->maxDecibelsSpinBox->value());
    setMaxDecibelsSpinBoxMinimum(ui->minDecibelsSpinBox->value());

    connect(ui->maxFrequencySpinBox, SIGNAL(valueChanged(int)), this, SLOT(setMinFrequencySpinBoxMaximum(int)));
    connect(ui->minFrequencySpinBox, SIGNAL(valueChanged(int)), this, SLOT(setMaxFrequencySpinBoxMinimum(int)));
    setMinFrequencySpinBoxMaximum(ui->maxFrequencySpinBox->value());
    setMaxFrequencySpinBoxMinimum(ui->minFrequencySpinBox->value());

    connect(ui->displayModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDisplayMode(int)));
    setDisplayMode(SpectrogramWidget::NORMALIZED);

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

Ui::SettingsDialog *SettingsDialog::getUi() {
    return ui;
}

void SettingsDialog::toggleVisible() {
    if(this->isVisible()) {
        this->hide();
    } else {
        this->show();
    }
}

void SettingsDialog::setMaxDecibelsSpinBoxMinimum(int freq) {
    ui->maxDecibelsSpinBox->setMinimum(freq + 1);
}

void SettingsDialog::setMinDecibelsSpinBoxMaximum(int freq) {
    ui->minDecibelsSpinBox->setMaximum(freq - 1);
}

void SettingsDialog::setMaxFrequencySpinBoxMinimum(int freq) {
    ui->maxFrequencySpinBox->setMinimum(freq + 1);
}

void SettingsDialog::setMinFrequencySpinBoxMaximum(int freq) {
    ui->minFrequencySpinBox->setMaximum(freq - 1);
}

void SettingsDialog::setDisplayMode(int mode) {
    if(mode == SpectrogramWidget::NORMALIZED) {
        ui->minDecibelsSpinBox->setEnabled(false);
        ui->maxDecibelsSpinBox->setEnabled(false);
    } else {    // standard
        ui->minDecibelsSpinBox->setEnabled(true);
        ui->maxDecibelsSpinBox->setEnabled(true);
    }
}
