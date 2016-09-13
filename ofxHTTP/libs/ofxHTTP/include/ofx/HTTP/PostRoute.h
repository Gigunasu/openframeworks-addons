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


#include "ofx/HTTP/BaseRoute.h"
#include "ofx/HTTP/PostRouteEvents.h"
#include "ofx/HTTP/PostRouteHandler.h"
#include "ofx/HTTP/PostRouteSettings.h"


namespace ofx {
namespace HTTP {


/// \brief A route for handing HTTP POST requests. 
class PostRoute: public BaseRoute_<PostRouteSettings>
{
public:
    /// \brief A typedef for a shared pointer.
    typedef std::shared_ptr<PostRoute> SharedPtr;

    /// \brief A typedef for a weak pointer.
    typedef std::weak_ptr<PostRoute> WeakPtr;

    /// \brief A typedef for the WebSocketRouteSettings.
    typedef PostRouteSettings Settings;

    PostRoute(const Settings& settings = Settings());
    
    virtual ~PostRoute();

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

    virtual PostRouteEvents& getEventsRef();

    PostRouteEvents events;

    template<class ListenerClass>
    void registerPostEvents(ListenerClass* listener);

    template<class ListenerClass>
    void unregisterPostEvents(ListenerClass* listener);

    static SharedPtr makeShared(const Settings& settings = Settings())
    {
        return SharedPtr(new PostRoute(settings));
    }

};


template<class ListenerClass>
void PostRoute::registerPostEvents(ListenerClass* listener)
{
    ofAddListener(events.onHTTPPostEvent, listener, &ListenerClass::onHTTPPostEvent);
    ofAddListener(events.onHTTPFormEvent, listener, &ListenerClass::onHTTPFormEvent);
    ofAddListener(events.onHTTPUploadEvent, listener, &ListenerClass::onHTTPUploadEvent);
}


template<class ListenerClass>
void PostRoute::unregisterPostEvents(ListenerClass* listener)
{
    ofRemoveListener(events.onHTTPPostEvent, listener, &ListenerClass::onHTTPPostEvent);
    ofRemoveListener(events.onHTTPFormEvent, listener, &ListenerClass::onHTTPFormEvent);
    ofRemoveListener(events.onHTTPUploadEvent, listener, &ListenerClass::onHTTPUploadEvent);
}


} } // namespace ofx::HTTP
