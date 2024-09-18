

#include "DataSender_component.h"
#include "VehicleData.h"

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

DataSenderComponent::DataSenderComponent()
    : m_counter(0)
{
    ara::core::InstanceSpecifier instanceSpec{"DataSender/DataSender/DistanceDataInterface_PPort"};
    m_logger_ctx3.LogInfo() << "Port in Executable Ref: " << instanceSpec.ToString();

    m_skeleton = new apd::sample::skeleton::DistanceDataInterfaceSkeleton(instanceSpec, ara::com::MethodCallProcessingMode::kPoll);

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);
    for(auto const& instanceId : instanceIDs) {
        m_logger_ctx3.LogInfo() << "Service Instance offerd: " << instanceId.ToString();
    }
    m_logger_ctx3.LogDebug() << "object address" << this;
    
}

DataSenderComponent::~DataSenderComponent()
{
    delete m_skeleton;
}

void DataSenderComponent::init()
{
    m_logger_ctx3.LogDebug() << "enter init()";
    
    // Initialize Fields Values before Offering Service.
    m_skeleton->OfferService();
    
    m_logger_ctx3.LogInfo() << "after OfferService() ";
}

void DataSenderComponent::act()
{
    m_logger_ctx3.LogInfo() << "DataSender: " << m_counter;

    // Allocate sample
    auto sampleCounter = m_skeleton->DistanceDataEvent.Allocate();

    //*sampleCounter = m_counter++;
	// To generate the Dummy data for the vehicle use this vbelow code 
	// To run this make initialize the m_counter at 0
    //if(m_counter <= 0) {
    //m_counter = 5;}
    //else{
    //m_counter -= 0.1;}

    // FIX for possible threading problem in vSomeIP which led to SEGFAULT
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    VehicleData::VehicleData Vdata(49152);
    m_counter = Vdata.receiveData();  // Now, m_counter is a double

    static double lastReceived = 0.0;  // Declare lastReceived as double

    // Check if m_counter is non-zero and different from lastReceived
    if (m_counter != 0.0 && m_counter != lastReceived) {
        std::cout << "Received Data: " << m_counter << std::endl;
        lastReceived = m_counter;  // Update the last received data
        m_skeleton->DistanceDataEvent.Send(std::move(m_counter));
    }
}

