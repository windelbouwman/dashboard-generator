
// Generated at {{ date }}

#include <iostream>

#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

#include <QtMqtt/QtMqtt>

class DashBoard : public QWidget
{
    public:
        DashBoard();
        ~DashBoard();

    private:
        QMqttClient* m_client;
        
        {% for topic in topics %}
        QMqttSubscription* m_subscription_{{ topic.id }};
        {% endfor %}

        {% for label in labels %}
        QLabel* m_label_title_{{ label.id }};
        QLabel* m_label_value_{{ label.id }};
        {% endfor %}

        QwtPlot* m_plt;
        QwtPlotCurve* m_curve1;
        QwtPlotCurve* m_curve2;

        void mqttStateChange();
        void subscribeToAllTopics();

        {% for topic in topics %}
        void on_message_{{ topic.id }}(const QMqttMessage &msg);
        {% endfor %}
};

DashBoard::DashBoard()
  : QWidget()
{
    this->m_client = new QMqttClient(this);
    m_client->setHostname("{{ host }}");
    m_client->setPort({{ port }});
    m_client->connectToHost();

    QVBoxLayout* l = new QVBoxLayout();
    this->setLayout(l);

    m_plt = new QwtPlot();
    m_curve1 = new QwtPlotCurve("Curve 1");
    m_curve2 = new QwtPlotCurve("Curve 2");

    m_curve1->attach(m_plt);
    m_curve2->attach(m_plt);
    m_plt->replot();
    
    l->addWidget(m_plt);

    // Add some labels:
    {% for label in labels %}
    m_label_title_{{ label.id }} = new QLabel(this);
    m_label_title_{{ label.id }}->setText("{{ label.name }}");
    l->addWidget(m_label_title_{{ label.id }});

    m_label_value_{{ label.id }} = new QLabel(this);
    m_label_value_{{ label.id }}->setText("-");
    l->addWidget(m_label_value_{{ label.id }});
    {% endfor %}

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
    if (m_client->state() == QMqttClient::ClientState::Connected)
    {
        std::cout << "Connected!" << std::endl;

        // Subscribe to some topics
        this->subscribeToAllTopics();

    }
}

void DashBoard::subscribeToAllTopics()
{
    {% for topic in topics %}
    QString topic_{{ topic.id }} = QString("{{ topic.name }}");
    m_subscription_{{ topic.id }} = m_client->subscribe(topic_{{ topic.id }});
    if (m_subscription_{{ topic.id }}) {
        std::cout << "Subscribed to! " << topic_{{ topic.id }}.toStdString() << std::endl;
        connect(
            m_subscription_{{ topic.id }},
            &QMqttSubscription::messageReceived,
            this,
            &DashBoard::on_message_{{ topic.id }}
        );
    } else {
        std::cout << "AARG, subscribe error: ptr = " <<  std::endl;
    }
    {% endfor %}
}

{% for topic in topics %}
void DashBoard::on_message_{{ topic.id }}(const QMqttMessage &msg)
{
    QString txt(msg.payload());
    // std::cout << "GOt message: " << txt.toStdString() << std::endl;
    // std::cout << "MSG!" << msg.payload() << endl;

    // Update labels in GUI:
    {% for label in topic.labels %}
    // TODO: insert rate limiting here.
    m_label_value_{{ label.id }}->setText(txt);
    {% endfor %}
}
{% endfor %}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    DashBoard dashboard;
    dashboard.show();
    app.exec();
}
