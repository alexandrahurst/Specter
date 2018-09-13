#ifndef SPECTROGRAMWIDGET_H
#define SPECTROGRAMWIDGET_H

#include <QAudioBuffer>
#include <QGraphicsView>
#include <QWidget>
#include <QAudioInput>
#include <QAudioFormat>
#include <QPainter>
#include <array>
#include <ccomplex>
#include <fftw3.h>


class SpectrogramWidget : public QWidget {
    Q_OBJECT
public:
    explicit SpectrogramWidget(QWidget *parent = 0);
    ~SpectrogramWidget();

    enum DisplayMode {
        NORMALIZED = 0,
        STANDARD = 1
    };
    enum AudioChannel {
        LEFT,
        RIGHT,
        MIX
    };
    enum WindowingFunction {
        HANNING = 0,
        BLACKMAN = 1,
        BLACKMAN_HARRIS = 2,
        RECTANGULAR = 3
    };

    union Color {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        uint32_t rgba;
    };

signals:

public slots:
    void setDisplayMode(int);
    void setMinDecibels(int);
    void setMaxDecibels(int);
    void setMinFrequency(int);
    void setMaxFrequency(int);
    void setWindowingFunction(int);
    void setInputAudioChannel(int);
    void setShowFrequencyMarker(bool);
    void setFrequencyMarkerFrequency(int);
    void setInput(const QString &);

    void setRecording(bool);

    void analyzeAudio();

private:
    QAudioInput *input;
    QIODevice *inputDevice;

    QImage *spectrogramDisplayImage;
    QPainter *painter;

    DisplayMode displayMode;
    int maxDecibels;
    int minDecibels;
    int maxFrequency;
    int minFrequency;
    WindowingFunction windowingFunction;
    AudioChannel inputAudioChannel;
    bool showFrequencyMarker;
    int frequencyMarkerFrequency;

    bool recording;

    double windowFunction(int, int);

    int indexToFreq(int, int, int);
    int freqToIndex(int, int, int);
    double normalize(double, double);
    //void blurRow(QImage &image, int row);
    void drawSpectrogram(const QAudioBuffer&);

    void paintEvent(QPaintEvent *);

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int SAMPLES_PER_ROW = 16384;   // must be a power of 2
    static constexpr int INFO_AREA_HEIGHT = 20;

    int sampleCount;
    std::array<double, SAMPLES_PER_ROW> sampleBuffer;

    std::array<double, SAMPLES_PER_ROW> data;
    std::array<fftw_complex, SAMPLES_PER_ROW> fft;
    std::array<double, SAMPLES_PER_ROW / 2> magnitude;

    fftw_plan fftPlan;
};

#endif // SPECTROGRAMWIDGET_H
