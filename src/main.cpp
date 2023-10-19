#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

#include "message_types.h"

struct DataServer {
    void startPublisher(zmq::context_t* ctx) {
        // TODO simulate publishing of live data

        //  Prepare publisher
        zmq::socket_t publisher(*ctx, zmq::socket_type::pub);
        publisher.bind("tcp://129.69.205.56:5555");
        //publisher.bind("tcp://127.0.0.1:5555");

        // wait for subscribers to connect?
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        publisher.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
        publisher.send(zmq::str_buffer("Debug message in A envelope"));

        send_rate_ = 1;
        current_data_row_ = 0;
        is_sending_ = false;

        is_running_ = true;

        auto t_0 = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_0).count();

        while (is_running_) {

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
                std::memcpy(&local_data_[row].data, load_buffer[row].data(), load_buffer[row].size());
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
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.8f, 0.6f, 0.2f, 1)));
                            }
                            else
                            {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                    row % 2 ? ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 1)) : ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 1))
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

    std::unique_ptr<H5Easy::File> local_data_file_;
    //std::vector<std::vector<double>> local_data_;
    std::vector<TrussStructureMessage::RawSensorData> local_data_;

    std::atomic<float> send_rate_;
    std::atomic<size_t> current_data_row_;
    std::atomic_bool is_sending_;

    std::atomic_bool is_running_;
};

struct DummySubscriber {
    // TODO simulate receiving live data

