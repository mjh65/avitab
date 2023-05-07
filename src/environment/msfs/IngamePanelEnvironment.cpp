/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2018 Folke Will <folko@solhost.org>
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
#include <cmath>
#include "IngamePanelEnvironment.h"
#include "IngamePanelGuiDriver.h"
#include "src/Logger.h"
#include "src/platform/Platform.h"
#include "src/libxdata/world/World.h"
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#define MSFS_VERBOSE_LOGGING 0

namespace msfs {

IngamePanelEnvironment::IngamePanelEnvironment()
:   StandAloneEnvironment(),
    panelServer(this)
{
    resetLocations();
}

IngamePanelEnvironment::~IngamePanelEnvironment()
{
    stopPanelServer();
}

std::shared_ptr<avitab::LVGLToolkit> IngamePanelEnvironment::createGUIToolkit() {
    driver = std::make_shared<IngamePanelGuiDriver>();
    return std::make_shared<avitab::LVGLToolkit>(driver);
}

unsigned IngamePanelEnvironment::encodeBMP(std::vector<unsigned char> &bmp)
{
    return (std::dynamic_pointer_cast<IngamePanelGuiDriver>(driver))->encodeBMP(bmp);
}

void IngamePanelEnvironment::eventLoop()
{
    startPanelServer();
    while (driver->handleEvents()) {
        runEnvironmentCallbacks();
        lastDrawTime = driver->getLastDrawTime() / 1000.0;
    }
    driver.reset();
}

avitab::AircraftID IngamePanelEnvironment::getActiveAircraftCount()
{
    std::lock_guard<std::mutex> lock(stateMutex);
    return 1 + otherLocations.size();
}

avitab::Location IngamePanelEnvironment::getAircraftLocation(avitab::AircraftID id)
{
    std::lock_guard<std::mutex> lock(stateMutex);
    if (id == 0) {
        return userLocation;
    } else {
        return otherLocations[id-1];
    }
}

void IngamePanelEnvironment::updateAircraftLocation(float x, float y, float a, float h)
{
    LOG_VERBOSE(MSFS_VERBOSE_LOGGING,"updateAircraftLocation(%f,%f,%f,%f)",x,y,a,h);
    std::lock_guard<std::mutex> lock(stateMutex);
    userLocation.latitude = y;
    userLocation.longitude = x;
    userLocation.heading = h;
    userLocation.elevation = a;
}

void IngamePanelEnvironment::updateMouseState(int x, int y, int button)
{
    (std::dynamic_pointer_cast<IngamePanelGuiDriver>(driver))->setPointerState(x, y, (button != 0));
}

void IngamePanelEnvironment::updateWheelState(bool up)
{
    (std::dynamic_pointer_cast<IngamePanelGuiDriver>(driver))->setWheelDirection(up);
}

void IngamePanelEnvironment::updateTraffic(std::string traffic)
{
    std::vector<float> nums;
    while (traffic.size()) {
        size_t i = 0, j;
        float lat = std::stof(traffic.substr(i), &j);
        i += (j+1);
        float lon = std::stof(traffic.substr(i), &j);
        i += (j+1);
        float alt = std::stof(traffic.substr(i), &j);
        i += (j+1);
        float hdg = std::stof(traffic.substr(i), &j);
        i += j;
        if ((i < traffic.size()) && (traffic[i] == '_')) {
            nums.push_back(lat);
            nums.push_back(lon);
            nums.push_back(alt);
            nums.push_back(hdg);
            traffic.erase(0, i+1);
        } else {
            break;
        }
    }

    std::lock_guard<std::mutex> lock(stateMutex);
    otherLocations.clear();
    for (auto i = nums.begin(); i != nums.end(); ) {
        avitab::Location loc;
        loc.latitude = *(i++);
        loc.longitude = *(i++);
        loc.elevation = *(i++);
        loc.heading = *(i++);
        otherLocations.push_back(loc);
    }
}

void IngamePanelEnvironment::resetLocations()
{
    std::lock_guard<std::mutex> lock(stateMutex);
    userLocation.latitude = 0.0;
    userLocation.longitude = 0.0;
    userLocation.heading = 0.0;
    userLocation.elevation = 0.0;
    otherLocations.clear();
}

void IngamePanelEnvironment::startPanelServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LOG_INFO(1, "Panel server: WSAStartup failed with error %d", WSAGetLastError());
        return;
    }
    int port = 26730; // base port
    while(panelServer.start(port)) {
        if (++port >= 26750) break;
    }
    if (port < 26750) {
        LOG_INFO(1, "Panel server started on port %d", port);
    } else {
        LOG_ERROR("Panel server did not start on any candidate port");
    }
}

void IngamePanelEnvironment::stopPanelServer()
{
    panelServer.stop();
    WSACleanup();
    LOG_INFO(1, "Panel server stopped");
}

} /* namespace msfs */
