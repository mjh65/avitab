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
#include <nlohmann/json.hpp>
#include "Chart.h"
#include "src/Logger.h"

namespace navigraph {

Chart::Chart(const nlohmann::json &json) {
    fileDay = json["file_day"];
    fileNight = json["file_night"];
    icao = json["icao_airport_identifier"];
    section = json["type"]["section"];
    desc = json["procedure_identifier"];
    index = json["index_number"];

    try {
        double width = json["bbox_local"][2];
        double height = json["bbox_local"][1];

        geoRef.lon1 = json["planview"]["bbox_geo"][0];
        geoRef.lat1 = json["planview"]["bbox_geo"][1];
        geoRef.lon2 = json["planview"]["bbox_geo"][2];
        geoRef.lat2 = json["planview"]["bbox_geo"][3];

        geoRef.x1 = json["planview"]["bbox_local"][0];
        geoRef.y1 = json["planview"]["bbox_local"][1];
        geoRef.x2 = json["planview"]["bbox_local"][2];
        geoRef.y2 = json["planview"]["bbox_local"][3];

        geoRef.x1 /= width;
        geoRef.x2 /= width;
        geoRef.y1 /= height;
        geoRef.y2 /= height;

        geoRef.valid = true;
    } catch (...) {
        geoRef.valid = false;
    }
}

ChartGEOReference Chart::getGeoReference() const {
    return geoRef;
}

std::string Chart::getICAO() const {
    return icao;
}

std::string Chart::getIndex() const {
    return index;
}

std::string Chart::getDescription() const {
    return desc;
}

std::string Chart::getFileDay() const {
    return fileDay;
}

std::string Chart::getFileNight() const {
    return fileNight;
}

bool Chart::isLoaded() const {
    return imgDay && imgNight;
}

void Chart::attachImages(std::shared_ptr<img::Image> day, std::shared_ptr<img::Image> night) {
    imgDay = day;
    imgNight = night;
}

std::shared_ptr<img::Image> Chart::getDayImage() const {
    return imgDay;
}

std::shared_ptr<img::Image> Chart::getNightImage() const {
    return imgNight;
}

} /* namespace navigraph */