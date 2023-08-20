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
    temp_plot.push_back(ui.temp_0); temp_plot.push_back(ui.temp_1); temp_plot.push_back(ui.temp_2); temp_plot.push_back(ui.temp_3);
    temp_plot.push_back(ui.temp_4); temp_plot.push_back(ui.temp_5); temp_plot.push_back(ui.temp_6); temp_plot.push_back(ui.temp_7);
    temp_plot.push_back(ui.temp_8); temp_plot.push_back(ui.temp_9); temp_plot.push_back(ui.temp_10); temp_plot.push_back(ui.temp_11);
    temp_plot.push_back(ui.temp_12); temp_plot.push_back(ui.temp_13); temp_plot.push_back(ui.temp_14); temp_plot.push_back(ui.temp_15);
    
    for(auto p:temp_plot){
        p->addGraph();
        p->graph(0)->setPen(QColor(Qt::green));
        p->xAxis->setAutoTickStep(false);
        p->xAxis->setTickStep(5);
    }
    
    current_plot.push_back(ui.current_0); current_plot.push_back(ui.current_1); current_plot.push_back(ui.current_2); current_plot.push_back(ui.current_3);
    current_plot.push_back(ui.current_4); current_plot.push_back(ui.current_5); current_plot.push_back(ui.current_6); current_plot.push_back(ui.current_7);
    current_plot.push_back(ui.current_8); current_plot.push_back(ui.current_9); current_plot.push_back(ui.current_10); current_plot.push_back(ui.current_11);
    current_plot.push_back(ui.current_12); current_plot.push_back(ui.current_13); current_plot.push_back(ui.current_14); current_plot.push_back(ui.current_15);
    
    for(auto p:current_plot){
        p->addGraph();
        p->graph(0)->setPen(QColor(Qt::green));
        p->xAxis->setAutoTickStep(false);
        p->xAxis->setTickStep(5);
    }

    gate_plot.push_back(ui.gate_0); gate_plot.push_back(ui.gate_1); gate_plot.push_back(ui.gate_2); gate_plot.push_back(ui.gate_3);
    gate_plot.push_back(ui.gate_4); gate_plot.push_back(ui.gate_5); gate_plot.push_back(ui.gate_6); gate_plot.push_back(ui.gate_7);
    gate_plot.push_back(ui.gate_8); gate_plot.push_back(ui.gate_9); gate_plot.push_back(ui.gate_10); gate_plot.push_back(ui.gate_11);
    gate_plot.push_back(ui.gate_12); gate_plot.push_back(ui.gate_13); gate_plot.push_back(ui.gate_14); gate_plot.push_back(ui.gate_15);
    
    for(auto p:gate_plot){
        p->addGraph();
        p->graph(0)->setPen(QColor(Qt::green));
        p->xAxis->setAutoTickStep(false);
        p->xAxis->setTickStep(5);
    }

    target_current_plot.push_back(ui.target_current_0); target_current_plot.push_back(ui.target_current_1); target_current_plot.push_back(ui.target_current_2); target_current_plot.push_back(ui.target_current_3);
    target_current_plot.push_back(ui.target_current_4); target_current_plot.push_back(ui.target_current_5); target_current_plot.push_back(ui.target_current_6); target_current_plot.push_back(ui.target_current_7);
    target_current_plot.push_back(ui.target_current_8); target_current_plot.push_back(ui.target_current_9); target_current_plot.push_back(ui.target_current_10); target_current_plot.push_back(ui.target_current_11);
    target_current_plot.push_back(ui.target_current_12); target_current_plot.push_back(ui.target_current_13); target_current_plot.push_back(ui.target_current_14); target_current_plot.push_back(ui.target_current_15);
    
    for(auto p:target_current_plot){
        p->addGraph();
        p->graph(0)->setPen(QColor(Qt::green));
        p->xAxis->setAutoTickStep(false);
        p->xAxis->setTickStep(5);
    }
    
    temp_dial.push_back(ui.temp_dial_0); temp_dial.push_back(ui.temp_dial_1); temp_dial.push_back(ui.temp_dial_2); temp_dial.push_back(ui.temp_dial_3);
    temp_dial.push_back(ui.temp_dial_4); temp_dial.push_back(ui.temp_dial_5); temp_dial.push_back(ui.temp_dial_6); temp_dial.push_back(ui.temp_dial_7);
    temp_dial.push_back(ui.temp_dial_8); temp_dial.push_back(ui.temp_dial_9); temp_dial.push_back(ui.temp_dial_10); temp_dial.push_back(ui.temp_dial_11);
    temp_dial.push_back(ui.temp_dial_12); temp_dial.push_back(ui.temp_dial_13); temp_dial.push_back(ui.temp_dial_14); temp_dial.push_back(ui.temp_dial_15);

    current_dial.push_back(ui.current_dial_0); current_dial.push_back(ui.current_dial_1); current_dial.push_back(ui.current_dial_2); current_dial.push_back(ui.current_dial_3);
    current_dial.push_back(ui.current_dial_4); current_dial.push_back(ui.current_dial_5); current_dial.push_back(ui.current_dial_6); current_dial.push_back(ui.current_dial_7);
    current_dial.push_back(ui.current_dial_8); current_dial.push_back(ui.current_dial_9); current_dial.push_back(ui.current_dial_10); current_dial.push_back(ui.current_dial_11);
    current_dial.push_back(ui.current_dial_12); current_dial.push_back(ui.current_dial_13); current_dial.push_back(ui.current_dial_14); current_dial.push_back(ui.current_dial_15);

    gate_dial.push_back(ui.gate_dial_0); gate_dial.push_back(ui.gate_dial_1); gate_dial.push_back(ui.gate_dial_2); gate_dial.push_back(ui.gate_dial_3);
    gate_dial.push_back(ui.gate_dial_4); gate_dial.push_back(ui.gate_dial_5); gate_dial.push_back(ui.gate_dial_6); gate_dial.push_back(ui.gate_dial_7);
    gate_dial.push_back(ui.gate_dial_8); gate_dial.push_back(ui.gate_dial_9); gate_dial.push_back(ui.gate_dial_10); gate_dial.push_back(ui.gate_dial_11);
    gate_dial.push_back(ui.gate_dial_12); gate_dial.push_back(ui.gate_dial_13); gate_dial.push_back(ui.gate_dial_14); gate_dial.push_back(ui.gate_dial_15);

    target_current_dial.push_back(ui.target_current_dial_0); target_current_dial.push_back(ui.target_current_dial_1); target_current_dial.push_back(ui.target_current_dial_2); target_current_dial.push_back(ui.target_current_dial_3);
    target_current_dial.push_back(ui.target_current_dial_4); target_current_dial.push_back(ui.target_current_dial_5); target_current_dial.push_back(ui.target_current_dial_6); target_current_dial.push_back(ui.target_current_dial_7);
    target_current_dial.push_back(ui.target_current_dial_8); target_current_dial.push_back(ui.target_current_dial_9); target_current_dial.push_back(ui.target_current_dial_10); target_current_dial.push_back(ui.target_current_dial_11);
    target_current_dial.push_back(ui.target_current_dial_12); target_current_dial.push_back(ui.target_current_dial_13); target_current_dial.push_back(ui.target_current_dial_14); target_current_dial.push_back(ui.target_current_dial_15);

    QwtDialSimpleNeedle *needle = new QwtDialSimpleNeedle(QwtDialSimpleNeedle::QwtDialSimpleNeedle::Arrow, true, Qt::red, Qt::red);
    for(auto dial:temp_dial){
        dial->setNeedle(needle);
    }
    for(auto dial:current_dial){
        dial->setNeedle(needle);
    }
    for(auto dial:gate_dial){
        dial->setNeedle(needle);
    }
    for(auto dial:target_current_dial){
        dial->setNeedle(needle);
    }

    ui.chamber_fan_dial->setNeedle(needle);
    ui.led_fan_dial->setNeedle(needle);

    enable_led.push_back(ui.enable_0); enable_led.push_back(ui.enable_1); enable_led.push_back(ui.enable_2); enable_led.push_back(ui.enable_3);
    enable_led.push_back(ui.enable_4); enable_led.push_back(ui.enable_5); enable_led.push_back(ui.enable_6); enable_led.push_back(ui.enable_7);
    enable_led.push_back(ui.enable_8); enable_led.push_back(ui.enable_9); enable_led.push_back(ui.enable_10); enable_led.push_back(ui.enable_11);
    enable_led.push_back(ui.enable_12); enable_led.push_back(ui.enable_13); enable_led.push_back(ui.enable_14); enable_led.push_back(ui.enable_15);


    current.resize(16);
    current_time.resize(16);
    temperature.resize(16);
    temperature_time.resize(16);
    gate.resize(16);
    gate_time.resize(16);
    target_current.resize(16);
    target_current_time.resize(16);

    QObject::connect(this, SIGNAL(plotSignal()), this, SLOT(plotData()));
    QObject::connect(ui.chamber_fan_slider, SIGNAL(valueChanged(double)), this, SLOT(chamber_fan_changed(double)));
    QObject::connect(ui.led_fan_slider, SIGNAL(valueChanged(double)), this, SLOT(led_fan_changed(double)));
    QObject::connect(ui.target_current_slider, SIGNAL(valueChanged(double)), this, SLOT(target_current_changed(double)));
    QObject::connect(ui.emergency_off, SIGNAL(clicked()), this, SLOT(emergency_off()));
    QObject::connect(ui.record, SIGNAL(clicked()), this, SLOT(record()));

    if(nh->hasParam("lamp_ip") && nh->hasParam("lamp_port")){
        nh->getParam("lamp_ip",lamp_ip);
        nh->getParam("lamp_port",lamp_port);
        lamp_coms_thread.reset(new std::thread(&UVEN_CONTROL_CENTER_GUI::lamp_coms, this));
        lamp_coms_thread->detach();
    }

    temp_external_sub = nh->subscribe("temp_external", 1, &UVEN_CONTROL_CENTER_GUI::TempExternal, this);

    ROS_INFO("uven gui initialized");
}

