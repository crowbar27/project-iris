#ifndef message_types_h
#define message_types_h

#include <array>
#include <string>
#include <vector>

struct TrussStructureMessage {

    typedef double InternalFormat;

    static constexpr std::string envelope() { return "TS"; }

    static const size_t sensor_cnt = 244;

    enum class SensorID {
        MOD_1_NUMVARS,
        MOD_1_TIMESTAMP,
        MOD_1_ELONG_1,
        MOD_1_ELONG_2,
        MOD_1_ELONG_3,
        MOD_1_ELONG_4,
        MOD_1_ELONG_5,
        MOD_1_ELONG_6,
        MOD_1_ELONG_7,
        MOD_1_ELONG_8,
        MOD_1_ELONG_9,
        MOD_1_ELONG_10,
        MOD_1_ELONG_11,
        MOD_1_ELONG_12,
        MOD_1_ELONG_13,
        MOD_1_ELONG_14,
        MOD_1_ELONG_15,
        MOD_1_ELONG_16,
        MOD_1_ELONG_17,
        MOD_1_ELONG_18,
        MOD_1_ELONG_19,
        MOD_1_ELONG_20,
        MOD_1_ELONG_21,
        MOD_1_ELONG_22,
        MOD_1_ELONG_23,
        MOD_1_ELONG_24,
        MOD_1_ELONG_25,
        MOD_1_ELONG_26,
        MOD_1_ELONG_27,
        MOD_1_ELONG_28,
        MOD_1_ELONG_29,
        MOD_1_ELONG_30,
        MOD_1_ELONG_31,
        MOD_1_ELONG_32,
        MOD_1_PRESSURE_1_X,
        MOD_1_PRESSURE_1_Y,
        MOD_1_PRESSURE_2_X,
        MOD_1_PRESSURE_2_Y,
        MOD_1_PRESSURE_3_X,
        MOD_1_PRESSURE_3_Y,
        MOD_1_PRESSURE_4_X,
        MOD_1_PRESSURE_4_Y,
        MOD_1_PRESSURE_5_X,
        MOD_1_PRESSURE_5_Y,
        MOD_1_PRESSURE_6_X,
        MOD_1_PRESSURE_6_Y,
        MOD_1_PRESSURE_7_X,
        MOD_1_PRESSURE_7_Y,
        MOD_1_PRESSURE_8_X,
        MOD_1_PRESSURE_8_Y,
        MOD_1_PRESSURE_9_X,
        MOD_1_PRESSURE_9_Y,
        MOD_1_PRESSURE_10_X,
        MOD_1_PRESSURE_10_Y,
        MOD_1_PRESSURE_11_X,
        MOD_1_PRESSURE_11_Y,
        MOD_1_PRESSURE_12_X,
        MOD_1_PRESSURE_12_Y,
        MOD_1_POSENC_1,
        MOD_1_POSENC_2,
        MOD_1_POSENC_3,
        MOD_1_POSENC_4,
        MOD_1_POSENC_5,
        MOD_1_POSENC_6,
        MOD_1_POSENC_7,
        MOD_1_POSENC_8,
        MOD_1_POSENC_9,
        MOD_1_POSENC_10,
        MOD_1_POSENC_11,
        MOD_1_POSENC_12,
        MOD_2_NUMVARS,
        MOD_2_TIMESTAMP,
        MOD_2_ELONG_1,
        MOD_2_ELONG_2,
        MOD_2_ELONG_3,
        MOD_2_ELONG_4,
        MOD_2_ELONG_5,
        MOD_2_ELONG_6,
        MOD_2_ELONG_7,
        MOD_2_ELONG_8,
        MOD_2_ELONG_9,
        MOD_2_ELONG_10,
        MOD_2_ELONG_11,
        MOD_2_ELONG_12,
        MOD_2_ELONG_13,
        MOD_2_ELONG_14,
        MOD_2_ELONG_15,
        MOD_2_ELONG_16,
        MOD_2_ELONG_17,
        MOD_2_ELONG_18,
        MOD_2_ELONG_19,
        MOD_2_ELONG_20,
        MOD_2_ELONG_21,
        MOD_2_ELONG_22,
        MOD_2_ELONG_23,
        MOD_2_ELONG_24,
        MOD_2_ELONG_25,
        MOD_2_ELONG_26,
        MOD_2_ELONG_27,
        MOD_2_ELONG_28,
        MOD_2_ELONG_29,
        MOD_2_ELONG_30,
        MOD_2_ELONG_31,
        MOD_2_ELONG_32,
        MOD_2_PRESSURE_13_X,
        MOD_2_PRESSURE_13_Y,
        MOD_2_PRESSURE_14_X,
        MOD_2_PRESSURE_14_Y,
        MOD_2_PRESSURE_15_X,
        MOD_2_PRESSURE_15_Y,
        MOD_2_PRESSURE_16_X,
        MOD_2_PRESSURE_16_Y,
        MOD_2_PRESSURE_17_X,
        MOD_2_PRESSURE_17_Y,
        MOD_2_PRESSURE_18_X,
        MOD_2_PRESSURE_18_Y,
        MOD_2_PRESSURE_19_X,
        MOD_2_PRESSURE_19_Y,
        MOD_2_PRESSURE_20_X,
        MOD_2_PRESSURE_20_Y,
        MOD_2_POSENC_13,
        MOD_2_POSENC_14,
        MOD_2_POSENC_15,
        MOD_2_POSENC_16,
        MOD_2_POSENC_17,
        MOD_2_POSENC_18,
        MOD_2_POSENC_19,
        MOD_2_POSENC_20,
        MOD_3_NUMVARS,
        MOD_3_TIMESTAMP,
        MOD_3_ELONG_1,
        MOD_3_ELONG_2,
        MOD_3_ELONG_3,
        MOD_3_ELONG_4,
        MOD_3_ELONG_5,
        MOD_3_ELONG_6,
        MOD_3_ELONG_7,
        MOD_3_ELONG_8,
        MOD_3_ELONG_9,
        MOD_3_ELONG_10,
        MOD_3_ELONG_11,
        MOD_3_ELONG_12,
        MOD_3_ELONG_13,
        MOD_3_ELONG_14,
        MOD_3_ELONG_15,
        MOD_3_ELONG_16,
        MOD_3_ELONG_17,
        MOD_3_ELONG_18,
        MOD_3_ELONG_19,
        MOD_3_ELONG_20,
        MOD_3_ELONG_21,
        MOD_3_ELONG_22,
        MOD_3_ELONG_23,
        MOD_3_ELONG_24,
        MOD_3_ELONG_25,
        MOD_3_ELONG_26,
        MOD_3_ELONG_27,
        MOD_3_ELONG_28,
        MOD_3_ELONG_29,
        MOD_3_ELONG_30,
        MOD_3_ELONG_31,
        MOD_3_ELONG_32,
        MOD_3_PRESSURE_21X,
        MOD_3_PRESSURE_21Y,
        MOD_3_PRESSURE_22X,
        MOD_3_PRESSURE_22Y,
        MOD_3_PRESSURE_23X,
        MOD_3_PRESSURE_23Y,
        MOD_3_PRESSURE_24X,
        MOD_3_PRESSURE_24Y,
        MOD_3_POSENC_1,
        MOD_3_POSENC_2,
        MOD_3_POSENC_3,
        MOD_3_POSENC_4,
        MOD_4_NUMVARS,
        MOD_4_TIMESTAMP,
        MOD_4_ELONG_1,
        MOD_4_ELONG_2,
        MOD_4_ELONG_3,
        MOD_4_ELONG_4,
        MOD_4_ELONG_5,
        MOD_4_ELONG_6,
        MOD_4_ELONG_7,
        MOD_4_ELONG_8,
        MOD_4_ELONG_9,
        MOD_4_ELONG_10,
        MOD_4_ELONG_11,
        MOD_4_ELONG_12,
        MOD_4_ELONG_13,
        MOD_4_ELONG_14,
        MOD_4_ELONG_15,
        MOD_4_ELONG_16,
        MOD_4_ELONG_17,
        MOD_4_ELONG_18,
        MOD_4_ELONG_19,
        MOD_4_ELONG_20,
        MOD_4_ELONG_21,
        MOD_4_ELONG_22,
        MOD_4_ELONG_23,
        MOD_4_ELONG_24,
        MOD_4_ELONG_25,
        MOD_4_ELONG_26,
        MOD_4_ELONG_27,
        MOD_4_ELONG_28,
        MOD_4_ELONG_29,
        MOD_4_ELONG_30,
        MOD_4_ELONG_31,
        MOD_4_ELONG_32,
        CAM_1_NUMVARS,
        CAM_1_TIMESTAMP,
        CAM_1_1_SW_Z,
        CAM_2_1_SW_X,
        CAM_3_1_NW_Z,
        CAM_4_1_NW_X,
        CAM_5_2_SW_Z,
        CAM_6_2_SW_X,
        CAM_7_2_NW_Z,
        CAM_8_2_NW_X,
        CAM_9_3_SW_Z,
        CAM_10_3_SW_X,
        CAM_11_3_NW_Z,
        CAM_12_3_NW_X,
        CAM_13_4_SW_Z,
        CAM_14_4_SW_X,
        CAM_15_4_NW_Z,
        CAM_16_4_NW_X,
        CAM_2_NUMVARS,
        CAM_2_TIMESTAMP,
        CAM_17_1_NW_Z,
        CAM_18_1_NW_Y,
        CAM_19_1_NO_Z,
        CAM_20_1_NO_Y,
        CAM_21_2_NW_Z,
        CAM_22_2_NW_Y,
        CAM_23_2_NO_Z,
        CAM_24_2_NO_Y,
        CAM_25_3_NW_Z,
        CAM_26_3_NW_Y,
        CAM_27_3_NO_Z,
        CAM_28_3_NO_Y,
        CAM_29_4_NW_Z,
        CAM_30_4_NW_Y,
        CAM_31_4_NO_Z,
        CAM_32_4_NO_Y
    };

