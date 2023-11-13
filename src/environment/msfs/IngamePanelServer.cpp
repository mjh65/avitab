/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2023 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "IngamePanelServer.h"
#include <Windows.h>
#include <WS2tcpip.h>
#include <sstream>
#include "IngamePanelEnvironment.h"
#include "IngamePanelHttpReq.h"
#include "src/Logger.h"
#include "src/platform/CrashHandler.h"

#define SERVER_VERBOSE_LOGGING 0

namespace msfs {

PanelServer::PanelServer(IngamePanelEnvironment *e)
:   owner(e)
{
    reqBuffer = std::make_unique<char[]>(REQ_BUFFER_SIZE);
}

PanelServer::~PanelServer()
{
    stop();
}

void PanelServer::stop() {
    serverKeepAlive = false;
    if (panelSocket != INVALID_SOCKET) {
        shutdown(panelSocket, SD_BOTH);
        closesocket(panelSocket);
    }
    if (serverThread) {
        serverThread->join();
        serverThread.reset();
    }
}

int PanelServer::start(int port) {
    stop(); // stop old instance (if any)

    LOG_INFO(1, "Starting MSFS-PanelServer");

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    std::ostringstream pstr;
    pstr << port;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    int iResult = getaddrinfo(NULL, pstr.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        LOG_ERROR("getaddrinfo failed with error: %d", iResult);
        return -2;
    }

    // Create a SOCKET for the server to listen for client connections.
    httpService = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (httpService == INVALID_SOCKET) {
        LOG_ERROR("socket failed with error: %ld", WSAGetLastError());
        freeaddrinfo(result);
        return -2;
    }

    // Setup the TCP listening socket
    iResult = bind( httpService, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        LOG_ERROR("bind failed with error: %d", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(httpService);
        return -1;
    }

    freeaddrinfo(result);

    iResult = listen(httpService, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        LOG_ERROR("listen failed with error: %d", WSAGetLastError());
        closesocket(httpService);
        return -2;
    }

    serverKeepAlive = true;
    serverThread = std::make_unique<std::thread>(&PanelServer::listenLoop, this);

    return 0;
}

void PanelServer::listenLoop() {
    crash::ThreadCookie crashCookie;

    sockaddr_in clientAddr {};
    socklen_t clientLen = sizeof(clientAddr);

    fd_set readSet;

    timeval timeout{};
    timeout.tv_usec = 1000 * 500;

    while (serverKeepAlive) {
        FD_ZERO(&readSet);
        FD_SET(httpService, &readSet);
        int fdMax = httpService;

        if (select(fdMax + 1, &readSet, nullptr, nullptr, &timeout) < 0) {
            break;
        }

        if (FD_ISSET(httpService, &readSet)) {
            LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Accepting msfs panel client");
            panelSocket = accept(httpService, (sockaddr *) &clientAddr, &clientLen);
            connectionLoop();
            closesocket(panelSocket);
            panelSocket = INVALID_SOCKET;
        }
    }

    closesocket(httpService);
    LOG_INFO(1, "Shutdown MSFS-PanelServer");
}

void PanelServer::connectionLoop()
{
    LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Panel has connected ...");

    bool keepConnection = true;
    while (serverKeepAlive && keepConnection) {
        auto req = std::make_unique<HttpReq>();
        while (serverKeepAlive) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(panelSocket, &readSet);
            int fdMax = panelSocket;

            timeval timeout{};
            timeout.tv_usec = 1000 * 1000;
            if (select(fdMax + 1, &readSet, nullptr, nullptr, &timeout) < 0) {
                keepConnection = false;
                break;
            }
            if (FD_ISSET(panelSocket, &readSet)) {
                int n = recv(panelSocket, reqBuffer.get(), REQ_BUFFER_SIZE, 0);
                if (n <= 0) {
                    keepConnection = false;
                    break;
                }
                if (req->feedData(reqBuffer.get(), n)) {
                    keepConnection = processRequest(req.get());
                    break;
                }
            }
        }
    }

    // cleanup
    (void)shutdown(panelSocket, SD_BOTH);
    closesocket(panelSocket);
    LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Panel disconnected");
}

bool PanelServer::processRequest(HttpReq *req)
{
    bool keepAlive = false;
    auto opcode = req->getUrl().substr(1,1);
    std::ostringstream header;
    std::vector<unsigned char> content;
    header << "HTTP/1.1 ";
    if (req->hasError()) {
        header << "404 NOT FOUND\r\n";
        header << "Content-Type: text/plain\r\n";
        std::string reply("AviTab: error");
        content.resize(reply.size());
        std::copy(reply.begin(), reply.end(), content.begin());
    } else {
        keepAlive = req->keepAlive();

        // if provided, get aircraft position and send this into the avitab engine
        std::string longitude, latitude, altitude, heading;
        if (req->getQueryString("lt",latitude) && req->getQueryString("ln",longitude) && req->getQueryString("al",altitude) && req->getQueryString("hg",heading)) {
            LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Got position: %s,%s,%s,%s", latitude.c_str(), longitude.c_str(), altitude.c_str(), heading.c_str());
            owner->updateAircraftLocation(std::stof(longitude), std::stof(latitude), std::stof(altitude), std::stof(heading));
        }

        // if provided, get mouse information and send it to the driver
        std::string mx, my, button;
        if (req->getQueryString("mx",mx) && req->getQueryString("my",my) && req->getQueryString("mb",button)) {
            LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Got mouse state: %s %s %s", mx.c_str(), my.c_str(), button.c_str());
            owner->updateMouseState(std::stoi(mx), std::stoi(my), std::stoi(button));
        }

        // if provided, get wheel information and send it to the driver
        std::string wheel;
        auto wheelUp = req->getQueryString("wu",wheel);
        auto wheelDown = req->getQueryString("wd",wheel);
        if (wheelUp || wheelDown) {
            LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Got wheel event: %s", wheelUp ? "up" : "down");
            owner->updateWheelState(wheelUp);
        }

        // if provided, get other aircraft information and pass it to the environment
        std::string traffic;
        if (req->getQueryString("tr",traffic)) {
            LOG_VERBOSE(SERVER_VERBOSE_LOGGING, "Got traffic information: %s", traffic.c_str());
            owner->updateTraffic(traffic);
        }

        if (opcode == "f") {
            // request for frame
            header << "200 OK\r\n";
            header << "Content-Type: image/bmp\r\n";
            owner->encodeBMP(content);
        } else if (opcode == "m") {
            header << "200 OK\r\n";
            header << "Content-Type: text/plain\r\n";
            header << "Access-Control-Allow-Origin: *\r\n";
            std::string reply("OKEY-DOKEY");
            content.resize(reply.size());
            std::copy(reply.begin(), reply.end(), content.begin());
        } else {
            header << "404 NOT FOUND\r\n";
            header << "Content-Type: text/plain\r\n";
            std::string reply("AviTab: unknown opcode");
            content.resize(reply.size());
            std::copy(reply.begin(), reply.end(), content.begin());
        }
    }
    header << "Content-Length: " << content.size() << "\r\n";
    if (!keepAlive) { header << "Connection: close\r\n"; }
    header << "\r\n";

    (void)send(panelSocket, header.str().c_str(), header.str().length(), 0);
    (void)send(panelSocket, reinterpret_cast<char *>(content.data()), content.size(), 0);

    // return false if the connection should be held open
    return keepAlive;
}


} /* namespace msfs */
