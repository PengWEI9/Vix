/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief JsonPrinterMsg class interface.
 */

#ifndef DC_JSONPRINTERMSG_H
#define DC_JSONPRINTERMSG_H

#include "printermsg.h"
#include <ipc.h>
#include <json/json.h>
#include <string>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace PrinterBackup {

    /**
     * @brief Encapsulates a JSON printer message.
     */
    class JsonPrinterMsg : public PrinterMsg
    {
        private:
            Json::Value m_msg;

        public:
            JsonPrinterMsg(const Json::Value &msg) : m_msg(msg) {}

            /**
             * @brief Sends print request to printer via IPC.
             * @param printerTaskId The specific IPC queue ID for the printe 
             * task.
             * @throw runtime_error If there's an IPC error.
             */
            void print(IpcMessageQueueId printerTaskId) 
            {
                std::string data = boost::lexical_cast<std::string>(m_msg);

                IPC_header_t hdr = { IPC_PRINTER_JSON, DC_PRINTER_TASK };

                if (IpcSendMulti(printerTaskId, &hdr, data.c_str(), 
                            data.size()) != 0)
                {
                    throw std::runtime_error("IpcSendMulti failed");
                }
            };
    };
}

#endif