void UVEN_CONTROL_CENTER_GUI::TempExternal(const std_msgs::Int32ConstPtr &msg){
    temp_external = msg->data;
}

void UVEN_CONTROL_CENTER_GUI::led_fan_changed(double value){
    // ROS_INFO("led fan changed to %d", (int)value);
    tx.values.led_fan = (int)value/100.0*255;
}

void UVEN_CONTROL_CENTER_GUI::chamber_fan_changed(double value){
    // ROS_INFO("chamber fan changed to %d", (int)value);
    tx.values.chamber_fan = (int)value/100.0*255;
}

void UVEN_CONTROL_CENTER_GUI::target_current_changed(double value){
    // ROS_INFO("target current changed to %d", (int)value);
    if(!emergency_off_flag){
        for(int i=0;i<16;i++){
            if(enable_led[i]->isChecked()){
                tx.values.target_current[i] = (int)(value*1000);
            }else{
                tx.values.target_current[i] = 0;
            }
        }
    }
}

void UVEN_CONTROL_CENTER_GUI::emergency_off(){
    if(ui.emergency_off->isChecked()){
        emergency_off_flag = true;
        for(int i=0;i<16;i++){
            tx.values.target_current[i] = 0;
        }
        ui.emergency_off->setStyleSheet("background: red");
        ui.target_current_slider->setEnabled(false);
    }else{
        emergency_off_flag = false;
        ui.emergency_off->setStyleSheet("background: lightgrey");
        ui.target_current_slider->setEnabled(true);
    }
}

