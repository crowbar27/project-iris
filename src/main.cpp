#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <highfive/H5Easy.hpp>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "portable-file-dialogs.h"

#include <future>
#include <stdlib.h>
#include <stdio.h>
#include <queue>

#include <format>

#include "config.h"
#include "message_types.h"
#include "udp_multicast_receive.h"

namespace TheiaColorPalette
{
    static constexpr ImVec4 white(float alpha = 0.92f) { return ImVec4(1.0f, 1.0f, 1.0f, alpha); }

    static constexpr ImVec4 blue(float alpha = 1.0f) { return ImVec4(0.33f, 0.32f, 1.0f, alpha); }

    static constexpr ImVec4 orange(float alpha = 1.0f) { return ImVec4(1.0f, 0.5f, 0.0f, alpha); }

    static constexpr ImVec4 green(float alpha = 1.0f) { return ImVec4(0.1f, 1.0f, 0.7f, alpha); }
}

struct DataServer {
    void startPublisher(zmq::context_t* ctx, std::string const& adress, std::string const& pub_port) {
        try {

            //  Prepare publisher
            zmq::socket_t publisher(*ctx, zmq::socket_type::pub);
            publisher.bind(adress + pub_port);

            // wait for subscribers to connect?
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            publisher.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
            publisher.send(zmq::str_buffer("Debug message in A envelope"));

            // Init params for local file mode
            send_rate_ = 1;
            current_data_row_ = 0;
            is_sending_ = false;

            // Init live data receiving
            std::array<char, 64> group;
            WSADATA wsaData{ 0 };

            if (::WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            auto wsaClean = wil::scope_exit([](void) {
                ::WSACleanup();
                });

            std::array<iris::receive_context, 2> receivers{
                iris::receive_context(iris::d1244BroadcastPorts()[0], {iris::d1244BroadcastIps()[0], iris::d1244BroadcastIps()[1], iris::d1244BroadcastIps()[2], iris::d1244BroadcastIps()[3], iris::d1244BroadcastIps()[4]}),
                iris::receive_context(iris::d1244BroadcastPorts()[5], {iris::d1244BroadcastIps()[5]}),
            };

            std::array<wil::unique_event, receivers.size()> events;
            std::for_each(events.begin(), events.end(), [](wil::unique_event& e) {
                e.create(wil::EventOptions::ManualReset);
                });
            for (std::size_t i = 0; i < events.size(); ++i) {
                receivers[i].overlapped.hEvent = events[i].get();
            }

            const auto WSARecvMsg = iris::get_wsa_recv_msg(receivers.front().socket);

            // start sending/receiving
            auto t_0 = std::chrono::high_resolution_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();
            is_running_ = true;

            while (is_running_) {

                if (!use_local_data_) // live data handing
                {
                    TrussStructureMessage recv_message;
                    std::array<bool, 6> recv_parts = {false,false,false,false,true,true};
                    bool recv_complete = false;

                    while (!recv_complete)
                    {
                        for (auto& r : receivers) {
                            auto status = WSARecvMsg(r.socket.get(),
                                &r.message,
                                nullptr,
                                &r.overlapped,
                                nullptr);
                            if (status == SOCKET_ERROR) {
                                auto error = ::WSAGetLastError();
                                if (error != WSA_IO_PENDING) {
                                    throw std::system_error(::WSAGetLastError(), std::system_category());
                                }
                            }
                        }

                        auto status = ::WSAWaitForMultipleEvents(static_cast<DWORD>(events.size()),
                            reinterpret_cast<HANDLE*>(events.data()), FALSE, INFINITE, FALSE);
                        switch (status) {
                        case WSA_WAIT_FAILED:
                            throw std::system_error(::WSAGetLastError(), std::system_category());
                        case WSA_WAIT_TIMEOUT:
                            continue;
                        }

                        auto r = status - WSA_WAIT_EVENT_0;
                        auto& receiver = receivers[r];
                        auto pkt_info = iris::get_packet_info(receiver.message);
                        assert(pkt_info != nullptr);
                        std::string dest_ip = ::inet_ntop(AF_INET, &pkt_info->ipi_addr, group.data(), group.size());
                        std::cout << "Received via " << dest_ip << std::endl;
                        receiver.message.dwFlags = 0;
                        events[r].ResetEvent();

                        if (dest_ip == iris::d1244BroadcastIps()[0])
                        {
                            //TODO: copy data to message
                            recv_parts[0] = true;
                        }
                        else if (dest_ip == iris::d1244BroadcastIps()[1])
                        {
                            //TODO: copy data to message
                            recv_parts[1] = true;
                        }
                        else if (dest_ip == iris::d1244BroadcastIps()[2])
                        {
                            //TODO: copy data to message
                            recv_parts[2] = true;
                        }
                        else if (dest_ip == iris::d1244BroadcastIps()[3])
                        {
                            //TODO: copy data to message
                            recv_parts[3] = true;
                        }
                        else if (dest_ip == iris::d1244BroadcastIps()[4])
                        {
                            //TODO: copy data to message
                            recv_parts[4] = true;
                        }
                        else if (dest_ip == iris::d1244BroadcastIps()[5])
                        {
                            //TODO: copy data to message
                            recv_parts[5] = true;
                        }

                        // check if all parts of a truss strcuture sensor message have been collected
                        recv_complete = true;
                        for (auto& recv_part : recv_parts)
                        {
                            recv_complete &= recv_part;
                        }
                    }

                    std::cout << "All parts receviced" << std::endl;
                }
                else
                {
                    if (!local_data_.empty() && is_sending_)
                    {
                        while (elapsed_time + (1.0 / send_rate_) < std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count()) {
                            publisher.send(zmq::message_t(TrussStructureMessage::envelope().data(), TrussStructureMessage::envelope().size()), zmq::send_flags::sndmore);
                            publisher.send(zmq::message_t(&(local_data_[current_data_row_++]), sizeof(TrussStructureMessage::RawSensorData)));
                            elapsed_time += (1.0 / send_rate_);
                        }

                        if (current_data_row_ >= local_data_.size()) {
                            current_data_row_ = 0;
                        }
                    }
                    else
                    {
                        elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }
            }

        }
        catch (std::system_error err)
        {
            std::cerr << err.what();
        }
    }

    void stopPublisher() {
        is_running_ = false;
    }

    void startSending() {
        is_sending_ = true;
    }

    void pauseSending() {
        is_sending_ = false;
    }

    void resetSending() {
        is_sending_ = false;
        current_data_row_ = 0;
    }

    void loadLocalDataFromFile(std::string const& filepath) {
        // TODO load hdf5 file and store local data

        // TODO try-catch
        local_data_file_ = std::make_unique<H5Easy::File>(filepath, H5Easy::File::ReadOnly);

        auto shape = H5Easy::getShape((*local_data_file_), "/meas_data");

        // check matrix format
        if (shape.size() == 2) {

            // we get the dataset
            HighFive::DataSet dataset = local_data_file_->getDataSet("/meas_data");

            std::vector<std::vector<double>> load_buffer;
            dataset.read(load_buffer);

            local_data_.resize(load_buffer.size());
            for (size_t row = 0; row < load_buffer.size(); ++row) {
                std::memcpy(&local_data_[row].data, load_buffer[row].data(), load_buffer[row].size() * sizeof(double));
            }
        }
    }

    void previewLocalData() {
        // TODO draw local data as imgui table

        if (local_data_file_ != nullptr)
        {
            ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

            //if (ImGui::Begin("Local HDF5 data)")) {
            if (ImGui::BeginTable("/meas_data", TrussStructureMessage::sensor_cnt, flags))
            {
                for (int column = 0; column < TrussStructureMessage::sensor_cnt; ++column) {
                    ImGui::TableSetupColumn(TrussStructureMessage::getLabel(column).c_str());
                }
                ImGui::TableHeadersRow();
                ImGuiListClipper clipper;
                clipper.Begin(local_data_.size());
                while (clipper.Step()) {
                    for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                        ImGui::TableNextRow();
                        auto& row_data = local_data_[row];
                        for (int column = 0; column < TrussStructureMessage::sensor_cnt; ++column)
                        {
                            ImGui::TableSetColumnIndex(column);
                            if (row == current_data_row_) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(TheiaColorPalette::orange()));
                            }
                            else
                            {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                    row % 2 ? ImGui::GetColorU32(TheiaColorPalette::white(0.4f)) : ImGui::GetColorU32(TheiaColorPalette::white(0.2f))
                                );
                            }

                            //ImGui::Text("Row %d Column %d", row, column);
                            ImGui::Text("%04.4f", row_data.data[column]);
                        }
                    }
                }
                ImGui::EndTable();
            }
            //}
            //ImGui::End();
        }
    }

    std::atomic_bool is_running_;
    bool             use_local_data_;

    // local data mode variables
    std::unique_ptr<H5Easy::File>                     local_data_file_;
    std::vector<TrussStructureMessage::RawSensorData> local_data_;
    std::atomic<float>                                send_rate_;
    std::atomic<size_t>                               current_data_row_;
    std::atomic_bool                                  is_sending_;

    // live data mode variables
    std::vector<TrussStructureMessage::RawSensorData> received_data_ringbuffer_;
    size_t                                            current_ringbuffer_idx_;
};

