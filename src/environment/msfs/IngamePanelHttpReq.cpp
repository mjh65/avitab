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

#include "IngamePanelHttpReq.h"
#include "src/Logger.h"

#define HTTP_VERBOSE_LOGGING 0

namespace msfs {

HttpReq::HttpReq()
{
}

HttpReq::~HttpReq()
{
}

bool HttpReq::feedData(char *fragment, int n)
{
    LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"HttpReq: recv(%d bytes)", n);
    while(n) {
        switch(feedState) {
        case kMethod:
            feedMethod(fragment, n);
            break;
        case kUrl:
            feedUrl(fragment, n);
            break;
        case kVersion:
            feedVersion(fragment, n);
            break;
        case kHeader:
            feedHeader(fragment, n);
            break;
        case kComplete:
            LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"Ignoring (unexpected) message body in http request");
            return true;
        case kError:
            LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"Ignoring rest of http request after error");
            return true;
        default:
            LOG_ERROR("No handler for state %d in http request parser", feedState);
            return true;
        }
    }
    return (feedState < kComplete) ? false : true;
}

void HttpReq::feedMethod(char *&req, int &n)
{
    while ((n > 0) && !isspace(*req)) {
        working.push_back(*req); ++req; --n;
    }
    if (n) {
        while ((n > 0) && isspace(*req)) { ++req; --n; }
        LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"HttpReq: extracted method %s (%d remaining)", working.c_str(), n);
        method = working;
        working.clear();
        feedState = kUrl;
    }
}

void HttpReq::feedUrl(char *&req, int &n)
{
    while ((n > 0) && !isspace(*req)) {
        working.push_back(*req); ++req; --n;
    }
    if (n) {
        while ((n > 0) && isspace(*req)) { ++req; --n; }
        LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"HttpReq: extracted url %s (%d remaining)", working.c_str(), n);
        url = working;
        working.clear();
        feedState = kVersion;
    }
}

inline bool iseol(const char c) { return ((c == '\r') && (c == '\n')); }

void HttpReq::feedVersion(char *&req, int &n)
{
    while ((n > 0) && !isspace(*req) && !iseol(*req)) {
        working.push_back(*req); ++req; --n;
    }
    if (n) {
        while ((n > 0) && (isspace(*req) || iseol(*req)) ) { ++req; --n; }
        LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"HttpReq: extracted version %s (%d remaining)", working.c_str(), n);
        version = working;
        working.clear();
        feedState = kHeader;
    }
}

void HttpReq::feedHeader(char *&req, int &n)
{
    // extract everything to the end of the line
    while ((n > 0) && (*req != '\r') && (*req != '\n')) {
        working.push_back(*req); ++req; --n;
    }
    // is enough left in the fragment to check for eol
    if (n >= 2) {
        if ((*req == '\r') && (*(req+1) == '\n')) {
            req += 2; n -= 2;
            if (working.length() == 0) {
                // empty line - end of request
                LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"empty header (%d remaining)", n);
                feedState = kComplete;
            } else {
                LOG_VERBOSE(HTTP_VERBOSE_LOGGING,"added header %s (%d remaining)", working.c_str(), n);
                headers.push_back(working);
                working.clear();
            }
        } else {
            // http protocol error
            feedState = kError;
        }
    }
}

void HttpReq::extractQueryStrings() {
    if (queryStrings.size()) return; // don't extract more than once
    // extract the query string part of the url, everything after the '?'
    std::string qstr;
    auto qsep = url.find('?');
    if (qsep != std::string::npos) {
        qstr = url.substr(qsep+1);
    }
    // extract each of the parameters in turn, separated by '&'
    while (qstr.size()) {
        // extract the next parameter
        auto psep = qstr.find('&');
        auto pstr = qstr.substr(0,psep);
        if (psep != std::string::npos) ++psep;
        qstr.erase(0,psep);
        // split the parameter into name and value, separated by '='
        auto nvsep = pstr.find('=');
        if (nvsep == std::string::npos) {
            queryStrings[pstr] = "";
        } else {
            auto esep = pstr.find('=');
            auto pname = pstr.substr(0,esep);
            if (esep != std::string::npos) ++esep;
            queryStrings[pname] = pstr.substr(esep);
        }
    }
}

bool HttpReq::hasError() const
{
    return (feedState == kError);
}

bool HttpReq::keepAlive() const
{
    // returns true if headers included 'Connection: keep-alive'
    for (auto i = headers.begin(); i != headers.end(); ++i) {
        if ((*i).find("keep-alive") != std::string::npos) {
            if ((*i).find("onnection:") != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}
bool HttpReq::getQueryString(const char *q, std::string &val)
{
    bool exists = false;
    extractQueryStrings();
    if (queryStrings.find(q) != queryStrings.end()) {
        val = queryStrings[q];
        exists = true;
    }
    return exists;
}

std::string HttpReq::getUrl() const
{
    return url;
}


}