    static std::string getLabel(size_t id) {
        static const auto sensor_labels = std::to_array<std::string>({
        "mod_1_numvars",
        "mod_1_timestamp",
        "mod_1_elong_1",
        "mod_1_elong_2",
        "mod_1_elong_3",
        "mod_1_elong_4",
        "mod_1_elong_5",
        "mod_1_elong_6",
        "mod_1_elong_7",
        "mod_1_elong_8",
        "mod_1_elong_9",
        "mod_1_elong_10",
        "mod_1_elong_11",
        "mod_1_elong_12",
        "mod_1_elong_13",
        "mod_1_elong_14",
        "mod_1_elong_15",
        "mod_1_elong_16",
        "mod_1_elong_17",
        "mod_1_elong_18",
        "mod_1_elong_19",
        "mod_1_elong_20",
        "mod_1_elong_21",
        "mod_1_elong_22",
        "mod_1_elong_23",
        "mod_1_elong_24",
        "mod_1_elong_25",
        "mod_1_elong_26",
        "mod_1_elong_27",
        "mod_1_elong_28",
        "mod_1_elong_29",
        "mod_1_elong_30",
        "mod_1_elong_31",
        "mod_1_elong_32",
        "mod_1_pressure_1_x",
        "mod_1_pressure_1_y",
        "mod_1_pressure_2_x",
        "mod_1_pressure_2_y",
        "mod_1_pressure_3_x",
        "mod_1_pressure_3_y",
        "mod_1_pressure_4_x",
        "mod_1_pressure_4_y",
        "mod_1_pressure_5_x",
        "mod_1_pressure_5_y",
        "mod_1_pressure_6_x",
        "mod_1_pressure_6_y",
        "mod_1_pressure_7_x",
        "mod_1_pressure_7_y",
        "mod_1_pressure_8_x",
        "mod_1_pressure_8_y",
        "mod_1_pressure_9_x",
        "mod_1_pressure_9_y",
        "mod_1_pressure_10_x",
        "mod_1_pressure_10_y",
        "mod_1_pressure_11_x",
        "mod_1_pressure_11_y",
        "mod_1_pressure_12_x",
        "mod_1_pressure_12_y",
        "mod_1_posenc_1",
        "mod_1_posenc_2",
        "mod_1_posenc_3",
        "mod_1_posenc_4",
        "mod_1_posenc_5",
        "mod_1_posenc_6",
        "mod_1_posenc_7",
        "mod_1_posenc_8",
        "mod_1_posenc_9",
        "mod_1_posenc_10",
        "mod_1_posenc_11",
        "mod_1_posenc_12",
        "mod_2_numvars",
        "mod_2_timestamp",
        "mod_2_elong_1",
        "mod_2_elong_2",
        "mod_2_elong_3",
        "mod_2_elong_4",
        "mod_2_elong_5",
        "mod_2_elong_6",
        "mod_2_elong_7",
        "mod_2_elong_8",
        "mod_2_elong_9",
        "mod_2_elong_10",
        "mod_2_elong_11",
        "mod_2_elong_12",
        "mod_2_elong_13",
        "mod_2_elong_14",
        "mod_2_elong_15",
        "mod_2_elong_16",
        "mod_2_elong_17",
        "mod_2_elong_18",
        "mod_2_elong_19",
        "mod_2_elong_20",
        "mod_2_elong_21",
        "mod_2_elong_22",
        "mod_2_elong_23",
        "mod_2_elong_24",
        "mod_2_elong_25",
        "mod_2_elong_26",
        "mod_2_elong_27",
        "mod_2_elong_28",
        "mod_2_elong_29",
        "mod_2_elong_30",
        "mod_2_elong_31",
        "mod_2_elong_32",
        "mod_2_pressure_13_x",
        "mod_2_pressure_13_y",
        "mod_2_pressure_14_x",
        "mod_2_pressure_14_y",
        "mod_2_pressure_15_x",
        "mod_2_pressure_15_y",
        "mod_2_pressure_16_x",
        "mod_2_pressure_16_y",
        "mod_2_pressure_17_x",
        "mod_2_pressure_17_y",
        "mod_2_pressure_18_x",
        "mod_2_pressure_18_y",
        "mod_2_pressure_19_x",
        "mod_2_pressure_19_y",
        "mod_2_pressure_20_x",
        "mod_2_pressure_20_y",
        "mod_2_posenc_13",
        "mod_2_posenc_14",
        "mod_2_posenc_15",
        "mod_2_posenc_16",
        "mod_2_posenc_17",
        "mod_2_posenc_18",
        "mod_2_posenc_19",
        "mod_2_posenc_20",
        "mod_3_numvars",
        "mod_3_timestamp",
        "mod_3_elong_1",
        "mod_3_elong_2",
        "mod_3_elong_3",
        "mod_3_elong_4",
        "mod_3_elong_5",
        "mod_3_elong_6",
        "mod_3_elong_7",
        "mod_3_elong_8",
        "mod_3_elong_9",
        "mod_3_elong_10",
        "mod_3_elong_11",
        "mod_3_elong_12",
        "mod_3_elong_13",
        "mod_3_elong_14",
        "mod_3_elong_15",
        "mod_3_elong_16",
        "mod_3_elong_17",
        "mod_3_elong_18",
        "mod_3_elong_19",
        "mod_3_elong_20",
        "mod_3_elong_21",
        "mod_3_elong_22",
        "mod_3_elong_23",
        "mod_3_elong_24",
        "mod_3_elong_25",
        "mod_3_elong_26",
        "mod_3_elong_27",
        "mod_3_elong_28",
        "mod_3_elong_29",
        "mod_3_elong_30",
        "mod_3_elong_31",
        "mod_3_elong_32",
        "mod_3_pressure_21x",
        "mod_3_pressure_21y",
        "mod_3_pressure_22x",
        "mod_3_pressure_22y",
        "mod_3_pressure_23x",
        "mod_3_pressure_23y",
        "mod_3_pressure_24x",
        "mod_3_pressure_24y",
        "mod_3_posenc_1",
        "mod_3_posenc_2",
        "mod_3_posenc_3",
        "mod_3_posenc_4",
        "mod_4_numvars",
        "mod_4_timestamp",
        "mod_4_elong_1",
        "mod_4_elong_2",
        "mod_4_elong_3",
        "mod_4_elong_4",
        "mod_4_elong_5",
        "mod_4_elong_6",
        "mod_4_elong_7",
        "mod_4_elong_8",
        "mod_4_elong_9",
        "mod_4_elong_10",
        "mod_4_elong_11",
        "mod_4_elong_12",
        "mod_4_elong_13",
        "mod_4_elong_14",
        "mod_4_elong_15",
        "mod_4_elong_16",
        "mod_4_elong_17",
        "mod_4_elong_18",
        "mod_4_elong_19",
        "mod_4_elong_20",
        "mod_4_elong_21",
        "mod_4_elong_22",
        "mod_4_elong_23",
        "mod_4_elong_24",
        "mod_4_elong_25",
        "mod_4_elong_26",
        "mod_4_elong_27",
        "mod_4_elong_28",
        "mod_4_elong_29",
        "mod_4_elong_30",
        "mod_4_elong_31",
        "mod_4_elong_32",
        "cam_1_numvars",
        "cam_1_timestamp",
        "cam_1_1_sw_z",
        "cam_2_1_sw_x",
        "cam_3_1_nw_z",
        "cam_4_1_nw_x",
        "cam_5_2_sw_z",
        "cam_6_2_sw_x",
        "cam_7_2_nw_z",
        "cam_8_2_nw_x",
        "cam_9_3_sw_z",
        "cam_10_3_sw_x",
        "cam_11_3_nw_z",
        "cam_12_3_nw_x",
        "cam_13_4_sw_z",
        "cam_14_4_sw_x",
        "cam_15_4_nw_z",
        "cam_16_4_nw_x",
        "cam_2_numvars",
        "cam_2_timestamp",
        "cam_17_1_nw_z",
        "cam_18_1_nw_y",
        "cam_19_1_no_z",
        "cam_20_1_no_y",
        "cam_21_2_nw_z",
        "cam_22_2_nw_y",
        "cam_23_2_no_z",
        "cam_24_2_no_y",
        "cam_25_3_nw_z",
        "cam_26_3_nw_y",
        "cam_27_3_no_z",
        "cam_28_3_no_y",
        "cam_29_4_nw_z",
        "cam_30_4_nw_y",
        "cam_31_4_no_z",
        "cam_32_4_no_y"
            });

        return sensor_labels[id];
    }

