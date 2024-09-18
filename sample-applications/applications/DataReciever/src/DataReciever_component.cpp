#include "DataReciever_component.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>


#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "ara/com/internal/skeleton/mutable_field_dispatcher.h"



using namespace ara::log;

double data=0;

DataRecieverComponent::DataRecieverComponent()
    : m_proxy(nullptr), m_gpio(nullptr)
{
    m_logger.LogDebug() << "object address" << this;
}

void DataRecieverComponent::init()
{
    m_logger.LogDebug() << "enter init()";

    ara::core::InstanceSpecifier portSpecifier{"DataReciever/DataReciever/DistanceDataInterface_RPort"};
    m_logger.LogInfo() << "Port In Executable Ref: " << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        throw std::runtime_error{"No InstanceIdentifiers resolved from provided InstanceSpecifier"};
    }
    
    m_logger.LogInfo() << "Searching for Serivce Instance:" << instanceIDs[0].ToString();

    apd::sample::proxy::DistanceDataInterfaceProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<apd::sample::proxy::DistanceDataInterfaceProxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            DataRecieverComponent::serviceAvailabilityCallback(std::move(handles), handler);
        },
        instanceIDs[0]);

    // Initialize GPIO
    m_gpio = std::make_unique<GPIO>();

    m_logger.LogInfo() << "init() exit";
}

void DataRecieverComponent::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<apd::sample::proxy::DistanceDataInterfaceProxy::HandleType> handles, ara::com::FindServiceHandle handler)
{
    for (auto it : handles) {
        m_logger.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }

    if (handles.size() > 0) {
        if (nullptr == m_proxy) {
            m_proxy = std::make_shared<apd::sample::proxy::DistanceDataInterfaceProxy>(handles[0]);
            m_logger.LogInfo() << "Created proxy from handle with instance: "
                               << m_proxy->GetHandle().GetInstanceId().ToString();
       }
    }
}

void DataRecieverComponent::act()
{
    m_logger.LogInfo() << "DataReciever";
    

    if (nullptr != m_proxy) {
        if (m_proxy->DistanceDataEvent.IsSubscribed()) {

            /*LogStream logMsg{m_logger.LogVerbose()};

            auto callback = [&logMsg](auto sample) {
                logMsg << "Polling: DataSender ";
                logMsg << " data: ";
                logMsg << *sample;
                data = *sample; // Assuming sample is an integer
                
                logMsg.Flush();
            };*/
            
            auto callback = [&data](auto sample) {
                
                data = *sample; // Assuming sample is an integer
                
            };

            m_proxy->DistanceDataEvent.GetNewSamples(callback); 
        } else {
            m_proxy->DistanceDataEvent.Subscribe(3);
            m_logger.LogInfo() << "DistanceDataEvent subscription complete";
             /*LogStream logMsg{m_logger.LogVerbose()};

            auto callback = [&logMsg](auto sample) {
                logMsg << "Polling: DataSender ";
                logMsg << " data: ";
                logMsg << *sample;
                logMsg.Flush();
            };

            m_proxy->DistanceDataEvent.GetNewSamples(callback); */
            auto callback = [&data](auto sample) {
                
                data = *sample; // Assuming sample is an integer
                
            };

            m_proxy->DistanceDataEvent.GetNewSamples(callback);
            
        }
        
        if (data <= 1.5) {
                    m_logger.LogInfo() << "Polling: DataSender data: " << data << " Critical Stop Vehicle !";
                    // Set GPIO value to 1 (high)
                    m_gpio->setValue_17(1);
                    m_gpio->setValue_27(0);
                    m_gpio->setValue_22(0);
        } 
        else if (data > 1.5 && data <= 3.5) {
                    m_logger.LogInfo() << "Polling: DataSender data: " << data << " Warning ! Maintain Safe Distance !";
                    // Set GPIO value to 1 (high)
                    m_gpio->setValue_17(0);
                    m_gpio->setValue_27(1);
                    m_gpio->setValue_22(0);
        } else {
                    m_logger.LogInfo() << "Polling: DataSender data: " << data << " Safe Distance";
                    // Set GPIO value to  (high)
                    m_gpio->setValue_17(0);
                    m_gpio->setValue_27(0);
                    m_gpio->setValue_22(1);
        }
    }
    
    //GPIO CODE to turn on LED at GPIO_PIN_16
    // Set GPIO value to 1 (high)
    //if (m_gpio) {
    //    m_gpio->setValue(1);
    //}
}