void UVEN_CONTROL_CENTER_GUI::record(){
    if(ui.record->isChecked()){
        ROS_INFO("record started");
        record_file.open("uven.log",ios_base::out);
        record_file << "time, temp_external[C], temp(0-15)[1], current(0-15)[mA]\n";
        record_flag = true;
    }else{
        ROS_INFO("record stopped");
        record_flag = false;
        record_file.close();
    }
}

void UVEN_CONTROL_CENTER_GUI::lamp_coms(){
    ROS_INFO("lamp coms started");
    start_time = ros::Time::now();
    ros::Rate rate(10);
    boost::asio::io_service ios;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(lamp_ip), lamp_port);
    boost::asio::ip::tcp::socket socket(ios);
    ros::Time t0;
    while(ros::ok()){
        try{
            socket.connect(endpoint);
            boost::system::error_code error;
            t0 = ros::Time::now();
            socket.write_some(boost::asio::buffer(tx.data, BUFFER_SIZE), error);
            socket.read_some(boost::asio::buffer(rx.data, BUFFER_SIZE), error);
            socket.close();
        }catch( const boost::system::system_error& ex ){
            ROS_ERROR_STREAM_THROTTLE(1,ex.what());
            continue;
        }
        for(int i=0;i<16;i++){
            current[i].push_back(((int)rx.values.current[i]/1000.0));
            current_time[i].push_back((t0-start_time).toSec());
            temperature[i].push_back(rx.values.temperature[i]);
            temperature_time[i].push_back((t0-start_time).toSec());
            gate[i].push_back(rx.values.gate[i]);
            gate_time[i].push_back((t0-start_time).toSec());
            target_current[i].push_back(((int)rx.values.target_current[i])/1000.0);
            target_current_time[i].push_back((t0-start_time).toSec());
            while((temperature_time[i].back()-temperature_time[i].front())>60){
                current[i].pop_front();
                current_time[i].pop_front();
                temperature[i].pop_front();
                temperature_time[i].pop_front();
                gate[i].pop_front();
                gate_time[i].pop_front();
                target_current[i].pop_front();
                target_current_time[i].pop_front();
            }
        }
        temp_driver = rx.values.temperature[16];
        chamber_fan = ((int)rx.values.chamber_fan)/255.0f*100;
        led_fan = ((int)rx.values.led_fan)/255.0f*100;
        Q_EMIT plotSignal();
        
        rate.sleep();
    }
}