    static std::string getLabel(SensorID id) {
        return getLabel(static_cast<size_t>(id));
    }

    enum class SensorType {
        STRAIN_GAUGE,
        HYDRAULIC_PRESSURE,
        DISTANCE_METER,
        OPTICAL,
        OTHER
    };

    static SensorType getSensorType(size_t id) {
        static const auto sensor_types = std::array<SensorType, sensor_cnt>({
            SensorType::OTHER,
            SensorType::OTHER,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::OTHER,
        SensorType::OTHER,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::OTHER,
        SensorType::OTHER,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::HYDRAULIC_PRESSURE,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::DISTANCE_METER,
        SensorType::OTHER,
        SensorType::OTHER,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::STRAIN_GAUGE,
        SensorType::OTHER,
        SensorType::OTHER,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OTHER,
        SensorType::OTHER,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
        SensorType::OPTICAL,
            }
        );

        return sensor_types[id];
    }

    struct RawSensorData
    {
        double data[sensor_cnt];

        constexpr double getValue(SensorID id) { return data[static_cast<size_t>(id)]; }
    };

};

struct TrussStructureFaultMessage {
    typedef double InternalFormat;

    static constexpr std::string envelope() { return "TSF"; }

    static const size_t fault_channel_cnt = 2;

    static const size_t max_fault_class = 128;

