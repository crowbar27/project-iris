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

    // UDP broadcast adresses of control system
    constexpr std::array<std::string, 6> d1244BroadcastIps() {
        return {
            "225.1.1.1",
            "226.1.1.1",
            "227.1.1.1",
            "228.1.1.1",
            "229.1.1.1",
            "230.1.1.1",
        };
    }

    constexpr std::array<std::string, 6> d1244BroadcastPorts() {
        return {
            "4321",
            "4321",
            "4321",
            "4321",
            "4321",
            "4322",
        };
    }
}

#endif // !config_h
