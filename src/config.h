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

    constexpr int d1244BroadcastIpCount() { return 4; }

    // UDP broadcast adresses of control system
    constexpr std::array<std::string, 7> d1244BroadcastIps() {
        return {
            "225.1.1.1", //560bytes
            "226.1.1.1", //464bytes
            "227.1.1.1", //368bytes
            "228.1.1.1", //272bytes
            "229.1.1.1",
            "230.1.1.1",
            "226.1.1.3"
        };
    }

    constexpr std::array<int,7> d1244BroadcastPorts() {
        return {
            4321,
            4321,
            4321,
            4321,
            4321,
            4322,
            4323
        };
    }

    constexpr std::string d1244BroadcastIp_MOD1() {
        return "225.1.1.1";
    }

    constexpr std::string d1244BroadcastIp_MOD2() {
        return "226.1.1.1";
    }

    constexpr std::string d1244BroadcastIp_MOD3() {
        return "227.1.1.1";
    }

    constexpr std::string d1244BroadcastIp_MOD4() {
        return "228.1.1.1";
    }

    constexpr std::string d1244BroadcastIp_CAM1() {
        return "229.1.1.1";
    }

    constexpr std::string d1244BroadcastIp_CAM2() {
        return "230.1.1.1";
    }

    constexpr std::string d1244BroadcastIp_SensorFaults() {
        return "226.1.1.3";
    }

    constexpr int d1244BroadcastPort_SensorValues() {
        return 4321;
    }

    constexpr int d1244BroadcastPort_CAM2() {
        return 4322;
    }

    constexpr int d1244BroadcastPort_SensorFaults() {
        return 4323;
    }
}

#endif // !config_h