struct DummySubscriber {
    // TODO simulate receiving live data

    void startSubscriber(zmq::context_t* ctx, std::string const& adress, std::string const& sub_port) {
        //  Prepare subscriber
        zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
        subscriber.connect(adress + sub_port);

        //  Opens "A" and "B" envelopes
        subscriber.set(zmq::sockopt::subscribe, "A");
        subscriber.set(zmq::sockopt::subscribe, TrussStructureMessage::envelope());

        std::vector<zmq::pollitem_t> items = { {subscriber, 0, ZMQ_POLLIN, 0 } };

        is_running_ = true;
        while (is_running_) {
            // Poll for incoming messages with 100ms timeout
            zmq::poll(items, 100);

            if (items.front().revents & ZMQ_POLLIN)
            {
                // Receive all parts of the message
                std::vector<zmq::message_t> recv_msgs;
                zmq::recv_result_t result =
                    zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));
                assert(result && "recv failed");
                assert(*result == 2);

                if (recv_msgs[0].to_string() == "A") {
                    std::cout << "[" << recv_msgs[0].to_string() << "] "
                        << recv_msgs[1].to_string() << std::endl;
                }
                else if (recv_msgs[0].to_string() == TrussStructureMessage::envelope()) {
                    std::cout << "[" << recv_msgs[0].to_string() << "] "
                        << recv_msgs[1] << std::endl;

                    received_data_.push_back(*(recv_msgs[1].data<TrussStructureMessage::RawSensorData>()));
                }
            }
        }
    }

    void stopSubscriber()
    {
        is_running_ = false;
    }

    void showReceivedData() {
        ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

        if (!received_data_.empty()) {
            if (ImGui::Begin("Local HDF5 data)")) {
                if (ImGui::BeginTable("/meas_data", TrussStructureMessage::sensor_cnt, flags))
                {
                    for (int column = 0; column < TrussStructureMessage::sensor_cnt; ++column) {
                        ImGui::TableSetupColumn(TrussStructureMessage::getLabel(column).c_str());
                    }
                    ImGui::TableHeadersRow();
                    ImGuiListClipper clipper;
                    clipper.Begin(received_data_.size());
                    while (clipper.Step()) {
                        for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                            ImGui::TableNextRow();
                            auto& row_data = received_data_[row];
                            for (int column = 0; column < TrussStructureMessage::sensor_cnt; column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                //ImGui::Text("Row %d Column %d", row, column);
                                ImGui::Text("%04.4f", row_data.data[column]);
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::End();
        }
    }

    std::atomic_bool is_running_;

    std::vector<TrussStructureMessage::RawSensorData> received_data_;
};

/**
* Contains a subsriber for receiving operator pose data from HoloLens
* and a publisher to forward the pose data to Unreal
*/
struct OperatorPoseServer
{
    void start(zmq::context_t* ctx, std::string const& adress, std::string const& sub_port, std::string const& pub_port)
    {
        //  Prepare subscriber
        zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
        subscriber.bind(adress + sub_port); // bind to own IP adress, while HoloLens will connect to this adress
        //  Opens OperatorPoseMessage envelope
        subscriber.set(zmq::sockopt::subscribe, OperatorPoseMessage::envelope());

        // Prepare publisher for message forwarding
        zmq::socket_t publisher(*ctx, zmq::socket_type::pub);
        publisher.bind(adress + pub_port);

        std::vector<zmq::pollitem_t> items = { {subscriber, 0, ZMQ_POLLIN, 0 } };

        auto t_0 = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();

        send_rate_ = 1;
        current_data_row_ = 0;
        is_running_ = true;
        while (is_running_) {

            if (use_local_data_ && forward_data_ && !local_data_.empty())
            {
                while (elapsed_time + (1.0 / send_rate_) < std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count()) {
                    publisher.send(zmq::message_t(OperatorPoseMessage::envelope().data(), OperatorPoseMessage::envelope().size()), zmq::send_flags::sndmore);
                    publisher.send(zmq::message_t(&(local_data_[current_data_row_++]), sizeof(OperatorPoseMessage::RawData)));
                    elapsed_time += (1.0 / send_rate_);
                }

                if (current_data_row_ >= local_data_.size()) {
                    current_data_row_ = 0;
                }
            }
            else
            {
                elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Poll for incoming messages with 100ms timeout
                zmq::poll(items, 100);

                if (items.front().revents & ZMQ_POLLIN)
                {
                    // Receive all parts of the message
                    std::vector<zmq::message_t> recv_msgs;
                    zmq::recv_result_t result =
                        zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));
                    assert(result && "recv failed");
                    assert(*result == 2);

                    if (recv_msgs[0].to_string() == OperatorPoseMessage::envelope()) {
                        //std::cout << "[" << recv_msgs[0].to_string() << "] "
                        //    << recv_msgs[1] << std::endl;

                        received_data_.push_back(*(recv_msgs[1].data<OperatorPoseMessage::RawData>()));

                        // forward message if not using local file
                    }
                }

            }
        }
    }

    void displayDataTable(std::vector<OperatorPoseMessage::RawData> const& data) {
        ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

        //if (ImGui::Begin("Received data")) {
        int columm_cnt = 10;/* number of floats in each OperatorPoseMessage */
        if (ImGui::BeginTable("Operator pose", columm_cnt, flags))
        {
            ImGui::TableSetupColumn("Position x");
            ImGui::TableSetupColumn("Position y");
            ImGui::TableSetupColumn("Position z");
            ImGui::TableSetupColumn("Orientation x");
            ImGui::TableSetupColumn("Orientation y");
            ImGui::TableSetupColumn("Orientation z");
            ImGui::TableSetupColumn("Orientation w");
            ImGui::TableSetupColumn("Gaze x");
            ImGui::TableSetupColumn("Gaze y");
            ImGui::TableSetupColumn("Gaze z");

            ImGui::TableHeadersRow();

            if (!data.empty()) {
                ImGuiListClipper clipper;
                clipper.Begin(data.size());
                while (clipper.Step()) {
                    for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                        ImGui::TableNextRow();
                        auto& row_data = data[row];

                        for (int col = 0; col < 10; ++col) {
                            if (row == current_data_row_) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(TheiaColorPalette::orange()), col);
                            }
                            else
                            {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                    row % 2 ? ImGui::GetColorU32(TheiaColorPalette::white(0.4f)) : ImGui::GetColorU32(TheiaColorPalette::white(0.2f)), col
                                );
                            }
                        }

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%04.4f", row_data.position[0]);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%04.4f", row_data.position[1]);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%04.4f", row_data.position[2]);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%04.4f", row_data.orientation[0]);
                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%04.4f", row_data.orientation[1]);
                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%04.4f", row_data.orientation[2]);
                        ImGui::TableSetColumnIndex(6);
                        ImGui::Text("%04.4f", row_data.orientation[3]);

                        ImGui::TableSetColumnIndex(7);
                        ImGui::Text("%04.4f", row_data.gaze_ray[0]);
                        ImGui::TableSetColumnIndex(8);
                        ImGui::Text("%04.4f", row_data.gaze_ray[1]);
                        ImGui::TableSetColumnIndex(9);
                        ImGui::Text("%04.4f", row_data.gaze_ray[2]);
                    }
                }
            }
            ImGui::EndTable();
        }
        //}
        //ImGui::End();
    }

    void storeReceivedData(std::string const& filepath)
    {
        HighFive::File file(filepath, HighFive::File::Truncate);

        std::vector<std::vector<float>> write_buffer;

        for (size_t row = 0; row < received_data_.size(); ++row) {
            write_buffer.push_back(
                std::vector<float>{
                static_cast<float>(received_data_[row].operator_id),
                    received_data_[row].position[0],
                    received_data_[row].position[1],
                    received_data_[row].position[2],
                    received_data_[row].orientation[0],
                    received_data_[row].orientation[1],
                    received_data_[row].orientation[2],
                    received_data_[row].orientation[3],
                    received_data_[row].gaze_ray[0],
                    received_data_[row].gaze_ray[1],
                    received_data_[row].gaze_ray[2]
            });
            //write_buffer.push_back(std::vector<float>(10));
            //std::memcpy(write_buffer[row].data(), &received_data_[row], sizeof(OperatorPoseMessage::RawData));
        }

        file.createDataSet("op_pose/data", write_buffer);
    }

    void loadLocalDataFromFile(std::string const& filepath) {

        // TODO try-catch
        local_data_file_ = std::make_unique<H5Easy::File>(filepath, H5Easy::File::ReadOnly);

        auto shape = H5Easy::getShape((*local_data_file_), "op_pose/data");

        // check matrix format
        if (shape.size() == 2) {

            // we get the dataset
            HighFive::DataSet dataset = local_data_file_->getDataSet("op_pose/data");

            std::vector<std::vector<float>> load_buffer;
            dataset.read(load_buffer);

            local_data_.resize(load_buffer.size());
            for (size_t row = 0; row < load_buffer.size(); ++row) {
                local_data_[row].operator_id = static_cast<HoloLensOperatorID>(load_buffer[row].data()[0]);
                local_data_[row].position = { load_buffer[row].data()[1],load_buffer[row].data()[2],load_buffer[row].data()[3] };
                local_data_[row].orientation = { load_buffer[row].data()[4],load_buffer[row].data()[5],load_buffer[row].data()[6], load_buffer[row].data()[7] };
                local_data_[row].gaze_ray = { load_buffer[row].data()[8],load_buffer[row].data()[9],load_buffer[row].data()[10] };
                //std::memcpy(&local_data_[row], load_buffer[row].data(), load_buffer[row].size() * sizeof(float));
            }
        }
    }

    void startSending() {
        forward_data_ = true;
    }

    void pauseSending() {
        forward_data_ = false;
    }

    void resetSending() {
        forward_data_ = false;
        current_data_row_ = 0;
    }

    std::vector<OperatorPoseMessage::RawData> received_data_;

    std::atomic_bool is_running_;

    std::atomic_bool forward_data_;

    std::atomic_bool use_local_data_;

    std::unique_ptr<H5Easy::File> local_data_file_;

    std::vector<OperatorPoseMessage::RawData> local_data_;

    std::atomic<float> send_rate_;

    std::atomic<size_t> current_data_row_;
};