    static std::vector<TrussStructureMessage::SensorID> getSensorIDs(size_t fault_class) {
        static const auto sensor_ids = std::to_array<std::vector<TrussStructureMessage::SensorID>>({
{TrussStructureMessage::SensorID::MOD_1_ELONG_25,TrussStructureMessage::SensorID::MOD_1_ELONG_26,TrussStructureMessage::SensorID::MOD_1_ELONG_27,TrussStructureMessage::SensorID::MOD_1_ELONG_28},
{TrussStructureMessage::SensorID::MOD_1_ELONG_1,TrussStructureMessage::SensorID::MOD_1_ELONG_2,TrussStructureMessage::SensorID::MOD_1_ELONG_3,TrussStructureMessage::SensorID::MOD_1_ELONG_4},
{TrussStructureMessage::SensorID::MOD_1_ELONG_9,TrussStructureMessage::SensorID::MOD_1_ELONG_10,TrussStructureMessage::SensorID::MOD_1_ELONG_11,TrussStructureMessage::SensorID::MOD_1_ELONG_12},
{TrussStructureMessage::SensorID::MOD_1_ELONG_17,TrussStructureMessage::SensorID::MOD_1_ELONG_18,TrussStructureMessage::SensorID::MOD_1_ELONG_19,TrussStructureMessage::SensorID::MOD_1_ELONG_20},
{TrussStructureMessage::SensorID::MOD_1_ELONG_29,TrussStructureMessage::SensorID::MOD_1_ELONG_30},
{TrussStructureMessage::SensorID::MOD_1_ELONG_7,TrussStructureMessage::SensorID::MOD_1_ELONG_8},
{TrussStructureMessage::SensorID::MOD_1_ELONG_5,TrussStructureMessage::SensorID::MOD_1_ELONG_6},
{TrussStructureMessage::SensorID::MOD_1_ELONG_15,TrussStructureMessage::SensorID::MOD_1_ELONG_16},
{TrussStructureMessage::SensorID::MOD_1_ELONG_13,TrussStructureMessage::SensorID::MOD_1_ELONG_14},
{TrussStructureMessage::SensorID::MOD_1_ELONG_23,TrussStructureMessage::SensorID::MOD_1_ELONG_24},
{TrussStructureMessage::SensorID::MOD_1_ELONG_21,TrussStructureMessage::SensorID::MOD_1_ELONG_22},
{TrussStructureMessage::SensorID::MOD_1_ELONG_31,TrussStructureMessage::SensorID::MOD_1_ELONG_32},
{TrussStructureMessage::SensorID::MOD_2_ELONG_25,TrussStructureMessage::SensorID::MOD_2_ELONG_26,TrussStructureMessage::SensorID::MOD_2_ELONG_27,TrussStructureMessage::SensorID::MOD_2_ELONG_28},
{TrussStructureMessage::SensorID::MOD_2_ELONG_1,TrussStructureMessage::SensorID::MOD_2_ELONG_2,TrussStructureMessage::SensorID::MOD_2_ELONG_3,TrussStructureMessage::SensorID::MOD_2_ELONG_4},
{TrussStructureMessage::SensorID::MOD_2_ELONG_9,TrussStructureMessage::SensorID::MOD_2_ELONG_10,TrussStructureMessage::SensorID::MOD_2_ELONG_11,TrussStructureMessage::SensorID::MOD_2_ELONG_12},
{TrussStructureMessage::SensorID::MOD_2_ELONG_17,TrussStructureMessage::SensorID::MOD_2_ELONG_18,TrussStructureMessage::SensorID::MOD_2_ELONG_19,TrussStructureMessage::SensorID::MOD_2_ELONG_20},
{TrussStructureMessage::SensorID::MOD_2_ELONG_29,TrussStructureMessage::SensorID::MOD_2_ELONG_30},
{TrussStructureMessage::SensorID::MOD_2_ELONG_7,TrussStructureMessage::SensorID::MOD_2_ELONG_8},
{TrussStructureMessage::SensorID::MOD_2_ELONG_5,TrussStructureMessage::SensorID::MOD_2_ELONG_6},
{TrussStructureMessage::SensorID::MOD_2_ELONG_15,TrussStructureMessage::SensorID::MOD_2_ELONG_16},
{TrussStructureMessage::SensorID::MOD_2_ELONG_13,TrussStructureMessage::SensorID::MOD_2_ELONG_14},
{TrussStructureMessage::SensorID::MOD_2_ELONG_23,TrussStructureMessage::SensorID::MOD_2_ELONG_24},
{TrussStructureMessage::SensorID::MOD_2_ELONG_21,TrussStructureMessage::SensorID::MOD_2_ELONG_22},
{TrussStructureMessage::SensorID::MOD_2_ELONG_31,TrussStructureMessage::SensorID::MOD_2_ELONG_32},
{TrussStructureMessage::SensorID::MOD_3_ELONG_25,TrussStructureMessage::SensorID::MOD_3_ELONG_26,TrussStructureMessage::SensorID::MOD_3_ELONG_27,TrussStructureMessage::SensorID::MOD_3_ELONG_28},
{TrussStructureMessage::SensorID::MOD_3_ELONG_1,TrussStructureMessage::SensorID::MOD_3_ELONG_2,TrussStructureMessage::SensorID::MOD_3_ELONG_3,TrussStructureMessage::SensorID::MOD_3_ELONG_4},
{TrussStructureMessage::SensorID::MOD_3_ELONG_9,TrussStructureMessage::SensorID::MOD_3_ELONG_10,TrussStructureMessage::SensorID::MOD_3_ELONG_11,TrussStructureMessage::SensorID::MOD_3_ELONG_12},
{TrussStructureMessage::SensorID::MOD_3_ELONG_17,TrussStructureMessage::SensorID::MOD_3_ELONG_18,TrussStructureMessage::SensorID::MOD_3_ELONG_19,TrussStructureMessage::SensorID::MOD_3_ELONG_20},
{TrussStructureMessage::SensorID::MOD_3_ELONG_29,TrussStructureMessage::SensorID::MOD_3_ELONG_30},
{TrussStructureMessage::SensorID::MOD_3_ELONG_7,TrussStructureMessage::SensorID::MOD_3_ELONG_8},
{TrussStructureMessage::SensorID::MOD_3_ELONG_5,TrussStructureMessage::SensorID::MOD_3_ELONG_6},
{TrussStructureMessage::SensorID::MOD_3_ELONG_15,TrussStructureMessage::SensorID::MOD_3_ELONG_16},
{TrussStructureMessage::SensorID::MOD_3_ELONG_13,TrussStructureMessage::SensorID::MOD_3_ELONG_14},
{TrussStructureMessage::SensorID::MOD_3_ELONG_23,TrussStructureMessage::SensorID::MOD_3_ELONG_24},
{TrussStructureMessage::SensorID::MOD_3_ELONG_21,TrussStructureMessage::SensorID::MOD_3_ELONG_22},
{TrussStructureMessage::SensorID::MOD_3_ELONG_31,TrussStructureMessage::SensorID::MOD_3_ELONG_32},
{TrussStructureMessage::SensorID::MOD_4_ELONG_25,TrussStructureMessage::SensorID::MOD_4_ELONG_26,TrussStructureMessage::SensorID::MOD_4_ELONG_27,TrussStructureMessage::SensorID::MOD_4_ELONG_28},
{TrussStructureMessage::SensorID::MOD_4_ELONG_1,TrussStructureMessage::SensorID::MOD_4_ELONG_2,TrussStructureMessage::SensorID::MOD_4_ELONG_3,TrussStructureMessage::SensorID::MOD_4_ELONG_4},
{TrussStructureMessage::SensorID::MOD_4_ELONG_9,TrussStructureMessage::SensorID::MOD_4_ELONG_10,TrussStructureMessage::SensorID::MOD_4_ELONG_11,TrussStructureMessage::SensorID::MOD_4_ELONG_12},
{TrussStructureMessage::SensorID::MOD_4_ELONG_17,TrussStructureMessage::SensorID::MOD_4_ELONG_18,TrussStructureMessage::SensorID::MOD_4_ELONG_19,TrussStructureMessage::SensorID::MOD_4_ELONG_20},
{TrussStructureMessage::SensorID::MOD_4_ELONG_29,TrussStructureMessage::SensorID::MOD_4_ELONG_30},
{TrussStructureMessage::SensorID::MOD_4_ELONG_7,TrussStructureMessage::SensorID::MOD_4_ELONG_8},
{TrussStructureMessage::SensorID::MOD_4_ELONG_5,TrussStructureMessage::SensorID::MOD_4_ELONG_6},
{TrussStructureMessage::SensorID::MOD_4_ELONG_15,TrussStructureMessage::SensorID::MOD_4_ELONG_16},
{TrussStructureMessage::SensorID::MOD_4_ELONG_13,TrussStructureMessage::SensorID::MOD_4_ELONG_14},
{TrussStructureMessage::SensorID::MOD_4_ELONG_23,TrussStructureMessage::SensorID::MOD_4_ELONG_24},
{TrussStructureMessage::SensorID::MOD_4_ELONG_21,TrussStructureMessage::SensorID::MOD_4_ELONG_22},
{TrussStructureMessage::SensorID::MOD_4_ELONG_31,TrussStructureMessage::SensorID::MOD_4_ELONG_32},
{TrussStructureMessage::SensorID::CAM_1_1_SW_Z},
{TrussStructureMessage::SensorID::CAM_2_1_SW_X},
{TrussStructureMessage::SensorID::CAM_3_1_NW_Z},
{TrussStructureMessage::SensorID::CAM_4_1_NW_X},
{TrussStructureMessage::SensorID::CAM_5_2_SW_Z},
{TrussStructureMessage::SensorID::CAM_6_2_SW_X},
{TrussStructureMessage::SensorID::CAM_7_2_NW_Z},
{TrussStructureMessage::SensorID::CAM_8_2_NW_X},
{TrussStructureMessage::SensorID::CAM_9_3_SW_Z},
{TrussStructureMessage::SensorID::CAM_10_3_SW_X},
{TrussStructureMessage::SensorID::CAM_11_3_NW_Z},
{TrussStructureMessage::SensorID::CAM_12_3_NW_X},
{TrussStructureMessage::SensorID::CAM_13_4_SW_Z},
{TrussStructureMessage::SensorID::CAM_14_4_SW_X},
{TrussStructureMessage::SensorID::CAM_15_4_NW_Z},
{TrussStructureMessage::SensorID::CAM_16_4_NW_X},
{TrussStructureMessage::SensorID::CAM_17_1_NW_Z},
{TrussStructureMessage::SensorID::CAM_18_1_NW_Y},
{TrussStructureMessage::SensorID::CAM_19_1_NO_Z},
{TrussStructureMessage::SensorID::CAM_20_1_NO_Y},
{TrussStructureMessage::SensorID::CAM_21_2_NW_Z},
{TrussStructureMessage::SensorID::CAM_22_2_NW_Y},
{TrussStructureMessage::SensorID::CAM_23_2_NO_Z},
{TrussStructureMessage::SensorID::CAM_24_2_NO_Y},
{TrussStructureMessage::SensorID::CAM_25_3_NW_Z},
{TrussStructureMessage::SensorID::CAM_26_3_NW_Y},
{TrussStructureMessage::SensorID::CAM_27_3_NO_Z},
{TrussStructureMessage::SensorID::CAM_28_3_NO_Y},
{TrussStructureMessage::SensorID::CAM_29_4_NW_Z},
{TrussStructureMessage::SensorID::CAM_30_4_NW_Y},
{TrussStructureMessage::SensorID::CAM_31_4_NO_Z},
{TrussStructureMessage::SensorID::CAM_32_4_NO_Y},
{TrussStructureMessage::SensorID::MOD_1_POSENC_1},
{TrussStructureMessage::SensorID::MOD_1_POSENC_2},
{TrussStructureMessage::SensorID::MOD_1_POSENC_3},
{TrussStructureMessage::SensorID::MOD_1_POSENC_4},
{TrussStructureMessage::SensorID::MOD_1_POSENC_5},
{TrussStructureMessage::SensorID::MOD_1_POSENC_6},
{TrussStructureMessage::SensorID::MOD_1_POSENC_7},
{TrussStructureMessage::SensorID::MOD_1_POSENC_8},
{TrussStructureMessage::SensorID::MOD_1_POSENC_9},
{TrussStructureMessage::SensorID::MOD_1_POSENC_10},
{TrussStructureMessage::SensorID::MOD_1_POSENC_11},
{TrussStructureMessage::SensorID::MOD_1_POSENC_12},
{TrussStructureMessage::SensorID::MOD_2_POSENC_13},
{TrussStructureMessage::SensorID::MOD_2_POSENC_14},
{TrussStructureMessage::SensorID::MOD_2_POSENC_15},
{TrussStructureMessage::SensorID::MOD_2_POSENC_16},
{TrussStructureMessage::SensorID::MOD_2_POSENC_17},
{TrussStructureMessage::SensorID::MOD_2_POSENC_18},
{TrussStructureMessage::SensorID::MOD_2_POSENC_19},
{TrussStructureMessage::SensorID::MOD_2_POSENC_20},
{TrussStructureMessage::SensorID::MOD_3_POSENC_1},
{TrussStructureMessage::SensorID::MOD_3_POSENC_2},
{TrussStructureMessage::SensorID::MOD_3_POSENC_3},
{TrussStructureMessage::SensorID::MOD_3_POSENC_4},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_1_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_1_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_2_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_2_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_3_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_3_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_4_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_4_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_5_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_5_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_6_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_6_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_7_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_7_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_8_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_8_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_9_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_9_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_10_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_10_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_11_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_11_Y},
{TrussStructureMessage::SensorID::MOD_1_PRESSURE_12_X,TrussStructureMessage::SensorID::MOD_1_PRESSURE_12_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_13_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_13_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_14_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_14_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_15_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_15_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_16_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_16_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_17_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_17_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_18_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_18_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_19_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_19_Y},
{TrussStructureMessage::SensorID::MOD_2_PRESSURE_20_X,TrussStructureMessage::SensorID::MOD_2_PRESSURE_20_Y},
{TrussStructureMessage::SensorID::MOD_3_PRESSURE_21X,TrussStructureMessage::SensorID::MOD_3_PRESSURE_21Y},
{TrussStructureMessage::SensorID::MOD_3_PRESSURE_22X,TrussStructureMessage::SensorID::MOD_3_PRESSURE_22Y},
{TrussStructureMessage::SensorID::MOD_3_PRESSURE_23X,TrussStructureMessage::SensorID::MOD_3_PRESSURE_23Y},
{TrussStructureMessage::SensorID::MOD_3_PRESSURE_24X,TrussStructureMessage::SensorID::MOD_3_PRESSURE_24Y}

            });

