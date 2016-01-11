#include "easywsclient.hpp"
//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#include <assert.h>
#include <stdio.h>
#include <string>

using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

void handle_message(const std::string & message)
{
    printf("<<< %s\n", message.c_str());
    if (message == "world") { ws->close(); }
}

int main()
{
    ws = WebSocket::from_url("ws://10.242.5.212:8080");
    assert(ws);
    std::string data = "{ \"data\": { \"cardnumber\": \"042F1919721D80\", \"expiry\": \"\", \"pin\": \"1234\", \"roles\": [ { \"profile\": \"1\", \"type\": \"11\" } ], \"serviceprovider\": \"1001\", \"staffid\": \"5070\", \"type\": \"operator\", \"valid\": true }, \"event\": \"cardpresented\", \"name\": \"cardevent\", \"terminalid\": \"3\", \"type\": \"PUT\", \"userid\": null }";
    
    printf(">>> %s\n", data.c_str());
    ws->send(data.c_str());

    while (ws->getReadyState() != WebSocket::CLOSED) {
      ws->poll();
      ws->dispatch(handle_message);
    }
    delete ws;

    return 0;
}
