#include "spectrogramwidget.h"

#include <iostream>
#include <cmath>
#include <imageutil.h>

SpectrogramWidget::SpectrogramWidget(QWidget *parent) : QWidget(parent),
    spectrogramDisplayImage(new QImage(2000, 1200, QImage::Format_RGB32)),
    painter(new QPainter(spectrogramDisplayImage)),

    displayMode(NORMALIZED),
    maxDecibels(80),
    minDecibels(-80),
    maxFrequency(1000),
    minFrequency(0),
    windowingFunction(HANNING),
    inputAudioChannel(MIX),
    showFrequencyMarker(true),
    frequencyMarkerFrequency(250),

    recording(false),

    sampleCount(0),
    sampleBuffer(),

    data(),
    fft(),
    magnitude(),

    fftPlan(fftw_plan_dft_1d(SAMPLES_PER_ROW, fft.data(), fft.data(), FFTW_FORWARD, FFTW_MEASURE))
{

    // set initial spectrogram background to black
    Color background;
    background.r = background.g = background.b = 0;
    background.a = 255;
    spectrogramDisplayImage->fill(background.rgba);

    // initialize audio
    QAudioFormat format{};
    format.setCodec("audio/PCM");
    format.setSampleRate(96000);
    format.setSampleSize(32);
    format.setSampleType(QAudioFormat::SignedInt);

    format = QAudioDeviceInfo::defaultInputDevice().nearestFormat(format);

    std::cout << format.sampleRate() << " " << format.sampleSize() << " " << format.sampleType() << std::endl;


    input = new QAudioInput(QAudioDeviceInfo::defaultInputDevice(), format, this);

}


void SpectrogramWidget::setDisplayMode(int mode) {
    this->displayMode = (DisplayMode) mode;
}

void SpectrogramWidget::setMinDecibels(int decibels) {
    this->minDecibels = decibels;
}

void SpectrogramWidget::setMaxDecibels(int decibels) {
    this->maxDecibels = decibels;
}

void SpectrogramWidget::setMinFrequency(int frequency) {
    this->minFrequency = frequency;
}

void SpectrogramWidget::setMaxFrequency(int frequency) {
    this->maxFrequency = frequency;
}

void SpectrogramWidget::setWindowingFunction(int function) {
    this->windowingFunction = (WindowingFunction) function;
}

void SpectrogramWidget::setInputAudioChannel(int channel) {
    this->inputAudioChannel = (AudioChannel) channel;
}

void SpectrogramWidget::setShowFrequencyMarker(bool show) {
    this->showFrequencyMarker = show;
}

void SpectrogramWidget::setFrequencyMarkerFrequency(int frequency) {
    this->frequencyMarkerFrequency = frequency;
}

void SpectrogramWidget::setInput(const QString &deviceName) {
    bool wasRecording = recording;
    if(wasRecording) {
        setRecording(false);
    }

    input->disconnect(inputDevice);
    QAudioFormat format(input->format());
    delete input;

    for(QAudioDeviceInfo device : QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if(device.deviceName() == deviceName) {

            input = new QAudioInput(device, format, this);
            break;
        }
    }

    if(wasRecording) {
        setRecording(true);
    }
}

double SpectrogramWidget::windowFunction(int n, int N) {
    switch(windowingFunction) {
    case HANNING:           return 0.5 * (1 - cos((2 * M_PI * n) / (N - 1)));
    case BLACKMAN:          return 0.42659 - 0.49656 * cos((2 * M_PI * n) / (N - 1)) + 0.076849 * cos((4 * M_PI * n) / (N - 1));
    case BLACKMAN_HARRIS:   return 0.35875 - 0.48829 * cos((2 * M_PI * n) / (N - 1)) + 0.14128 * cos((4 * M_PI * n) / (N - 1)) - 0.01168 * cos((6 * M_PI * n) / (N - 1));
    default:                return 1;      // rectangular
    }
}

int SpectrogramWidget::indexToFreq(int index, int Fs, int N) {
    return (int) (index * ((double) Fs * 2) / N);
}

int SpectrogramWidget::freqToIndex(int frequency, int Fs, int N) {
    return (int) (frequency * N / ((double) Fs * 2));
}

double SpectrogramWidget::normalize(double value, double minValue) {
    return value - minValue;
}

void SpectrogramWidget::setRecording(bool recording) {
    if(recording != this->recording) {
        this->recording = recording;
        if(recording) {
            inputDevice = input->start();
            std::cout << inputDevice << std::endl;
            connect(inputDevice, SIGNAL(readyRead()), this, SLOT(analyzeAudio()));
        } else {
            disconnect(inputDevice, SIGNAL(readyRead()), this, SLOT(analyzeAudio()));
            inputDevice->close();
            input->stop();
        }
    }
}