void UVEN_CONTROL_CENTER_GUI::plotData(){
    for(int i=0;i<16;i++){
        if(ui.tabWidget->currentIndex()==0){
            if(current[i].back()>nominal_current) {
                current_dial[i]->setStyleSheet("background: yellow");
                current_plot[i]->graph(0)->setPen(QColor(Qt::darkYellow));
            }else if(current[i].back()>a_lot_of_current) {
                current_dial[i]->setStyleSheet("background: red");
                current_plot[i]->graph(0)->setPen(QColor(Qt::red));
            }else{
                current_dial[i]->setStyleSheet("background: lightblue");
                current_plot[i]->graph(0)->setPen(QColor(Qt::blue));
            }
            
            current_plot[i]->graph(0)->setData(current_time[i],current[i]);
            current_plot[i]->rescaleAxes();
            current_plot[i]->replot();
            current_dial[i]->setValue(current[i].back());
        }

        if(ui.tabWidget->currentIndex()==1){
            if(target_current[i].back()>nominal_current) {
                target_current_dial[i]->setStyleSheet("background: yellow");
                target_current_plot[i]->graph(0)->setPen(QColor(Qt::darkYellow));
            }else if(target_current[i].back()>a_lot_of_current) {
                target_current_dial[i]->setStyleSheet("background: red");
                target_current_plot[i]->graph(0)->setPen(QColor(Qt::red));
            }else{
                target_current_dial[i]->setStyleSheet("background: lightblue");
                target_current_plot[i]->graph(0)->setPen(QColor(Qt::blue));
            }
            
            target_current_plot[i]->graph(0)->setData(target_current_time[i],target_current[i]);
            target_current_plot[i]->rescaleAxes();
            target_current_plot[i]->replot();
            target_current_dial[i]->setValue(target_current[i].back());
        }
        
        if(ui.tabWidget->currentIndex()==2){
            if(temperature[i].back()>warm) {
                temp_dial[i]->setStyleSheet("background: yellow");
                temp_plot[i]->graph(0)->setPen(QColor(Qt::darkYellow));
            }else if(temperature[i].back()>hot) {
                temp_dial[i]->setStyleSheet("background: red");
                temp_plot[i]->graph(0)->setPen(QColor(Qt::red));
            }else{
                temp_dial[i]->setStyleSheet("background: lightblue");
                temp_plot[i]->graph(0)->setPen(QColor(Qt::blue));
            }
            
            temp_plot[i]->graph(0)->setData(temperature_time[i],temperature[i]);
            temp_plot[i]->rescaleAxes();
            temp_plot[i]->replot();
            temp_dial[i]->setValue(temperature[i].back());
        }

        if(ui.tabWidget->currentIndex()==3){
            if(gate[i].back()>warm) {
                gate_dial[i]->setStyleSheet("background: yellow");
                gate_plot[i]->graph(0)->setPen(QColor(Qt::darkYellow));
            }else if(gate[i].back()>hot) {
                gate_dial[i]->setStyleSheet("background: red");
                gate_plot[i]->graph(0)->setPen(QColor(Qt::red));
            }else{
                gate_dial[i]->setStyleSheet("background: lightblue");
                gate_plot[i]->graph(0)->setPen(QColor(Qt::blue));
            }
            
            
            gate_plot[i]->graph(0)->setData(gate_time[i],gate[i]);
            gate_plot[i]->rescaleAxes();
            gate_plot[i]->replot();
            gate_dial[i]->setValue(gate[i].back()/4095.0*100);
        }
    }
    ui.chamber_fan_dial->setValue(chamber_fan);
    ui.led_fan_dial->setValue(led_fan);

    if(!rx.values.control){
        ui.interlock->setStyleSheet("background: lightgreen");
    }else{
        ui.interlock->setStyleSheet("background: red");
    }

    ui.temp_external->setValue(temp_external);
    ui.temp_driver->setValue(temp_driver);

    if(record_flag){
        static ros::Time t0 = ros::Time::now();
        if((ros::Time::now()-t0).toSec()>1){
            t0 = ros::Time::now();
            const auto now = std::chrono::system_clock::now();
            record_file << std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count() << ", " << temp_external << ", ";
            for(int i=0;i<16;i++){
                record_file << int(temperature[i].back()) << ", ";
            }
            for(int i=0;i<16;i++){
                record_file << int(current[i].back()*1000);
                if(i<15){
                   record_file << ", ";
                }
            }
            record_file << endl;
        }
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
