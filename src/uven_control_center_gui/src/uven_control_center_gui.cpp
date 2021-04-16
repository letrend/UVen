#include <uven_control_center_gui/uven_control_center_gui.hpp>

UVEN_CONTROL_CENTER_GUI::UVEN_CONTROL_CENTER_GUI()
        : widget_(0) {
        this->setObjectName("UVEN_CONTROL_CENTER_GUI");
}

void UVEN_CONTROL_CENTER_GUI::initPlugin(qt_gui_cpp::PluginContext &context) {
        // access standalone command line arguments
        QStringList argv = context.argv();
        // create QWidget
        widget_ = new QWidget();
        // extend the widget with all attributes and children from UI file
        ui.setupUi(widget_);
        // add widget to the user interface
        context.addWidget(widget_);

        nh = ros::NodeHandlePtr(new ros::NodeHandle);
        if (!ros::isInitialized()) {
                int argc = 0;
                char **argv = NULL;
                ros::init(argc, argv, "UVEN_CONTROL_CENTER_GUI");
        }

        ui.temp_0->addGraph();
        ui.temp_0->graph(0)->setPen(QColor(Qt::green));
        ui.temp_1->addGraph();
        ui.temp_1->graph(0)->setPen(QColor(Qt::green));
        ui.temp_2->addGraph();
        ui.temp_2->graph(0)->setPen(QColor(Qt::green));
        ui.temp_3->addGraph();
        ui.temp_3->graph(0)->setPen(QColor(Qt::green));
        ui.temp_4->addGraph();
        ui.temp_4->graph(0)->setPen(QColor(Qt::green));

        ui.current_0->addGraph();
        ui.current_0->graph(0)->setPen(QColor(Qt::green));
        ui.current_1->addGraph();
        ui.current_1->graph(0)->setPen(QColor(Qt::green));
        ui.current_2->addGraph();
        ui.current_2->graph(0)->setPen(QColor(Qt::green));
        ui.current_3->addGraph();
        ui.current_3->graph(0)->setPen(QColor(Qt::green));
        ui.current_4->addGraph();
        ui.current_4->graph(0)->setPen(QColor(Qt::green));

        QwtDialSimpleNeedle *needle = new QwtDialSimpleNeedle(QwtDialSimpleNeedle::QwtDialSimpleNeedle::Arrow, true, Qt::red, Qt::red);
        ui.temp_dial_0->setNeedle(needle);
        ui.temp_dial_1->setNeedle(needle);
        ui.temp_dial_2->setNeedle(needle);
        ui.temp_dial_3->setNeedle(needle);
        ui.temp_dial_4->setNeedle(needle);

        ui.current_dial_0->setNeedle(needle);
        ui.current_dial_1->setNeedle(needle);
        ui.current_dial_2->setNeedle(needle);
        ui.current_dial_3->setNeedle(needle);
        ui.current_dial_4->setNeedle(needle);

        start_time = ros::Time::now();

        QObject::connect(this, SIGNAL(plotSignal()), this, SLOT(plotData()));

        temp_subscriber[0] = nh->subscribe("temp0",1,&UVEN_CONTROL_CENTER_GUI::temp0_CB,this);
        temp_subscriber[1] = nh->subscribe("temp1",1,&UVEN_CONTROL_CENTER_GUI::temp1_CB,this);
        temp_subscriber[2] = nh->subscribe("temp2",1,&UVEN_CONTROL_CENTER_GUI::temp2_CB,this);
        temp_subscriber[3] = nh->subscribe("temp3",1,&UVEN_CONTROL_CENTER_GUI::temp3_CB,this);
        temp_subscriber[4] = nh->subscribe("temp4",1,&UVEN_CONTROL_CENTER_GUI::temp4_CB,this);

        current_subscriber[0] = nh->subscribe("current0",1,&UVEN_CONTROL_CENTER_GUI::current0_CB,this);
        current_subscriber[1] = nh->subscribe("current1",1,&UVEN_CONTROL_CENTER_GUI::current1_CB,this);
        current_subscriber[2] = nh->subscribe("current2",1,&UVEN_CONTROL_CENTER_GUI::current2_CB,this);
        current_subscriber[3] = nh->subscribe("current3",1,&UVEN_CONTROL_CENTER_GUI::current3_CB,this);
        current_subscriber[4] = nh->subscribe("current4",1,&UVEN_CONTROL_CENTER_GUI::current4_CB,this);
        //
        // rps_subscriber = nh->subscribe("rps",1,&UVEN_CONTROL_CENTER_GUI::rps_CB,this);
        // torque_subscriber = nh->subscribe("torque",1,&UVEN_CONTROL_CENTER_GUI::torque_CB,this);

        ROS_INFO("uven gui initialized");
}