        return sensor_ids[fault_class];
    }

    struct RawData {
        double InternalFormat[fault_channel_cnt];
    };
};

enum class HoloLensOperatorID : uint8_t {
    OPERATOR_0 = 0,
    OPERATOR_1 = 1,
    OPERATOR_2 = 2,
    OPERATOR_3 = 3,
    ALL_OPERATORS = 255
};

/**
* Message type for communicatong the pose (position + orientation)
* and eye gaze ray of a (human) operator in the MR scene.
*/
struct OperatorPoseMessage {

    static constexpr std::string envelope() { return "OP"; }

    static constexpr std::array<float, 2> getFov() {
        return { 43.0,29.0 };
    }

    struct RawData
    {
        HoloLensOperatorID   operator_id;
        std::array<float, 3> position; // 3d position (x,y,z)
        std::array<float, 4> orientation; // orientation given as quaternion (x,y,z,w)
        std::array<float, 3> gaze_ray; // additional directional vector for eye gaze (x,y,z)
    };
};

namespace EventMessages {
    enum class EventType {
        PING,
        HERE,
        EVACUATE,
        CLEAR_ALL,
        SHOW_PLOT,
        SHOW_TEXT,
        REQUEST_RESPONSE,
        SEND_RESPONSE
    };

    enum class Receiver {
        HOLOLENS,
        UNREAL
    };

