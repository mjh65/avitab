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

#include "IngamePanelGuiDriver.h"

namespace msfs {

IngamePanelGuiDriver::~IngamePanelGuiDriver()
{

}

void IngamePanelGuiDriver::readPointerState(int &x, int &y, bool &pressed)
{
    x = panelMouseX;
    y = panelMouseY;
    pressed = panelMousePressed;
}

void IngamePanelGuiDriver::setPointerState(int x, int y, bool pressed)
{
    panelMouseX = x;
    panelMouseY = y;
    panelMousePressed = pressed;
}

int IngamePanelGuiDriver::getWheelDirection() {
    int d = panelWheelDir;
    panelWheelDir = 0;
    return d;
}

void IngamePanelGuiDriver::setWheelDirection(bool up) {
    if (up) panelWheelDir = 1; else panelWheelDir = -1;
}

unsigned IngamePanelGuiDriver::encodeBMP(std::vector<unsigned char> &bmp)
{
    std::lock_guard<std::mutex> lock(driverMutex);

    bmp.resize(14 + 40 + (3 * width() * height()));

    static const unsigned char hdr[] = {
                                    0x42, 0x4d,                 // signature
                                    0x36, 0x94, 0x11, 0x00,     // file length
                                    0x00, 0x00,                 // res1
                                    0x00, 0x00,                 // res2
                                    0x36, 0x00, 0x00, 0x00,     // offset of pixel map
                                    0x28, 0x00, 0x00, 0x00,     // length of DIB header
                                    0x20, 0x03, 0x00, 0x00,     // width in pixels (800)
                                    0xe0, 0x01, 0x00, 0x00,     // height in pixels (480)
                                    0x01, 0x00,                 // # colour planes (1)
                                    0x18, 0x00,                 // # bits per pixel (24)
                                    0x00, 0x00, 0x00, 0x00,     // compression method
                                    0x00, 0x94, 0x11, 0x00,     // image size in bytes
                                    0xc3, 0x0e, 0x00, 0x00,     // horizontal resolution (pixels/m)
                                    0xc3, 0x0e, 0x00, 0x00,     // vertical resolution (pixels/m)
                                    0x00, 0x00, 0x00, 0x00,     // # colours in palette
                                    0x00, 0x00, 0x00, 0x00      // # important colours in palette
                                };

    memcpy(bmp.data(), hdr, sizeof(hdr));

    // TODO - update header fields based on width and height (although these don't change, so not exactly urgent!)
    unsigned char *d = bmp.data() + sizeof(hdr);

    for (int y = 1; y <= height(); ++y) {
        unsigned char *s = reinterpret_cast<unsigned char*>(data()) + (4 * width() * (height() - y));
        for (int x = 0; x < width(); ++x) {
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            s++;
        }
    }

    return 0;
}

}
