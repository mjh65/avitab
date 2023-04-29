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

#include <string>
#include <vector>
#include <map>

namespace msfs {

class HttpReq {
public:
    HttpReq();
    virtual ~HttpReq();

    bool feedData(char *fragment, int n); // returns true if request headers have all been received
    bool hasError() const; // returns true if headers could not be parsed
    bool keepAlive() const; // returns true if headers included 'Connection: keep-alive'

    bool getQueryString(const char *q, std::string &val);
    std::string getUrl() const;

private:
    enum { kMethod, kUrl, kVersion, kHeader, kComplete, kError };

    void feedMethod(char *&req, int &n);
    void feedUrl(char *&req, int &n);
    void feedVersion(char *&req, int &n);
    void feedHeader(char *&req, int &n);

    void extractQueryStrings();

private:
    int feedState = kMethod;
    std::string working;
    std::string method;
    std::string url;
    std::string version;
    std::map<std::string, std::string> queryStrings;
    std::vector<std::string> headers;
};

} /* namespace msfs */