    static constexpr std::string envelope(Receiver rcvr, EventType evt_type) {
        std::string prefix = rcvr == Receiver::HOLOLENS ? "HL_" : "UE_";
        std::string event = "unknown";

        const size_t event_cnt = 8;
        std::string event_codes[event_cnt] = {
            "ping",
            "here",
            "evac",
            "clr_all",
            "show_plot",
            "show_text",
            "request_response",
            "send_response"
        };

        const size_t index = static_cast<size_t>(evt_type);
        if (index < event_cnt) {
            event = event_codes[index];
        }

        return prefix + event;
    }

    template <typename RawDataType, EventType event_type>
    struct BaseEventMessage {
        static constexpr EventType type() {
            return event_type;
        }

        using RawData = RawDataType;
    };

    struct OperatorIdRawData {
        HoloLensOperatorID operator_id;
    };

    struct TextRawData
    {
    private:
        static constexpr uint32_t MAX_CHAR_LENGTH = 500u;
        uint32_t char_count = 0;
        std::array<char, MAX_CHAR_LENGTH> message_chars{};
    public:
        TextRawData(const std::string& text)
            : char_count(std::min(static_cast<uint32_t>(text.size()), MAX_CHAR_LENGTH))
        {
            for (uint32_t i = 0; i < char_count; ++i) {
                message_chars[i] = static_cast<uint32_t>(*(text.begin() + i));
            }
        }

