#ifndef config_h
#define config_h

namespace iris
{
    // Bind to localhost for use with Unreal Engine application?
    constexpr std::string serverIp() { return "tcp://127.0.0.1"; }

    constexpr std::string sensorDataPubPort() { return ":5555"; }

    constexpr std::string operatorPoseSubPort() { return ":5556"; }

    constexpr std::string operatorPosePubPort() { return ":5557"; }

    constexpr std::string eventDataSubPort() { return ":5558"; }

    constexpr std::string eventDataPubPort() { return ":5559"; }
}

#endif // !config_h
