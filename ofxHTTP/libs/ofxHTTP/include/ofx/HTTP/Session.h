// =============================================================================
//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include <string>
#include <map>
#include "Poco/Mutex.h"
#include "Poco/Timestamp.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Net/HTTPCookie.h"
#include "Poco/Any.h"
#include "ofTypes.h"
#include "ofUtils.h"


namespace ofx {
namespace HTTP {


class Session
{
public:
    typedef std::shared_ptr<Session> SharedPtr;

    Session(const Poco::UUID& uuid = Poco::UUIDGenerator::defaultGenerator().createRandom(),
            const Poco::Timestamp& lastModified = Poco::Timestamp());

    virtual ~Session();

    const Poco::UUID& getId() const;

    const Poco::Timestamp getLastModified() const;

    bool has(const std::string& hashKey) const;
    void put(const std::string& hashKey, const Poco::Any& hashValue);

    Poco::Any get(const std::string& hashKey) const;

    static SharedPtr makeShared(const Poco::UUID& uuid = Poco::UUIDGenerator::defaultGenerator().createRandom(),
                                const Poco::Timestamp& lastModified = Poco::Timestamp())
    {
        return SharedPtr(new Session(uuid, lastModified));
    }

private:
    Session(const Session&);
	Session& operator = (const Session&);

    typedef std::map<std::string, Poco::Any> SessionData;

    SessionData _sessionData;

    Poco::UUID _uuid;

    mutable Poco::Timestamp _lastModified;

    mutable Poco::FastMutex _mutex;
    
};


} } // namespace ofx::HTTP
