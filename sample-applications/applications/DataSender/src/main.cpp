

#include "DataSender_component.h"

#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <csignal>
#include "ara/log/logger.h"

#include <thread>
#include <chrono>

static void DataSenderThread()
{
    DataSenderComponent DataSender;
    DataSender.init();

    while (1) {
        DataSender.act();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main()
{
    ara::core::Result<void> initSuccess = ara::core::Initialize();
    if (!initSuccess) {
        return 1;
    }
    ara::log::Logger& logger = ara::log::CreateLogger("Main", "DataSender Application", ara::log::LogLevel::kVerbose);

    logger.LogInfo() << "Starting DataSender...";

    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    std::thread DataThread(DataSenderThread);
    DataThread.join();

    logger.LogInfo() << "Shutting down DataSender...";
    ara::core::Result<void> shutdownSuccess = ara::core::Deinitialize();
    if (!shutdownSuccess) {
        return 1;
    }

    return 0;
}