void UVEN_CONTROL_CENTER_GUI::plotData(){
        ui.temp_0->graph(0)->setData(values["temp0_sec"],values["temp0"]);
        ui.temp_1->graph(0)->setData(values["temp1_sec"],values["temp1"]);
        ui.temp_2->graph(0)->setData(values["temp2_sec"],values["temp2"]);
        ui.temp_3->graph(0)->setData(values["temp3_sec"],values["temp3"]);
        ui.temp_4->graph(0)->setData(values["temp4_sec"],values["temp4"]);

        ui.current_0->graph(0)->setData(values["current0_sec"],values["current0"]);
        ui.current_1->graph(0)->setData(values["current1_sec"],values["current1"]);
        ui.current_2->graph(0)->setData(values["current2_sec"],values["current2"]);
        ui.current_3->graph(0)->setData(values["current3_sec"],values["current3"]);
        ui.current_4->graph(0)->setData(values["current4_sec"],values["current4"]);

        if(values["temp0"].back()>warm) {
                ui.temp_dial_0->setStyleSheet("background: yellow");
                ui.temp_0->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["temp0"].back()>hot) {
                ui.temp_dial_0->setStyleSheet("background: red");
                ui.temp_0->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.temp_dial_0->setStyleSheet("background: green");
                ui.temp_0->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["temp1"].back()>warm) {
                ui.temp_dial_1->setStyleSheet("background: yellow");
                ui.temp_1->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["temp1"].back()>hot) {
                ui.temp_dial_1->setStyleSheet("background: red");
                ui.temp_1->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.temp_dial_1->setStyleSheet("background: green");
                ui.temp_1->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["temp2"].back()>warm) {
                ui.temp_dial_2->setStyleSheet("background: yellow");
                ui.temp_2->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["temp2"].back()>hot) {
                ui.temp_dial_2->setStyleSheet("background: red");
                ui.temp_2->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.temp_dial_2->setStyleSheet("background: green");
                ui.temp_2->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["temp3"].back()>warm) {
                ui.temp_dial_3->setStyleSheet("background: yellow");
                ui.temp_3->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["temp3"].back()>hot) {
                ui.temp_dial_3->setStyleSheet("background: red");
                ui.temp_3->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.temp_dial_3->setStyleSheet("background: green");
                ui.temp_3->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["temp4"].back()>warm) {
                ui.temp_dial_4->setStyleSheet("background: yellow");
                ui.temp_4->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["temp4"].back()>hot) {
                ui.temp_dial_4->setStyleSheet("background: red");
                ui.temp_4->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.temp_dial_4->setStyleSheet("background: green");
                ui.temp_4->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["current0"].back()>nominal_current) {
                ui.current_dial_0->setStyleSheet("background: yellow");
                ui.current_0->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["current0"].back()>a_lot_of_current) {
                ui.current_dial_0->setStyleSheet("background: red");
                ui.current_0->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.current_dial_0->setStyleSheet("background: green");
                ui.current_0->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["current1"].back()>nominal_current) {
                ui.current_dial_1->setStyleSheet("background: yellow");
                ui.current_1->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["current1"].back()>a_lot_of_current) {
                ui.current_dial_1->setStyleSheet("background: red");
                ui.current_1->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.current_dial_1->setStyleSheet("background: green");
                ui.current_1->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["current2"].back()>nominal_current) {
                ui.current_dial_2->setStyleSheet("background: yellow");
                ui.current_2->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["current2"].back()>a_lot_of_current) {
                ui.current_dial_2->setStyleSheet("background: red");
                ui.current_2->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.current_dial_2->setStyleSheet("background: green");
                ui.current_2->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["current3"].back()>nominal_current) {
                ui.current_dial_3->setStyleSheet("background: yellow");
                ui.current_3->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["current3"].back()>a_lot_of_current) {
                ui.current_dial_3->setStyleSheet("background: red");
                ui.current_3->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.current_dial_3->setStyleSheet("background: green");
                ui.current_3->graph(0)->setPen(QColor(Qt::blue));
        }

        if(values["current4"].back()>nominal_current) {
                ui.current_dial_4->setStyleSheet("background: yellow");
                ui.current_4->graph(0)->setPen(QColor(Qt::yellow));
        }else if(values["current4"].back()>a_lot_of_current) {
                ui.current_dial_4->setStyleSheet("background: red");
                ui.current_4->graph(0)->setPen(QColor(Qt::red));
        }else{
                ui.current_dial_4->setStyleSheet("background: green");
                ui.current_4->graph(0)->setPen(QColor(Qt::blue));
        }

        ui.temp_0->rescaleAxes();
        ui.temp_1->rescaleAxes();
        ui.temp_2->rescaleAxes();
        ui.temp_3->rescaleAxes();
        ui.temp_4->rescaleAxes();

        ui.current_0->rescaleAxes();
        ui.current_1->rescaleAxes();
        ui.current_2->rescaleAxes();
        ui.current_3->rescaleAxes();
        ui.current_4->rescaleAxes();

        ui.temp_0->replot();
        ui.temp_1->replot();
        ui.temp_2->replot();
        ui.temp_3->replot();
        ui.temp_4->replot();

        ui.current_0->replot();
        ui.current_1->replot();
        ui.current_2->replot();
        ui.current_3->replot();
        ui.current_4->replot();

        ui.temp_dial_0->setValue(values["temp0"].back());
        ui.temp_dial_1->setValue(values["temp1"].back());
        ui.temp_dial_2->setValue(values["temp2"].back());
        ui.temp_dial_3->setValue(values["temp3"].back());
        ui.temp_dial_4->setValue(values["temp4"].back());

        ui.current_dial_0->setValue(values["current0"].back());
        ui.current_dial_1->setValue(values["current1"].back());
        ui.current_dial_2->setValue(values["current2"].back());
        ui.current_dial_3->setValue(values["current3"].back());
        ui.current_dial_4->setValue(values["current4"].back());
}

