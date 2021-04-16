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

#endif

using namespace std;

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

Q_SIGNALS:
void plotSignal();

private:
void temp0_CB(const std_msgs::Float32ConstPtr &msg);
void temp1_CB(const std_msgs::Float32ConstPtr &msg);
void temp2_CB(const std_msgs::Float32ConstPtr &msg);
void temp3_CB(const std_msgs::Float32ConstPtr &msg);
void temp4_CB(const std_msgs::Float32ConstPtr &msg);

void current0_CB(const std_msgs::Float32ConstPtr &msg);
void current1_CB(const std_msgs::Float32ConstPtr &msg);
void current2_CB(const std_msgs::Float32ConstPtr &msg);
void current3_CB(const std_msgs::Float32ConstPtr &msg);
void current4_CB(const std_msgs::Float32ConstPtr &msg);

void rps_CB(const std_msgs::Float32ConstPtr &msg);
void torque_CB(const std_msgs::Float32ConstPtr &msg);

Ui::UVEN_CONTROL_CENTER_GUI ui;
QColor color_pallette[16] = {Qt::blue, Qt::red, Qt::green, Qt::cyan, Qt::magenta, Qt::darkGray, Qt::darkRed, Qt::darkGreen,
                             Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow, Qt::black, Qt::gray, Qt::green, Qt::cyan};
QWidget *widget_;
ros::NodeHandlePtr nh;
ros::Subscriber temp_subscriber[5], current_subscriber[5];
map<string,QVector<double> > values;
boost::shared_ptr<ros::AsyncSpinner> spinner;
ros::Time start_time;
float warm = 40, hot = 60, nominal_current = 2.25, a_lot_of_current = 3.5;
};
