#include "mainwindow.h"
#include "utils.h"

// tab1
#include "views/raw/rawaccelview.h"
#include "views/raw/rawgyroview.h"
#include "views/raw/rawmagnview.h"
#include "views/raw/rawgpsview.h"

// tab2
#include "views/raw/xdrawmagnview.h"
#include "views/calibration/numcalibrationview.h"
#include "views/calibration/xdcalibrationview.h"

// tab3/4/5
#include "controllers/direct/attributes/kalmanorientationattrcontroller.h"
#include "controllers/direct/attributes/kalmanpositionattrcontroller.h"
#include "controllers/switches/orientationmodelswitch.h"
#include "controllers/switches/positionmodelswitch.h"

#include "views/orientation/rpyorientationview.h"
#include "views/orientation/xdorientationview.h"
#include "views/orientation/stdorientationview.h"
#include "views/position/enupositionview.h"
#include "views/position/trackpositionview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(std::make_unique<Ui::MainWindow>()),
    receiver("192.168.0.100", 65000, "192.168.0.100", 7700, magn_cal),
    terminal_base("192.168.0.100", 65001, "192.168.0.13", 65001),
    terminal( terminal_base )
{
    ui->setupUi(this);
    setWindowTitle("INS Filters Testing Framework");
    setWindowIcon(QIcon("res/main.png"));

    connect(ui->actionRun, SIGNAL(triggered(bool)), &terminal, SLOT(show()));
    connect(ui->actionWrite, SIGNAL(toggled(bool)), &file_out, SLOT(handle_enable(bool)));

    //tab1
    raw_tab_controller = std::make_unique<RawController>();
    raw_tab_controller->attach_view(std::make_shared<RawAccelView>(ui->plot1));
    raw_tab_controller->attach_view(std::make_shared<RawMagnView>(ui->plot2));
    raw_tab_controller->attach_view(std::make_shared<RawGyroView>(ui->plot3));
    raw_tab_controller->attach_view(std::make_shared<RawGPSView>(ui->x_le, ui->y_le, ui->z_le, ui->vx_le, ui->vy_le,ui->vz_le,
                                                                 ui->lat_le, ui->lon_le, ui->alt_le, ui->msl_alt_le, ui->time_le));

    //tab2
    magn_cal_raw_controller = std::make_unique<RawController>(ui->pushButton);
    magn_cal_raw_controller->attach_view(std::make_unique<XDRawMagnView>(ui->widget, ui->gridLayout));

    magn_cal_controller = std::make_unique<MagnCalibrationController>(magn_cal, ui->pushButton, ui->pushButton_3);
    magn_cal_controller->attach_view(std::make_shared<XDCalibrationView>(ui->widget_2, ui->gridLayout));
    magn_cal_controller->attach_view(std::make_shared<NumCalibrationView>(ui->xbias_le, ui->ybias_le, ui->zbias_le,
                                                                          ui->xspan_le, ui->yspan_le, ui->zspan_le));

    //tab3
    kalman_ori_controller = std::make_shared<OrientationFilteringController>(ui->pushButton_2, ui->pushButton_6, ui->samples_le);
    kalman_ori_controller->attach_view(std::make_shared<RPYOrientationView>(ui->plot4));
    kalman_ori_controller->attach_view(std::make_shared<XDOrientationView>(ui->dwidget, ui->gridLayout_3));
    kalman_ori_controller->attach_view(std::make_shared<StdOrientationView>(ui->roll_std_le, ui->pitch_std_le, ui->yaw_std_le,
                                                                            ui->roll_le, ui->pitch_le, ui->magnetic_heading_le));

    kalman_pos_controller = std::make_shared<PositionFilteringController>(ui->pushButton_2, ui->pushButton_6, ui->samples_le);

    auto enu_pos_view = std::make_shared<ENUPositionView>(ui->plot5);
    kalman_pos_controller->attach_view(enu_pos_view);
    kalman_pos_controller->attach_view(std::make_shared<TrackPositionView>(ui->gps_heading_le, ui->ground_speed_le));

    kalman_raw_controller = std::make_unique<RawController>(ui->pushButton_2);
    kalman_raw_controller->attach_view(enu_pos_view);

    auto kalman_oriattr_controller = std::make_unique<KalmanOrientationAttrController>(ui->gyro_process_le, ui->gyro_bias_process_le,
                                                                                      ui->accel_meas_le, ui->magn_meas_le,
                                                                                      ui->qs_init_le, ui->qx_init_le, ui->qy_init_le,
                                                                                      ui->qz_init_le, ui->bias_init_le);
    auto kalman_posattr_controller = std::make_unique<KalmanPositionAttrController>(ui->accel_process_le, ui->pos_meas_le, ui->vel_meas_le,
                                                                                    ui->pos_init_le, ui->vel_init_le, ui->accel_init_le);

    auto kalman_ori_sw = std::make_shared<OrientationModelSwitch>(ui->comboBox_3, kalman_ori_controller, std::move(kalman_oriattr_controller));
    auto kalman_pos_sw = std::make_shared<PositionModelSwitch>(ui->comboBox_4, kalman_pos_controller, std::move(kalman_posattr_controller));
    auto mix_sw = std::make_unique<MixedModelSwitch>(ui->comboBox_2, kalman_pos_sw, kalman_ori_sw);

    meta_controller = std::make_unique<MetaController>(ui->comboBox, kalman_pos_sw, kalman_ori_sw, std::move(mix_sw), kalman_pos_controller);

    //tab4
    compl_ori_controller = std::make_unique<OrientationFilteringController>(ui->pushButton_4, ui->pushButton_7, ui->samples_le_2);
    compl_ori_controller->attach_view(std::make_shared<RPYOrientationView>(ui->plot6));
    compl_ori_controller->attach_view(std::make_shared<XDOrientationView>(ui->dwidget2, ui->gridLayout_8));
    compl_ori_controller->attach_view(std::make_shared<StdOrientationView>(ui->roll_std_le_2, ui->pitch_std_le_2, ui->yaw_std_le_2,
                                                                           ui->roll_le_2, ui->pitch_le_2, ui->magnetic_heading_le_2));

    sim_pos_controller = std::make_unique<PositionFilteringController>(ui->pushButton_4, ui->pushButton_7, ui->samples_le_2);

    sim_pos_controller->attach_view(std::make_shared<ENUPositionView>(ui->plot7));
    sim_pos_controller->attach_view(std::make_shared<TrackPositionView>(ui->track_angle_le, ui->ground_speed_le_2));

    compl_oriattr_controller = std::make_unique<ComplOrientationAttrController>(ui->a_gain_le, ui->m_gain_le, ui->b_gain_le);
    sim_posattr_controller = std::make_unique<SimPositionAttrController>(ui->sim_speed_le, ui->init_track_le);

    compl_ori_controller->set_model(&ori_cf);

    compl_oriattr_controller->set_model(&ori_cf);
    compl_oriattr_controller->borrow_attributes();

    sim_pos_controller->set_model(&pos_sim);
    sim_posattr_controller->set_model(&pos_sim);
    sim_posattr_controller->borrow_attributes();

    //tab5
    remote_ori_controller = std::make_unique<OrientationRemoteController>(ui->pushButton_5, ui->samples_le_3);
    remote_ori_controller->attach_view(std::make_shared<RPYOrientationView>(ui->plot8));
    remote_ori_controller->attach_view(std::make_shared<XDOrientationView>(ui->dwidget3, ui->gridLayout_2));
    remote_ori_controller->attach_view(std::make_shared<StdOrientationView>(ui->roll_std_le_3, ui->pitch_std_le_3, ui->yaw_std_le_3,
                                                                            ui->roll_le_3, ui->pitch_le_3, ui->magnetic_heading_le_3));


    remote_pos_controller = std::make_unique<PositionRemoteController>(ui->pushButton_5, ui->samples_le_3);

    auto enu_pos_view3 = std::make_shared<ENUPositionView>(ui->plot9);
    remote_pos_controller->attach_view(enu_pos_view3);
    remote_pos_controller->attach_view(std::make_shared<TrackPositionView>(ui->track_angle_le_2, ui->ground_speed_le_3));

    remote_raw_controller = std::make_unique<RawController>();
    remote_raw_controller->attach_view(enu_pos_view3);

    compl_oriattr_remote_controlller = std::make_unique<RemoteComplOriAttr>(terminal_base, ui->pushButton_8, ui->a_gain_le_2, ui->m_gain_le_2, ui->b_gain_le_2);
    kalman_posattr_remote_controller = std::make_unique<RemoteKalmanPosAttr>(terminal_base, ui->pushButton_8, ui->accel_process_le_2, ui->pos_meas_le_2, ui->vel_meas_le_2,
                                                                             ui->pos_init_le_2, ui->vel_init_le_2, ui->accel_init_le_2);


    //other
    ui->statusBar->addWidget(&current_time);
    ui->statusBar->addWidget(&analog_pitch);
    ui->statusBar->addWidget(&analog_roll);

    resize(1300, 800);

    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void MainWindow::got_raw_packet(const RawPacket &z)
{
    current_time.setText("GPS time: " + utils::gps_time_string(z.gps_data.time));
    analog_pitch.setText("Analog pitch: " + QString::number(utils::fixed_to_angle(z.ref_pitch)));
    analog_roll.setText("Analog roll: " + QString::number(utils::fixed_to_angle(z.ref_roll)));
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    receiver.disconnect();
    connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), this, SLOT(got_raw_packet(RawPacket)));
    connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), &file_out, SLOT(handle_input(RawPacket)));

    switch(index)
    {
    case 0:
        connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), raw_tab_controller.get(), SLOT(handle_input(RawPacket)));
        break;
    case 1:
        connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), magn_cal_raw_controller.get(), SLOT(handle_input(RawPacket)));
        connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), magn_cal_controller.get(), SLOT(handle_input(RawPacket)));
        break;
    case 2:
        connect(&receiver, SIGNAL(raw_sample_received(FilterInput)), kalman_ori_controller.get(), SLOT(handle_input(FilterInput)));
        connect(&receiver, SIGNAL(raw_sample_received(FilterInput)), kalman_pos_controller.get(), SLOT(handle_input(FilterInput)));
        connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), kalman_raw_controller.get(), SLOT(handle_input(RawPacket)));
        break;
    case 3:
        connect(&receiver, SIGNAL(raw_sample_received(FilterInput)), compl_ori_controller.get(), SLOT(handle_input(FilterInput)));
        connect(&receiver, SIGNAL(raw_sample_received(FilterInput)), sim_pos_controller.get(), SLOT(handle_input(FilterInput)));
        break;
    case 4:
        connect(&receiver, SIGNAL(filtered_packet_received(FilteredPacket)), remote_ori_controller.get(), SLOT(handle_input(FilteredPacket)));
        connect(&receiver, SIGNAL(filtered_packet_received(FilteredPacket)), remote_pos_controller.get(), SLOT(handle_input(FilteredPacket)));
        connect(&receiver, SIGNAL(raw_packet_received(RawPacket)), remote_raw_controller.get(), SLOT(handle_input(RawPacket)));
        break;
    }
}
