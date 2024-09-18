
#ifndef DataSender_COMPONENT_H_
#define DataSender_COMPONENT_H_

#include "apd/sample/distancedatainterface_skeleton.h"

#include "ara/log/ifc/logging.h"
#include <string>
#include <random>
#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

class DataSenderComponent
{
public:

    DataSenderComponent();
    ~DataSenderComponent();

    void init();
    void act();

protected:
    double m_counter;
    apd::sample::skeleton::DistanceDataInterfaceSkeleton* m_skeleton;
    ara::log::Logger& m_logger_ctx3{ara::log::CreateLogger("Comp", "DataSender Component", ara::log::LogLevel::kVerbose)};
};

#endif // DataSender_COMPONENT_H_