        // The message could be an empty string.
        std::string getMessage() const
        {
            std::string Message = message_chars.data();
            Message = Message.substr(0, char_count);
            return Message;
        };
    };

    struct HereEventRawData {
        HoloLensOperatorID   receiver;
        std::array<float, 3> position;
        TextRawData          message;

        HereEventRawData(const std::array<float, 3>& position, HoloLensOperatorID receiver = HoloLensOperatorID::ALL_OPERATORS)
            : receiver(receiver), position(position), message("")
        {}

        HereEventRawData(const std::array<float, 3>& position, const std::string& message, HoloLensOperatorID receiver = HoloLensOperatorID::ALL_OPERATORS)
            : receiver(receiver), position(position), message(message)
        {}
    };

    struct ShowPlotEventRawData {
        HoloLensOperatorID       receiver;
        uint32_t                 sensor_cnt;
        std::array<uint32_t, 12> sensor_ids;

        ShowPlotEventRawData(std::initializer_list<TrussStructureMessage::SensorID> sensors_ids, HoloLensOperatorID receiver = HoloLensOperatorID::ALL_OPERATORS)
            : receiver(receiver), sensor_cnt(std::min(static_cast<uint32_t>(sensors_ids.size()), 12u)), sensor_ids()
        {
            //TODO print warning if initializer list too long?
            for (uint32_t i = 0; i < sensor_cnt; ++i) {
                sensor_ids[i] = static_cast<uint32_t>(*(sensors_ids.begin() + i));
            }
        }