struct EventServer {

    void start(zmq::context_t* ctx, std::string const& adress, std::string const& sub_port, std::string const& pub_port)
    {
        std::cout << "Started event server!" << std::endl;

        //  Prepare subscriber
        zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);

        // subscriber.bind("tcp://127.0.0.1:5558");
        subscriber.bind(adress + sub_port); // Bind to * for use locally with Unreal Engine application.

        // Prepare publisher for message forwarding
        zmq::socket_t publisher(*ctx, zmq::socket_type::pub);
        publisher.bind(adress + pub_port);

        // Subscribe to the OperatorIdRawData events.

        const auto event_clear_all_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::CLEAR_ALL);
        subscriber.set(zmq::sockopt::subscribe, event_clear_all_envelope);

        const auto event_evacuate_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::EVACUATE);
        subscriber.set(zmq::sockopt::subscribe, event_evacuate_envelope);

        const auto event_ping_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::PING);
        subscriber.set(zmq::sockopt::subscribe, event_ping_envelope);

        // Subscribe to the REQUEST_RESPONSE ResponseEventRawData event.
        const auto event_request_response_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::REQUEST_RESPONSE);
        subscriber.set(zmq::sockopt::subscribe, event_request_response_envelope);

        // Subscribe to HERE event.
        const auto event_here_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::HERE);
        subscriber.set(zmq::sockopt::subscribe, event_here_envelope);

        // Subscribe to SHOW_PLOT event.
        const auto event_show_plot_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::SHOW_PLOT);
        subscriber.set(zmq::sockopt::subscribe, event_show_plot_envelope);

        // Subscribe to SHOW_TEXT event.
        const auto event_show_text_envelope =
            EventMessages::envelope(EventMessages::Receiver::HOLOLENS, EventMessages::EventType::SHOW_TEXT);
        subscriber.set(zmq::sockopt::subscribe, event_show_text_envelope);

        std::vector<zmq::pollitem_t> items = { {subscriber, 0, ZMQ_POLLIN, 0 } };

        auto t_0 = std::chrono::high_resolution_clock::now();
        auto elapsed_time =
            std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();

        is_running_ = true;

        while (is_running_) {

            elapsed_time =
                std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Poll for incoming messages with 100ms timeout
            zmq::poll(items, 100);

            if (items.front().revents & ZMQ_POLLIN)
            {
                // Receive all parts of the message
                std::vector<zmq::message_t> recv_msgs;
                zmq::recv_result_t result = zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));

                assert(result && "recv failed");
                assert(*result == 2);

                std::cout << "Received Messages Size: " << recv_msgs.size() << std::endl;
                const std::string envelope_string = recv_msgs[0].to_string();
                std::cout << "Envelope '" << envelope_string << "' message received at: " << elapsed_time << std::endl;

                // HERE event envelope.
                if (envelope_string == event_here_envelope) {

                    std::cout << "Received a HERE event." << std::endl;

                    const EventMessages::HereEventRawData MsgData =
                        *(recv_msgs[1].data<EventMessages::HereEventRawData>());

                    // Vector array values, formatted with less decimals.
                    std::string location_string =
                        "[" + std::format("{:.2f}", MsgData.position[0]) + ", "
                        + std::format("{:.2f}", MsgData.position[1]) + ", "
                        + std::format("{:.2f}", MsgData.position[2]) + "]";

                    std::string message_string = "\nMsg: '" + MsgData.message.getMessage() + "'";
                    std::cout << location_string << message_string << std::endl;

                    // Print the receiver enum value.
                    std::cout << "Receiving Operator Number: " << static_cast<uint32_t>(MsgData.receiver) << std::endl;

                    received_here_data_.push_back(*(recv_msgs[1].data<EventMessages::HereEventMessage>()));
                    received_messages_.push_back({ envelope_string, location_string + message_string });
                }
                // SHOW_PLOT event envelope.
                else if (envelope_string == event_show_plot_envelope)
                {
                    std::cout << "Received a SHOW_PLOT event." << std::endl;

                    const EventMessages::ShowPlotEventRawData MsgData =
                        *(recv_msgs[1].data<EventMessages::ShowPlotEventRawData>());

                    std::string sensor_string = "[";
                    for (int i = 0; i < MsgData.sensor_cnt; ++i)
                    {
                        sensor_string += TrussStructureMessage::getLabel(MsgData.sensor_ids[i]);
                        if (i < MsgData.sensor_cnt - 1)
                        {
                            sensor_string += ", ";
                        }
                        else
                        {
                            sensor_string += "]";
                        }
                    }

                    // Print data as well as the receiving operator.
                    std::cout << sensor_string << std::endl;
                    std::cout << "Receiving Operator Number: " << static_cast<uint32_t>(MsgData.receiver) << std::endl;

                    received_show_plot_data_.push_back(*(recv_msgs[1].data<EventMessages::ShowPlotEventMessage>()));
                    received_messages_.push_back({ envelope_string, sensor_string });
                }
                // SHOW_TEXT event envelope.
                else if (envelope_string == event_show_text_envelope)
                {
                    std::cout << "Received a SHOW_TEXT event." << std::endl;

                    const EventMessages::ShowTextEventRawData MsgData =
                        *(recv_msgs[1].data<EventMessages::ShowTextEventRawData>());

                    std::string message = MsgData.message.getMessage();

                    std::cout << "Message: " << message << ", Size: " << message.size() << std::endl;

                    std::cout << "Receiving Operator Number: " << static_cast<uint32_t>(MsgData.receiver) << std::endl;

                    received_show_text_data_.push_back(*(recv_msgs[1].data<EventMessages::ShowTextEventMessage>()));
                    received_messages_.push_back({ envelope_string, message });
                }
                // Any OperatorIdRawData event envelope.
                else if (
                    envelope_string == event_clear_all_envelope ||
                    envelope_string == event_evacuate_envelope ||
                    envelope_string == event_ping_envelope
                    )
                {
                    std::cout << "Received a OperatorID (" << envelope_string << ") event." << std::endl;

                    const EventMessages::OperatorIdRawData MsgData =
                        *(recv_msgs[1].data<EventMessages::OperatorIdRawData>());

                    uint32_t operator_id_number = static_cast<uint32_t>(MsgData.operator_id);
                    std::cout << "Receiving Operator Number: " << operator_id_number << std::endl;

                    received_show_text_data_.push_back(*(recv_msgs[1].data<EventMessages::ShowTextEventMessage>()));
                    received_messages_.push_back({ envelope_string, "Operator ID: " + std::to_string(operator_id_number) });
                }
                // REQUEST_RESPONSE event envelope.
                else if (envelope_string == event_request_response_envelope)
                {
                    std::cout << "Received a REQUEST_RESPONSE event." << std::endl;

                    const EventMessages::ResponseEventRawData MsgData =
                        *(recv_msgs[1].data<EventMessages::ResponseEventRawData>());

                    std::string message = MsgData.message.getMessage();

                    std::cout << "Receiving Operator Number: " << static_cast<uint32_t>(MsgData.operator_id) << std::endl;
                    std::cout << "Message: " << message << ", Size: " << message.size() << std::endl;
                    std::cout << "Response Type: " << static_cast<uint32_t>(MsgData.response_type) << std::endl;

                    received_show_text_data_.push_back(*(recv_msgs[1].data<EventMessages::ShowTextEventMessage>()));

                    std::string message_text =
                        "Operator ID: " + std::to_string(static_cast<uint8_t>(MsgData.operator_id)) +
                        "\nResponse Type ID: " + std::to_string(static_cast<uint8_t>(MsgData.response_type)) +
                        "\nMessage: " + message;

                    received_messages_.push_back({ envelope_string, message_text });
                }

                // Forward events
                publisher.send(recv_msgs[0], zmq::send_flags::sndmore);
                publisher.send(recv_msgs[1]);

                // Send message from queue (basically a debug feature to spawn messages from the server)
                while (!send_message_queue_.empty())
                {
                    auto const& msg = send_message_queue_.front();

                    switch (msg.first)
                    {
                    case EventMessages::EventType::SEND_RESPONSE:
                        publisher.send(zmq::message_t(
                            EventMessages::envelope(EventMessages::Receiver::UNREAL, EventMessages::EventType::SEND_RESPONSE).data(),
                            EventMessages::envelope(EventMessages::Receiver::UNREAL, EventMessages::EventType::SEND_RESPONSE).size()),
                            zmq::send_flags::sndmore
                        );
                        publisher.send(zmq::message_t(msg.second.get(), sizeof(EventMessages::ResponseEventRawData)));
                        break;
                        //TODO cover all (relevant) cases
                    default:
                        break;
                    }

                    //TODO add inserted message to UI display?
                }
            }
        }
    }

    void viewEventData()
    {
        ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable |
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
            ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

        if (ImGui::BeginTable("/event_data", 2, flags))
        {
            ImGui::TableSetupColumn("Envelope");
            ImGui::TableSetupColumn("Value(s)");
            ImGui::TableHeadersRow();
            ImGuiListClipper clipper;

            clipper.Begin(received_messages_.size());
            while (clipper.Step()) {
                for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
                {
                    ImGui::TableNextRow();

                    if (row % 2 == 0) {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(TheiaColorPalette::orange()));
                    }
                    else
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                            row % 2 ? ImGui::GetColorU32(TheiaColorPalette::white(0.4f)) : ImGui::GetColorU32(TheiaColorPalette::white(0.2f))
                        );
                    }

                    // Write to the two columns.
                    auto& row_data = received_messages_[row];
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextWrapped(get<0>(row_data).c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped(get<1>(row_data).c_str());
                }
            }
            ImGui::EndTable();
        }
    }

    std::atomic_bool is_running_;
    std::vector<EventMessages::HereEventMessage> received_here_data_;
    std::vector<EventMessages::ShowPlotEventMessage> received_show_plot_data_;
    std::vector<EventMessages::ShowTextEventMessage> received_show_text_data_;
    std::vector<std::tuple<std::string, std::string>> received_messages_;

    std::queue<std::pair<EventMessages::EventType, std::shared_ptr<void>>> send_message_queue_;
};


