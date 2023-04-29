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

#pragma once

#include "src/environment/standalone/StandAloneEnvironment.h"
#include <winsock2.h>
#include <windows.h>
#include "IngamePanelServer.h"

namespace msfs {

class IngamePanelEnvironment : public avitab::StandAloneEnvironment
{
public:
    IngamePanelEnvironment();
    virtual ~IngamePanelEnvironment();
    
    void eventLoop();

    std::shared_ptr<avitab::LVGLToolkit> createGUIToolkit() override;
    unsigned encodeBMP(std::vector<unsigned char> &bmp);

    avitab::AircraftID getActiveAircraftCount() override;
    avitab::Location getAircraftLocation(avitab::AircraftID id) override;

    void updateAircraftLocation(float x, float y, float a, float h);
    void updateMouseState(int x, int y, int button);
    void updateWheelState(bool up);

private:
    void                            resetLocations();

    std::mutex                      stateMutex;
    avitab::Location                userLocation;
    std::vector<avitab::Location>   otherLocations;

private:
    void                    startPanelServer();
    void                    stopPanelServer();
    PanelServer             panelServer;

};

} /* namespace msfs */
