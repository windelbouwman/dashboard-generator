
// Test app with Qwt

#include <iostream>

#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

// #include <Qwt/SeriesData>

#include <QtMqtt/QtMqtt>

class DashBoard : public QWidget
{
    public:
        DashBoard();
        ~DashBoard();

    private:
        QMqttClient* m_client;
        QMqttSubscription* m_sub;

        QLabel* m_label;
        QwtPlot* m_plt;
        QwtPlotCurve* m_curve1;
        QwtPlotCurve* m_curve2;

        void mqttStateChange();
        void on_message(const QMqttMessage &msg);

};

DashBoard::DashBoard()
  : QWidget()
{
    this->m_client = new QMqttClient(this);
    m_client->setHostname("localhost");
    m_client->setPort(1883);
    m_client->connectToHost();

    QVBoxLayout* l = new QVBoxLayout();
    this->setLayout(l);

    m_plt = new QwtPlot();
    m_curve1 = new QwtPlotCurve("Curve 1");
    m_curve2 = new QwtPlotCurve("Curve 2");

    m_label = new QLabel(this);
    m_label->setText("HJHJKFD");

    m_curve1->attach(m_plt);
    m_curve2->attach(m_plt);
    m_plt->replot();
    
    l->addWidget(m_plt);
    l->addWidget(m_label);

    connect(
        m_client,
        &QMqttClient::stateChanged,
        this,
        &DashBoard::mqttStateChange
    );
}

DashBoard::~DashBoard()
{
    delete m_client;
}

void DashBoard::mqttStateChange()
{
    std::cout << "State changed! = " << m_client->state() << std::endl;

    if (m_client->state() == QMqttClient::ClientState::Connected)
    {
        std::cout << "Connected!" << std::endl;

        QString topic = QString("/a/b");
        m_sub = m_client->subscribe(topic);
        if (m_sub) {
            std::cout << "Subscribed to! " << topic.toStdString() << std::endl;
            connect(
                m_sub,
                &QMqttSubscription::messageReceived,
                this,
                &DashBoard::on_message
            );
        } else {
            std::cout << "AARG, subscribe error: ptr = " <<  std::endl;
        }
    }
}

void DashBoard::on_message(const QMqttMessage &msg)
{
    QString txt(msg.payload());
    // std::cout << "GOt message: " << txt.toStdString() << std::endl;
    // std::cout << "MSG!" << msg.payload() << endl;
    m_label->setText(txt);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    DashBoard* dashboard = new DashBoard();

    // QVector<double>* points = new QVector<double>();
    // QwtArraySeriesData<double>* data = new QwtArraySeriesData<double>(points);

    // curve1->setData(data);

    dashboard->show();

    app.exec();
    delete dashboard;
}
