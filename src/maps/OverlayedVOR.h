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
#ifndef SRC_MAPS_OVERLAYED_VOR_H_
#define SRC_MAPS_OVERLAYED_VOR_H_

#include "OverlayedFix.h"
#include "OverlayedDME.h"

namespace maps {

class OverlayedVOR : public OverlayedFix {

public:
    OverlayedVOR(IOverlayHelper *h, const world::Fix *f);

    void configure(const OverlayConfig &cfg, const world::Location &loc) override;
    void drawGraphic() override;
    void drawText(bool detailed) override;

    Hotspot getClickHotspot() const override;

private:
    const world::VOR * const navVOR;
    std::unique_ptr<OverlayedDME> linkedDME;

    static constexpr const int CIRCLE_RADIUS = 70;
    static constexpr const int MARGIN = CIRCLE_RADIUS;
};

} /* namespace maps */

#endif /* SRC_MAPS_OVERLAYED_VOR_H_ */