        ShowPlotEventRawData(const std::vector<TrussStructureMessage::SensorID>& sensors_ids, HoloLensOperatorID receiver = HoloLensOperatorID::ALL_OPERATORS)
            : receiver(receiver), sensor_cnt(std::min(static_cast<uint32_t>(sensors_ids.size()), 12u)), sensor_ids()
        {
            for (uint32_t i = 0; i < sensor_cnt; ++i) {
                sensor_ids[i] = static_cast<uint32_t>(*(sensors_ids.begin() + i));
            }
        }
    };

    struct ShowTextEventRawData {
        HoloLensOperatorID   receiver;
        TextRawData          message;

        ShowTextEventRawData(const std::string& message, HoloLensOperatorID receiver = HoloLensOperatorID::ALL_OPERATORS)
            : receiver(receiver), message(message)
        {}
    };

    enum class ResponseType : uint8_t {
        CONTINUE = 0
    };

    struct ResponseEventRawData {
        HoloLensOperatorID operator_id;
        ResponseType       response_type;
        TextRawData        message;

        ResponseEventRawData(ResponseType response_type,
            const std::string& message = "", HoloLensOperatorID operator_id = HoloLensOperatorID::ALL_OPERATORS)
            : operator_id(operator_id), response_type(response_type), message(message)
        {}
    };

    using PingEventMessage = BaseEventMessage<OperatorIdRawData, EventType::PING>;

    using HereEventMessage = BaseEventMessage<HereEventRawData, EventType::HERE>;

    using EvacuateEventMessage = BaseEventMessage<OperatorIdRawData, EventType::EVACUATE>;

    using ClearAllEventMessage = BaseEventMessage<OperatorIdRawData, EventType::CLEAR_ALL>;

    using ShowPlotEventMessage = BaseEventMessage<ShowPlotEventRawData, EventType::SHOW_PLOT>;

    using ShowTextEventMessage = BaseEventMessage<ShowTextEventRawData, EventType::SHOW_TEXT>;

    using RequestResponseEventMessage = BaseEventMessage<ResponseEventRawData, EventType::REQUEST_RESPONSE>;

    using SendResponseEventMessage = BaseEventMessage<ResponseEventRawData, EventType::SEND_RESPONSE>;
}



#endif // !message_types_h