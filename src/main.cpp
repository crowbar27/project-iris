#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <highfive/H5Easy.hpp>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <future>
#include <stdlib.h>
#include <stdio.h>



struct DataServer {
    void startPublisher(zmq::context_t* ctx) {
        // TODO simulate publishing of live data

        //  Prepare publisher
        zmq::socket_t publisher(*ctx, zmq::socket_type::pub);
        publisher.bind("tcp://127.0.0.1:5555");

        is_running_ = true;
        while (is_running_) {
            //  Write three messages, each with an envelope and content
            publisher.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
            publisher.send(zmq::str_buffer("Debug message in A envelope"));

            if (local_data_file_ != nullptr)
            {
                publisher.send(zmq::str_buffer("B"), zmq::send_flags::sndmore);
                publisher.send(zmq::message_t(local_data_.front()));
            }

            // TODO set according to update rate
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void stopPublisher() {
        is_running_ = false;
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

            // we convert the hdf5 dataset to a single dimension vector
            dataset.read(local_data_);

            //local_data_ = H5Easy::load<std::vector<double>>((*local_data_file_), "/meas_data");
        }
    }

    void previewLocalData() {
        // TODO draw local data as imgui table

        if (local_data_file_ != nullptr)
        {
            ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;
            
            if (ImGui::Begin("Local HDF5 data)")) {
                if (ImGui::BeginTable("/meas_data", local_data_.front().size(), flags))
                {
                    ImGuiListClipper clipper;
                    clipper.Begin(local_data_.size());
                    while (clipper.Step()) {
                        for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                            ImGui::TableNextRow();
                            auto& row_data = local_data_[row];
                            for (int column = 0; column < local_data_.front().size(); column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                //ImGui::Text("Row %d Column %d", row, column);
                                ImGui::Text("%04.4f", row_data[column]);
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::End();
        }
    }

    std::unique_ptr<H5Easy::File> local_data_file_;
    std::vector<std::vector<double>> local_data_;

    float send_rate_;
    size_t current_data_row_;

    std::atomic_bool is_running_;
};

struct DummySubscriber {
    // TODO simulate receiving live data

    void startSubscriber(zmq::context_t* ctx) {
        //  Prepare subscriber
        zmq::socket_t subscriber(*ctx, zmq::socket_type::sub);
        subscriber.connect("tcp://127.0.0.1:5555");

        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        //  Opens "A" and "B" envelopes
        subscriber.set(zmq::sockopt::subscribe, "A");
        subscriber.set(zmq::sockopt::subscribe, "B");

        is_running_ = true;
        while (is_running_) {
            // Receive all parts of the message
            std::vector<zmq::message_t> recv_msgs;
            zmq::recv_result_t result =
                zmq::recv_multipart(subscriber, std::back_inserter(recv_msgs));
            assert(result && "recv failed");
            assert(*result == 2);

            if (recv_msgs[0].to_string() == "A"){
                std::cout << "[" << recv_msgs[0].to_string() << "] "
                    << recv_msgs[1].to_string() << std::endl;
            }
            else if (recv_msgs[0].to_string() == "B") {
                std::cout << "[" << recv_msgs[0].to_string() << "] "
                    << recv_msgs[1] << std::endl;

                received_data_.emplace_back(std::vector<double>(recv_msgs[1].data<double>(), recv_msgs[1].data<double>() + (recv_msgs[1].size() / sizeof(double))));
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

        if(!received_data_.empty()){
            if (ImGui::Begin("Local HDF5 data)")) {
                if (ImGui::BeginTable("/meas_data", received_data_.front().size(), flags))
                {
                    ImGuiListClipper clipper;
                    clipper.Begin(received_data_.size());
                    while (clipper.Step()) {
                        for (auto row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                            ImGui::TableNextRow();
                            auto& row_data = received_data_[row];
                            for (int column = 0; column < received_data_.front().size(); column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                //ImGui::Text("Row %d Column %d", row, column);
                                ImGui::Text("%04.4f", row_data[column]);
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

    std::vector<std::vector<double>> received_data_;
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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    zmq::context_t ctx;

    DataServer server;
    DummySubscriber dummy_subscriber;

    bool server_mode = false;
    auto a = std::async(std::launch::async, &DummySubscriber::startSubscriber, &dummy_subscriber, &ctx);
 
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

        bool control_panel_is_open = true;
        ImGui::Begin("ControlPanel", &control_panel_is_open);
        if (ImGui::RadioButton("Server", server_mode)) {
            if (!server_mode) {
                // stop subscriber and start publisher server
                dummy_subscriber.stopSubscriber();
                a = std::async(std::launch::async, &DataServer::startPublisher, &server, &ctx);
            }
            server_mode = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Subscriber", !server_mode)) {
            if (server_mode) {
                // stop publisher server and start subscriver
                server.stopPublisher();
                a = std::async(std::launch::async, &DummySubscriber::startSubscriber, &dummy_subscriber, &ctx);
            }
            server_mode = false;
        }
        ImGui::Separator();

        if (server_mode) {

            // Loading local data
            static char buf1[128] = "Z:/05 Projektarbeit/B05/D1244-Sensordaten/2023-01-17/08-51-23.hdf5";
            ImGui::InputText("Local filepath", buf1, 128);
            if (ImGui::Button("Load")) {
                server.loadLocalDataFromFile(std::string(buf1));
            }

            server.previewLocalData();
        }
        else {
            dummy_subscriber.showReceivedData();
        }

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