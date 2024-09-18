

#ifndef DataReciever_COMPONENT_H_
#define DataReciever_COMPONENT_H_

#include "apd/sample/distancedatainterface_proxy.h"

//#include "ara/log/logging.h"
#include "ara/log/logger.h"
#include "gpio.h"

class DataRecieverComponent
{
public:
    DataRecieverComponent();

    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<apd::sample::proxy::DistanceDataInterfaceProxy::HandleType> handles, ara::com::FindServiceHandle handler);
    
    void init();
    void act();

protected:
    std::shared_ptr<apd::sample::proxy::DistanceDataInterfaceProxy> m_proxy;
    ara::log::Logger& m_logger{ara::log::CreateLogger("Comp", "DataReciever Component", ara::log::LogLevel::kVerbose)};

private:
    std::unique_ptr<GPIO> m_gpio;
};

#endif // DataReciever_COMPONENT_H_
