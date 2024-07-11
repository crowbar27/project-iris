#ifndef upd_multicast_receive_h
#define udp_multicast_receive_h

#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

#include <array>
#include <assert.h>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include "wil/resource.h"

namespace iris {

    namespace {
        /// <summary>
        /// Joins the multicast group with the specified <paramref name="address" />.
        /// </summary>
        inline void join_mcast_group(wil::unique_socket& socket,
            const IN_ADDR& group_address) {
            ip_mreq_source imr{ 0 };

            imr.imr_multiaddr.s_addr = group_address.S_un.S_addr;
            //imr.imr_sourceaddr.s_addr = source_address.S_un.S_addr;

            auto status = ::setsockopt(socket.get(),
                IPPROTO_IP,
                IP_ADD_MEMBERSHIP,
                reinterpret_cast<char*>(std::addressof(imr)),
                sizeof(imr));
            if (status == SOCKET_ERROR) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }
        }

        /// <summary>
        /// Leaves the multicast group with the specified <paramref name="address" />.
        /// </summary>
        inline void leave_mcast_group(wil::unique_socket& socket,
            const IN_ADDR& group_address) {
            ip_mreq_source imr{ 0 };

            imr.imr_multiaddr.s_addr = group_address.S_un.S_addr;
            //imr.imr_sourceaddr.s_addr = source_address.S_un.S_addr;

            auto status = ::setsockopt(socket.get(),
                IPPROTO_IP,
                IP_DROP_MEMBERSHIP,
                reinterpret_cast<char*>(std::addressof(imr)),
                sizeof(imr));
            if (status == SOCKET_ERROR) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }
        }
    }

    /// <summary>
    /// Find the <see cref="in_pktinfo" /> header in <paramref name="message" />.
    /// </summary>
    inline in_pktinfo* get_packet_info(WSAMSG& message) {
        auto header = WSA_CMSG_FIRSTHDR(&message);

        while (header != nullptr) {
            if ((header->cmsg_level == IPPROTO_IP)
                && (header->cmsg_type == IP_PKTINFO)) {
                return reinterpret_cast<in_pktinfo*>(WSA_CMSG_DATA(header));
            }
            else {
                header = WSA_CMSG_NXTHDR(&message, header);
            }
        }

        return nullptr;
    }

    /// <summary>
    /// Gets the <see cref="WSARecvMsg" /> extension function.
    /// </summary>
    inline LPFN_WSARECVMSG get_wsa_recv_msg(wil::unique_socket& socket) {
        LPFN_WSARECVMSG retval;
        GUID guid = WSAID_WSARECVMSG;
        DWORD size;

        auto status = ::WSAIoctl(socket.get(),
            SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guid, sizeof(guid),
            &retval, sizeof(retval),
            &size,
            nullptr,
            nullptr);
        if (status == SOCKET_ERROR) {
            throw std::system_error(::WSAGetLastError(), std::system_category());
        }

        return retval;
    }

    struct receive_context {
        WSABUF buffer;
        std::vector<char> control;
        in_addr peer;
        WSAMSG message;
        WSAOVERLAPPED overlapped;
        std::vector<char> received;
        wil::unique_socket socket;

        inline receive_context(const int port,
            const std::vector<std::string>& groups,
            const std::size_t max_msg = 560)
            : buffer({ 0 }),
            control(WSA_CMSG_SPACE(sizeof(in_pktinfo))),
            peer({ 0 }),
            message({ 0 }),
            overlapped({ 0 }),
            received(max_msg),
            socket(::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED)) {
            this->buffer.buf = this->received.data();
            this->buffer.len = static_cast<ULONG>(this->received.size());

            this->message.Control.buf = this->control.data();
            this->message.Control.len = static_cast<ULONG>(this->control.size());

            this->message.lpBuffers = &this->buffer;
            this->message.dwBufferCount = 1;

            {
                int value = 1;
                auto status = ::setsockopt(this->socket.get(),
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    reinterpret_cast<char*>(&value),
                    sizeof(value));
                if (status == SOCKET_ERROR) {
                    throw std::system_error(::WSAGetLastError(), std::system_category());
                }
            }

            {
                int value = 1;
                auto status = ::setsockopt(this->socket.get(),
                    IPPROTO_IP,
                    IP_PKTINFO,
                    reinterpret_cast<char*>(&value),
                    sizeof(value));
                if (status == SOCKET_ERROR) {
                    throw std::system_error(::WSAGetLastError(), std::system_category());
                }
            }

            {
                sockaddr_in address{ 0 };
                address.sin_family = AF_INET;
                address.sin_addr = { INADDR_ANY };
                address.sin_port = ::htons(port);

                auto status = ::bind(this->socket.get(),
                    reinterpret_cast<::sockaddr*>(&address),
                    sizeof(sockaddr));
                if (status == SOCKET_ERROR) {
                    throw std::system_error(::WSAGetLastError(), std::system_category());
                }
            }

            for (auto& g : groups) {
                IN_ADDR address{ 0 };

                if (!::InetPtonA(AF_INET, g.c_str(), &address)) {
                    throw std::system_error(::WSAGetLastError(), std::system_category());
                }

                join_mcast_group(this->socket, address);
            }
        }

        receive_context(const receive_context&) = delete;
        receive_context(receive_context&&) = default;
        receive_context& operator =(const receive_context&) = delete;
        receive_context& operator =(receive_context&&) = default;
    };

    /// <summary>
    /// Posts an asynchronous <paramref name="receive" /> operation on the
    /// socket of the given <see cref="receive_context" />.
    /// </summary>
    inline void post_receive(LPFN_WSARECVMSG receive, receive_context& r) {
        auto status = receive(r.socket.get(),
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

    /// <summary>
    /// Iterates over a range of <see cref="receive_context" />s and posts a
    /// an asynchronous <paramref name="receive" /> message operation on each
    /// of the respective sockets.
    /// </summary>
    template<class TIterator>
    void post_receive(LPFN_WSARECVMSG receive, TIterator begin, TIterator end) {
        for (auto it = begin; it != end; ++it) {
            post_receive(receive, *it);
        }
    }

    /// <summary>
    /// Sends datagrams with very useful data to 225.1.1.1, 226.1.1.1, 227.1.1.1
    /// and 228.1.1.1 until <paramref name="running" /> is set <c>false</c>.
    /// </summary>
    inline void push_data2(std::atomic<bool>& running) {
        try {
            std::array<sockaddr_in, 4> addresses;
            std::array<std::vector<char>, addresses.size()> data{
                std::vector<char>(560),
                std::vector<char>(464),
                std::vector<char>(368),
                std::vector<char>(272)
            };
            WSADATA wsaData{ 0 };

            if (::WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            auto wsaClean = wil::scope_exit([](void) {
                ::WSACleanup();
                });

            for (auto& a : addresses) {
                a.sin_family = AF_INET;
                a.sin_port = ::htons(4321);
            }

            if (!::InetPtonW(AF_INET, L"225.1.1.1", &addresses[0].sin_addr)) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }
            if (!::InetPtonW(AF_INET, L"226.1.1.1", &addresses[1].sin_addr)) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }
            if (!::InetPtonW(AF_INET, L"227.1.1.1", &addresses[2].sin_addr)) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }
            if (!::InetPtonW(AF_INET, L"228.1.1.1", &addresses[3].sin_addr)) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            wil::unique_socket socket(::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED));
            if (!socket) {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            WSABUF buffers[data.size()];
            std::mt19937 engine(::time(nullptr));
            std::uniform_real_distribution<double> distribution(0.0, 1024.0);
            auto rng = std::bind(distribution, engine);

            for (std::size_t i = 0; i < data.size(); ++i) {
                buffers[i].buf = data[i].data();
                buffers[i].len = static_cast<ULONG>(data[i].size());
                assert(buffers[i].len % sizeof(double) == 0);
                auto b = reinterpret_cast<double*>(buffers[i].buf);
                auto e = reinterpret_cast<double*>(buffers[i].buf + buffers[i].len);
                std::generate(b, e, rng);
            }

            while (running) {
                for (std::size_t i = 0; i < addresses.size(); ++i) {
                    DWORD length = 0;

                    auto status = ::WSASendTo(socket.get(),
                        buffers + i, 1,
                        &length,
                        0,
                        reinterpret_cast<const sockaddr*>(addresses.data() + i),
                        sizeof(addresses[i]),
                        nullptr,
                        nullptr);
                    if (status == SOCKET_ERROR) {
                        throw std::system_error(::WSAGetLastError(), std::system_category());
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        }
        catch (std::system_error& ex) {
            std::cerr << ex.what() << std::endl;
        }
    }

}

#endif // !upd_multicast_receive_h