void SpectrogramWidget::analyzeAudio() {
    QAudioBuffer buffer = QAudioBuffer(inputDevice->readAll(), input->format());

    int N = buffer.sampleCount();

    int processedSamples = 0;
    while(processedSamples < N) {
        if(sampleCount + N < SAMPLES_PER_ROW) { // not enough samples to analyze, so just store samples for later analysis
            for(int i = 0; i < N; i++) {
                float sample = (float) buffer.constData<qint32>()[i];
                sample /= pow(2, 32 - 1);
                sampleBuffer[i + sampleCount] = sample;
                processedSamples++;
            }
            sampleCount += N;
        } else {    // enough samples; begin analysis
            for(int i = 0; i < sampleCount; i++) {
                data[i] = sampleBuffer[i] * windowFunction(i + sampleCount, SAMPLES_PER_ROW);
            }
            for(int i = 0; i + sampleCount < SAMPLES_PER_ROW; i++) {
                float sample = (float) buffer.constData<qint32>()[i];
                sample /= pow(2, 32 - 1);
                data[i + sampleCount] = sample * windowFunction(i + sampleCount, SAMPLES_PER_ROW);
                processedSamples++;
            }
            sampleCount = 0;

            // begin analysis

            // properly format sample data for fft
            for(int i = 0; i < SAMPLES_PER_ROW; i++) {
                fft[i][0] = data[i];
                fft[i][1] = 0;
            }

            // run fft
            fftw_execute(fftPlan);

            // calculate power spectrum (magnitude) values from fft
            // NOTE: values are in dB
            for(int i = 0; i < SAMPLES_PER_ROW / 2; i++) {
                double re = fft[i][0];
                double im = fft[i][1];
                magnitude[i] = 20 * log10(re * re + im * im);
                //magnitude[i] = sqrt(re * re + im * im);
                //magnitude[i] = re * re + im * im;
            }

            // redraw spectrogram, appending just-analyzed row at the bottom
            drawSpectrogram(buffer);
        }
    }
}

void SpectrogramWidget::drawSpectrogram(const QAudioBuffer& buffer) {

    // shift already-drawn spectrogram image up by one row to make room for new row
    // (i.e. take the previously-drawn rows and push them somewhere else)
    ImageUtil::shiftUp(*(this->spectrogramDisplayImage));


    // draw spectrogram
    int Fs = buffer.format().sampleRate();

    // find max and min values in power spectrum
    double maxMagnitude = -INFINITY;
    double minMagnitude = INFINITY;
    for(int i = 0; i < SAMPLES_PER_ROW / 2; i++) {
        if(magnitude[i] > maxMagnitude){
            maxMagnitude = magnitude[i];
        }
        if(magnitude[i] < minMagnitude){
            minMagnitude = magnitude[i];
        }
    }

    // calculate number for discrete frequencies to show
    int firstShownBin = freqToIndex(minFrequency, Fs, SAMPLES_PER_ROW);
    int lastShownBin = freqToIndex(maxFrequency, Fs, SAMPLES_PER_ROW);

    // draw current row of spectrogram
    for(int x = 0; x < width(); x++){
        int index = (int) (x / (float) width() * (lastShownBin - firstShownBin) + firstShownBin) * 2;   // multiplied by to to fix x-axis scaling (otherwise,

        double percentageIntensity;

        if(displayMode == NORMALIZED) {
            if(normalize(maxMagnitude, minMagnitude) != 0 &&
                    minMagnitude != -INFINITY &&
                    minMagnitude != INFINITY &&
                    maxMagnitude != -INFINITY &&
                    maxMagnitude != INFINITY) {
                percentageIntensity = normalize(magnitude[index], minMagnitude) / normalize(maxMagnitude, minMagnitude);
            } else {
                percentageIntensity = 0;
            }
        } else {    // standard
            if(normalize(maxDecibels, minDecibels) != 0 &&
                    minMagnitude != -INFINITY &&
                    minMagnitude != INFINITY &&
                    maxMagnitude != -INFINITY &&
                    maxMagnitude != INFINITY) {
                percentageIntensity = normalize(magnitude[index], minDecibels) / normalize(maxDecibels, minDecibels);
                if(percentageIntensity < 0) {
                    percentageIntensity = 0;
                } else if(percentageIntensity > 1) {
                    percentageIntensity = 1;
                }
            } else {
                percentageIntensity = 0;
            }
        }

        Color color;

        color.r = color.g = color.b = (uint8_t) (percentageIntensity * 255);
        color.a = 255;

        spectrogramDisplayImage->setPixel(x, spectrogramDisplayImage->height() - INFO_AREA_HEIGHT - 1, color.rgba);
    }

    // blur current row of spectrogram
    //ImageUtil::boxBlurRow(*(this->spectrogramDisplayImage), spectrogramDisplayImage->height() - INFO_AREA_HEIGHT - 1);


    // clear information area
    painter->fillRect(0, spectrogramDisplayImage->height() - INFO_AREA_HEIGHT, spectrogramDisplayImage->width(), INFO_AREA_HEIGHT, Qt::black);
    painter->setPen(Qt::red);
    for(int i = 0; i <= 10; i++) {
        painter->drawLine(i * width() / 10, spectrogramDisplayImage->height() - INFO_AREA_HEIGHT, i * width() / 10, spectrogramDisplayImage->height() - (INFO_AREA_HEIGHT / 2));
        painter->drawText(i * width() / 10 + 5, spectrogramDisplayImage->height() - INFO_AREA_HEIGHT + 15, QString::number(minFrequency + i * (maxFrequency - minFrequency) / 10) + " Hz");
    }

    update();   // causes widget to be redrawn with the updated spectrogramDisplayImage
}

SpectrogramWidget::~SpectrogramWidget() {

    setRecording(false);
    delete input;

    fftw_destroy_plan(fftPlan);

    delete painter;
    delete spectrogramDisplayImage;


}

void SpectrogramWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.drawPixmap(0, 0, width(), height(), QPixmap::fromImage(*spectrogramDisplayImage).copy(0, spectrogramDisplayImage->height() - height() , width(), height()));
    p.end();
}
