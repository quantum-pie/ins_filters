#ifndef MAGNCALIBRATIONCONTROLLER_H
#define MAGNCALIBRATIONCONTROLLER_H

#include "views/IBaseView.h"
#include "controllers/direct/observablebase.h"
#include "controllers/direct/runningflag.h"
#include "magncalibrator.h"
#include "packets.h"

#include <QObject>

#include <functional>

class Receiver;
class QPushButton;

class MagnCalibrationController : public QObject,
                                  public ObservableBase<ICalibrationView>,
                                  RunningFlag
{
    Q_OBJECT

public:
    MagnCalibrationController(MagnCalibrator & calibrator, const Receiver * receiver, const QPushButton * calibrate_btn);

    using RunningFlag::is_running;
    using RunningFlag::set_running;

public slots:
    void handle_input(const RawPacket & z);
    void handle_calibrate(bool en);

private:
    std::reference_wrapper<MagnCalibrator> calibration_model;
};

#endif // MAGNCALIBRATIONCONTROLLER_H