void customImGuiColors()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    //colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = TheiaColorPalette::white(0.50f); //ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg] = TheiaColorPalette::white(0.54f); //ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = TheiaColorPalette::white(0.67f); //ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = TheiaColorPalette::white(0.80f); //ImVec4(0.26f, 0.59f, 0.98f, 0.67f);

    colors[ImGuiCol_TitleBg] = TheiaColorPalette::white(0.54f); //ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = TheiaColorPalette::blue(1.00f); //ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = TheiaColorPalette::white(0.92f); //ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    colors[ImGuiCol_Button] = TheiaColorPalette::white(0.92f); //ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = TheiaColorPalette::white(1.00f); //ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = TheiaColorPalette::blue(1.00f); //ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

    colors[ImGuiCol_Header] = TheiaColorPalette::white(0.54f); //ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = TheiaColorPalette::white(1.00f); //ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = TheiaColorPalette::white(0.92f); //ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

    colors[ImGuiCol_Tab] = colors[ImGuiCol_Header];
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);

    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGuiStyle style = ImGui::GetStyle();

    //ImFontConfig config;
    //config.MergeMode = true;
    //static const ImWchar icon_ranges[] = { '\uE000', '\uF8FF', 0 };
    io.Fonts->AddFontDefault();
    auto aileron_light_font = io.Fonts->AddFontFromFileTTF("../resources/fonts/ProggyVector-Dotted.ttf", 24);
    //auto aileron_light_font = io.Fonts->AddFontFromFileTTF("../Aileron-Light.ttf", 24, &config, icon_ranges);

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    customImGuiColors();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    zmq::context_t ctx;

    // Servers..
    DataServer server;
    DummySubscriber dummy_subscriber;
    OperatorPoseServer operator_pose_server;
    EventServer event_server;

    bool server_mode = true;

    // Start servers.
    auto a = std::async(std::launch::async, &DataServer::startPublisher, &server, &ctx, iris::serverIp(), iris::sensorDataPubPort());
    //auto a = std::async(std::launch::async, &DummySubscriber::startSubscriber, &dummy_subscriber, &ctx, iris::serverIp(), iris::sensorDataPubPort());

    auto ops_exec = std::async(std::launch::async, &OperatorPoseServer::start, &operator_pose_server, &ctx, iris::serverIp(), iris::operatorPoseSubPort(), iris::operatorPosePubPort());

    auto event_exec = std::async(std::launch::async, &EventServer::start, &event_server, &ctx, iris::serverIp(), iris::eventDataSubPort(), iris::eventDataPubPort());

    // debug UDP multicast by sending some packages myself
    std::atomic<bool> run_server(true);
    {
        std::thread server(iris::push_data2, std::ref(run_server));
        server.detach();
    }

    std::string local_truss_structure_sensor_data_filepath;

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushFont(aileron_light_font);

        auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        bool control_panel_is_open = true;

        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
        ImGui::Begin("ControlPanel", &control_panel_is_open, flags);

        // Gather some numbers for layouting
        float frame_height = ImGui::GetFrameHeight();
        float item_inner_spacing = style.ItemInnerSpacing.x;

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_FittingPolicyDefault_;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            ImGui::SetNextItemWidth(frame_height * 12.0f + item_inner_spacing * 4.0f);
            if (ImGui::BeginTabItem("Truss Structure Sensor Data"))
            {

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                if (ImGui::RadioButton("Live Data", !server.use_local_data_))
                {
                    server.use_local_data_ = false;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Local File", server.use_local_data_))
                {
                    server.use_local_data_ = true;
                }
                ImGui::PopStyleColor();
                ImGui::Separator();

                if (server.use_local_data_)
                {
                    // Loading local data
                    if (ImGui::Button("Load Local Data", ImVec2(frame_height * 8.0f + item_inner_spacing * 2.0f, frame_height))) {

                        auto selection = pfd::open_file("Select a file").result();
                        if (!selection.empty()) {
                            local_truss_structure_sensor_data_filepath = selection[0];
                            server.loadLocalDataFromFile(selection[0]);
                        }
                    }
                    ImGui::SameLine();
                    ImGui::Text(local_truss_structure_sensor_data_filepath.c_str());

                    ImGui::Separator();

                    // Server settings
                    if (ImGui::Button("Start", ImVec2(frame_height * 4.0f, frame_height))) {
                        server.startSending();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Pause", ImVec2(frame_height * 4.0f, frame_height))) {
                        server.pauseSending();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reset", ImVec2(frame_height * 4.0f, frame_height))) {
                        server.resetSending();
                    }

                    static float send_rate = 1.0f;
                    ImGui::SetNextItemWidth(frame_height * 12.0f + item_inner_spacing * 4.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                    if (ImGui::DragFloat("Send rate (Hz)", &send_rate, 1.0f, 1.0f, 100.0f)) {
                        server.send_rate_ = send_rate;
                    }
                    ImGui::PopStyleColor();

                    static bool pin_sent_line = false;
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                    ImGui::RadioButton("Pin last sent data", &pin_sent_line);
                    ImGui::PopStyleColor();

                    ImGui::Separator();

                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 128));
                    server.previewLocalData();
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                }
                else
                {

                }

                ImGui::EndTabItem();
            }
            ImGui::SetNextItemWidth(frame_height * 12.0f + item_inner_spacing * 4.0f);
            if (ImGui::BeginTabItem("Operator Pose Data"))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                if (ImGui::RadioButton("Live Data", !operator_pose_server.use_local_data_))
                {
                    operator_pose_server.use_local_data_ = false;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Local File", operator_pose_server.use_local_data_))
                {
                    operator_pose_server.use_local_data_ = true;
                }
                ImGui::PopStyleColor();
                ImGui::Separator();

                if (!operator_pose_server.use_local_data_) {
                    if (ImGui::Button("Save Received Data", ImVec2(frame_height * 8.0f + item_inner_spacing * 2.0f, frame_height))) {
                        auto selection = pfd::save_file("Select a file").result();;
                        if (!selection.empty()) {
                            operator_pose_server.storeReceivedData(selection);
                        }
                    }

                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                    if (ImGui::RadioButton("Forward data", operator_pose_server.forward_data_)) {
                        operator_pose_server.forward_data_ = !operator_pose_server.forward_data_;
                    }
                    ImGui::PopStyleColor();
                }
                else
                {
                    if (ImGui::Button("Load Local File", ImVec2(frame_height * 8.0f + item_inner_spacing * 2.0f, frame_height))) {
                        auto selection = pfd::open_file("Select a file").result();;
                        if (!selection.empty()) {
                            operator_pose_server.loadLocalDataFromFile(selection[0]);
                        }
                    }

                    ImGui::Separator();

                    // Server settings
                    if (ImGui::Button("Start", ImVec2(frame_height * 4.0f, frame_height))) {
                        operator_pose_server.startSending();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Pause", ImVec2(frame_height * 4.0f, frame_height))) {
                        operator_pose_server.pauseSending();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reset", ImVec2(frame_height * 4.0f, frame_height))) {
                        operator_pose_server.resetSending();
                    }

                    static float send_rate = 1.0f;
                    ImGui::SetNextItemWidth(frame_height * 12.0f + item_inner_spacing * 4.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                    if (ImGui::DragFloat("Send rate (Hz)", &send_rate, 1.0f, 1.0f, 100.0f)) {
                        operator_pose_server.send_rate_ = send_rate;
                    }
                    ImGui::PopStyleColor();

                    static bool pin_sent_line = false;
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                    ImGui::RadioButton("Pin last sent data", &pin_sent_line);
                    ImGui::PopStyleColor();

                }

                ImGui::Separator();

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 128));
                if (!operator_pose_server.use_local_data_)
                {
                    operator_pose_server.displayDataTable(operator_pose_server.received_data_);
                }
                else
                {
                    operator_pose_server.displayDataTable(operator_pose_server.local_data_);
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                ImGui::EndTabItem();
            }
            ImGui::SetNextItemWidth(frame_height * 12.0f + item_inner_spacing * 4.0f);

            // Event tab.
            if (ImGui::BeginTabItem("Events"))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                ImGui::Text("Inject event message");
                ImGui::PopStyleColor();

                bool send_button_clicked = ImGui::Button("Send", ImVec2(frame_height * 8.0f + item_inner_spacing * 2.0f, frame_height));

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 128));
                static int receiver_selection_idx = 0;
                static int event_selection_idx = 0;
                {
                    const char* items[] = { "Unreal Engine", "HoloLens" };
                    const char* combo_preview_value = items[receiver_selection_idx];
                    ImGui::SetNextItemWidth(frame_height * 8.0f + item_inner_spacing * 2.0f);
                    if (ImGui::BeginCombo("##Receiver", combo_preview_value))
                    {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                        {
                            const bool is_selected = (receiver_selection_idx == n);
                            if (ImGui::Selectable(items[n], is_selected))
                            {
                                receiver_selection_idx = n;
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::SameLine();
                {
                    const char* items[] = { "Here", "SendResponse" };
                    const char* combo_preview_value = items[event_selection_idx];
                    ImGui::SetNextItemWidth(frame_height * 8.0f + item_inner_spacing * 2.0f);
                    if (ImGui::BeginCombo("##Event", combo_preview_value))
                    {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                        {
                            const bool is_selected = (event_selection_idx == n);
                            if (ImGui::Selectable(items[n], is_selected))
                            {
                                event_selection_idx = n;
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::SameLine();

                static char message_text[256] = "";
                ImGui::SetNextItemWidth(frame_height * 12.0f + item_inner_spacing * 4.0f);
                ImGui::InputText("##Message", message_text, 256);

                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                //TODO dropdown operator id

                if (send_button_clicked)
                {
                    if (event_selection_idx == 1)
                    {
                        event_server.send_message_queue_.push(std::pair<EventMessages::EventType, std::shared_ptr<void>>{
                            EventMessages::EventType::SEND_RESPONSE,
                                std::shared_ptr<void>(new EventMessages::ResponseEventRawData(EventMessages::ResponseType::CONTINUE, message_text))
                        });
                    }
                }

                ImGui::Separator();

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 234));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 128));
                event_server.viewEventData();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::PopFont();
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    run_server.store(false);

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}