void UVEN_CONTROL_CENTER_GUI::temp0_CB(const std_msgs::Float32ConstPtr &msg){
        values["temp0_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["temp0"].push_back(msg->data);
        if(values["temp0"].size()>100) {
                values["temp0_sec"].pop_front();
                values["temp0"].pop_front();
        }
        Q_EMIT plotSignal();
}

void UVEN_CONTROL_CENTER_GUI::temp1_CB(const std_msgs::Float32ConstPtr &msg){
        values["temp1_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["temp1"].push_back(msg->data);
        if(values["temp1"].size()>100) {
                values["temp1_sec"].pop_front();
                values["temp1"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::temp2_CB(const std_msgs::Float32ConstPtr &msg){
        values["temp2_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["temp2"].push_back(msg->data);
        if(values["temp2"].size()>100) {
                values["temp2_sec"].pop_front();
                values["temp2"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::temp3_CB(const std_msgs::Float32ConstPtr &msg){
        values["temp3_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["temp3"].push_back(msg->data);
        if(values["temp3"].size()>100) {
                values["temp3_sec"].pop_front();
                values["temp3"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::temp4_CB(const std_msgs::Float32ConstPtr &msg){
        values["temp4_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["temp4"].push_back(msg->data);
        if(values["temp4"].size()>100) {
                values["temp4_sec"].pop_front();
                values["temp4"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::current0_CB(const std_msgs::Float32ConstPtr &msg){
        values["current0_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["current0"].push_back(msg->data);
        if(values["current0"].size()>100) {
                values["current0_sec"].pop_front();
                values["current0"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::current1_CB(const std_msgs::Float32ConstPtr &msg){
        values["current1_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["current1"].push_back(msg->data);
        if(values["current1"].size()>100) {
                values["current1_sec"].pop_front();
                values["current1"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::current2_CB(const std_msgs::Float32ConstPtr &msg){
        values["current2_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["current2"].push_back(msg->data);
        if(values["current2"].size()>100) {
                values["current2_sec"].pop_front();
                values["current2"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::current3_CB(const std_msgs::Float32ConstPtr &msg){
        values["current3_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["current3"].push_back(msg->data);
        if(values["current3"].size()>100) {
                values["current3_sec"].pop_front();
                values["current3"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::current4_CB(const std_msgs::Float32ConstPtr &msg){
        values["current4_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["current4"].push_back(msg->data);
        if(values["current4"].size()>100) {
                values["current4_sec"].pop_front();
                values["current4"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::rps_CB(const std_msgs::Float32ConstPtr &msg){
        values["rps_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["rps"].push_back(msg->data);
        if(values["rps"].size()>100) {
                values["rps_sec"].pop_front();
                values["rps"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::torque_CB(const std_msgs::Float32ConstPtr &msg){
        values["torque_sec"].push_back((ros::Time::now()-start_time).toSec());
        values["torque"].push_back(msg->data);
        if(values["torque"].size()>100) {
                values["torque_sec"].pop_front();
                values["torque"].pop_front();
        }
}

void UVEN_CONTROL_CENTER_GUI::shutdownPlugin() {
        ros::shutdown();
}

void UVEN_CONTROL_CENTER_GUI::saveSettings(qt_gui_cpp::Settings &plugin_settings,
                                           qt_gui_cpp::Settings &instance_settings) const {
}

void UVEN_CONTROL_CENTER_GUI::restoreSettings(const qt_gui_cpp::Settings &plugin_settings,
                                              const qt_gui_cpp::Settings &instance_settings) {

}

PLUGINLIB_EXPORT_CLASS(UVEN_CONTROL_CENTER_GUI, rqt_gui_cpp::Plugin)
