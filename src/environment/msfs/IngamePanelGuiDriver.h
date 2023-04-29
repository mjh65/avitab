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

#include "../standalone/GlfwGUIDriver.h"

namespace msfs {

class IngamePanelGuiDriver: public avitab::GlfwGUIDriver {
public:
    ~IngamePanelGuiDriver();

    void readPointerState(int &x, int &y, bool &pressed) override;
    void setPointerState(int x, int y, bool pressed);

    int getWheelDirection() override;
    void setWheelDirection(bool up);

    unsigned encodeBMP(std::vector<unsigned char> &png);

private:
    std::atomic_int panelMouseX {0}, panelMouseY {0}, panelWheelDir {0};
    bool panelMousePressed {false};

};

} /* namespace avitab */