    void startSubscriber(zmq::context_t* ctx) {
        //  Prepare subscriber
        zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
        subscriber.connect("tcp://127.0.0.1:5555");

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
    void start(zmq::context_t* ctx)
    {
        //  Prepare subscriber
        zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
        subscriber.bind("tcp://129.69.205.56:5555"); // bind to own IP adress, while HoloLens will connect to this adress
        //  Opens OperatorPoseMessage envelope
        subscriber.set(zmq::sockopt::subscribe, OperatorPoseMessage::envelope());

        // Prepare publisher for message forwarding
        zmq::socket_t publisher(*ctx, zmq::socket_type::pub);
        publisher.bind("tcp://129.69.205.56:5555");

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

                if (recv_msgs[0].to_string() == OperatorPoseMessage::envelope()) {
                    std::cout << "[" << recv_msgs[0].to_string() << "] "
                        << recv_msgs[1] << std::endl;

                    received_data_.push_back(*(recv_msgs[1].data<OperatorPoseMessage::RawData>()));

                    //TODO forward message if not using local file
                }
            }
        }
    }

    void showReceivedData() {
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

                if (!received_data_.empty()) {
                    ImGuiListClipper clipper;
                    clipper.Begin(received_data_.size());
                    while (clipper.Step()) {
                        for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                            ImGui::TableNextRow();
                            auto& row_data = received_data_[row];

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
            write_buffer.push_back(std::vector<float>(10));
            std::memcpy(&(write_buffer[row]), &(received_data_[row]), sizeof(OperatorPoseMessage::RawData));
        }

        file.createDataSet("OP/data", write_buffer);
    }

    std::vector<OperatorPoseMessage::RawData> received_data_;

    std::atomic_bool is_running_;

    bool use_local_data_ = false;
};

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

    ImFontConfig config;
    config.MergeMode = true;
    static const ImWchar icon_ranges[] = { '\uE000', '\uF8FF', 0 };
    io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../resources/OpenFontIcons.ttf", 24, &config, icon_ranges);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    zmq::context_t ctx;

    DataServer server;
    DummySubscriber dummy_subscriber;
    OperatorPoseServer operator_pose_server;

    bool server_mode = true;
    auto a = std::async(std::launch::async, &DataServer::startPublisher, &server, &ctx);

    auto ops_exec = std::async(std::launch::async, &OperatorPoseServer::start, &operator_pose_server, &ctx);

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

        auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        bool control_panel_is_open = true;

        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
        ImGui::Begin("ControlPanel", &control_panel_is_open, flags);


        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_FittingPolicyDefault_;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Truss Structure Sensor Data"))
            {
                {
                    // Loading local data
                    ImGui::Text("Local Data");
                    if (ImGui::Button("\ue061" " Load")) {

                        auto selection = pfd::open_file("Select a file").result();
                        if (!selection.empty()){
                            local_truss_structure_sensor_data_filepath = selection[0];
                            server.loadLocalDataFromFile(selection[0]);
                        }
                    }
                    ImGui::SameLine();
                    ImGui::Text(local_truss_structure_sensor_data_filepath.c_str());

                    ImGui::Separator();

                    ImGui::Text("Server Controls");

                    // Server settings
                    if (ImGui::Button("Start")) {
                        server.startSending();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Pause")) {
                        server.pauseSending();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reset")) {
                        server.resetSending();
                    }
                    ImGui::SameLine();
                    static float send_rate = 1.0f;
                    if (ImGui::DragFloat("Send rate (Hz)", &send_rate, 1.0f, 1.0f, 100.0f)) {
                        server.send_rate_ = send_rate;
                    }

                    ImGui::Separator();

                    ImGui::Text("Data Preview");

                    static bool pin_sent_line = false;
                    ImGui::RadioButton("Pin last sent data.", &pin_sent_line);

                    server.previewLocalData();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Operator Pose Data"))
            {
                if(ImGui::RadioButton("Live data.", &operator_pose_server.use_local_data_))
                {
                    
                }
                //if(ImGui::RadioButton("Local data.", &use_local_data))
                //{
                //    live_data = false;
                //}

                ImGui::Separator();
                ImGui::Text("Received data");
                ImGui::SameLine();
                if (ImGui::Button("\ue061" " Save")) {
                    auto selection = pfd::save_file("Select a file").result();;
                    if (!selection.empty()) {
                        operator_pose_server.storeReceivedData(selection);
                    }
                }
                operator_pose_server.showReceivedData();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Operator Pose Forwarding"))
            {
                ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }


        //  ImGui::Text("Mode");
        //  if (ImGui::RadioButton("Server", server_mode)) {
        //      if (!server_mode) {
        //          // stop subscriber and start publisher server
        //          dummy_subscriber.stopSubscriber();
        //          a = std::async(std::launch::async, &DataServer::startPublisher, &server, &ctx);
        //      }
        //      server_mode = true;
        //  }
        //  ImGui::SameLine();
        //  if (ImGui::RadioButton("Subscriber", !server_mode)) {
        //      if (server_mode) {
        //          // stop publisher server and start subscriver
        //          server.stopPublisher();
        //          a = std::async(std::launch::async, &DummySubscriber::startSubscriber, &dummy_subscriber, &ctx);
        //      }
        //      server_mode = false;
        //  }
        //  ImGui::Separator();
        //  
        //  if (server_mode) {
        //  
        //      // Loading local data
        //      ImGui::Text("Local Data");
        //      static char buf1[128] = "08-51-23.hdf5";
        //      ImGui::InputText("##FilepathInput", buf1, 128);
        //      ImGui::SameLine();
        //      if (ImGui::Button("\ue061" " Load")) {
        //          server.loadLocalDataFromFile(std::string(buf1));
        //      }
        //  
        //      ImGui::Separator();
        //  
        //      ImGui::Text("Server Controls");
        //  
        //      // Server settings
        //      if(ImGui::Button("Start")){
        //          server.startSending();
        //      }
        //      ImGui::SameLine();
        //      if(ImGui::Button("Pause")){
        //          server.pauseSending();
        //      }
        //      ImGui::SameLine();
        //      if(ImGui::Button("Reset")){
        //          server.resetSending();
        //      }
        //      ImGui::SameLine();
        //      static float send_rate = 1.0f;
        //      if (ImGui::DragFloat("Send rate (Hz)", &send_rate, 1.0f, 1.0f, 100.0f)) {
        //          server.send_rate_ = send_rate;
        //      }
        //  
        //      ImGui::Separator();
        //  
        //      ImGui::Text("Data Preview");
        //  
        //      static bool pin_sent_line = false;
        //      ImGui::RadioButton("Pin last sent data.", &pin_sent_line);
        //  
        //      server.previewLocalData();
        //  }
        //  else {
        //      dummy_subscriber.showReceivedData();
        //  }

        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}