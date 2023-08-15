#pragma once

#ifndef Q_MOC_RUN

#include <ros/ros.h>
#include <yaml-cpp/yaml.h>
#include <rqt_gui_cpp/plugin.h>
#include <QWidget>
#include <pluginlib/class_list_macros.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <uven_control_center_gui/ui_uven_control_center_gui.h>
#include <std_msgs/Float32.h>
#include <qwt_dial_needle.h>
#include <boost/asio.hpp>
#include <std_msgs/Int32.h>
#include <fstream>
#include <chrono>

#endif

using namespace std;

#define BUFFER_SIZE 176

union FRAME{
  struct{
    uint16_t time;
    uint16_t control;
    float temperature[17];
    uint16_t led_fan;
    uint16_t chamber_fan;
    uint16_t target_current[16];
    uint16_t current[16];
    uint16_t gate[16];
    uint32_t crc;
  }values;
  uint8_t data[BUFFER_SIZE];
};

class UVEN_CONTROL_CENTER_GUI : public rqt_gui_cpp::Plugin {
Q_OBJECT
public:
    UVEN_CONTROL_CENTER_GUI();

    virtual void initPlugin(qt_gui_cpp::PluginContext &context);

    virtual void shutdownPlugin();

    virtual void saveSettings(qt_gui_cpp::Settings &plugin_settings,
                            qt_gui_cpp::Settings &instance_settings) const;

    virtual void restoreSettings(const qt_gui_cpp::Settings &plugin_settings,
                                const qt_gui_cpp::Settings &instance_settings);
public Q_SLOTS:;
    void plotData();
    void led_fan_changed(double value);
    void chamber_fan_changed(double value);
    void target_current_changed(double value);
    void emergency_off();
    void record();

Q_SIGNALS:
    void plotSignal();

private:
    void lamp_coms();
    void TempExternal(const std_msgs::Int32ConstPtr &msg);

    Ui::UVEN_CONTROL_CENTER_GUI ui;
    QColor color_pallette[16] = {Qt::blue, Qt::red, Qt::green, Qt::cyan, Qt::magenta, Qt::darkGray, Qt::darkRed, Qt::darkGreen,
                                Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow, Qt::black, Qt::gray, Qt::green, Qt::cyan};
    QWidget *widget_;
    ros::NodeHandlePtr nh;
    ros::Subscriber temp_external_sub;
    map<string,QVector<double> > values;
    boost::shared_ptr<ros::AsyncSpinner> spinner;
    boost::shared_ptr<std::thread> lamp_coms_thread;
    ros::Time start_time;
    float warm = 40, hot = 60, nominal_current = 2.25, a_lot_of_current = 3.5;
    vector<QCustomPlot*> temp_plot, current_plot, gate_plot, target_current_plot;
    vector<QwtDial*> temp_dial, current_dial, gate_dial, target_current_dial;
    vector<QVector<double>> temperature, temperature_time, current, current_time, gate, gate_time, target_current, target_current_time;
    double chamber_fan = 0, led_fan = 0;
    string lamp_ip;
    int lamp_port;
    FRAME tx,rx;
    bool emergency_off_flag = false;
    int temp_external = 0;
    ofstream record_file;
    bool record_flag = false;
};
