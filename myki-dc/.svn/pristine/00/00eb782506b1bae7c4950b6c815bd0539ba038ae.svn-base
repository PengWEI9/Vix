/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief BinaryPrinterMsg class interface.
 */

#ifndef DC_BINARYPRINTERMSG_H
#define DC_BINARYPRINTERMSG_H

#include "printermsg.h"
#include <ipc.h>
#include <stdexcept>

namespace PrinterBackup {

    /**
     * @brief Encapsulates a binary printer message, eg for tpurse load or 
     * the driver shift report.
     */
    template <typename T>
    class BinaryPrinterMsg : public PrinterMsg
    {
        private:
            T m_msg;

        public:
            BinaryPrinterMsg(const T &msg) : m_msg(msg) {}

            /**
             * @brief Sends print request to printer via IPC.
             * @param printerTaskId The specific IPC queue ID for the printe 
             * task.
             * @throw runtime_error If there's an IPC error.
             */
            void print(IpcMessageQueueId printerTaskId)
            {
                if (IpcSend(printerTaskId, &m_msg, sizeof(m_msg)) != 0)
                {
                    throw std::runtime_error("IpcSend failed");
                }
            }
    };
}

#endif
