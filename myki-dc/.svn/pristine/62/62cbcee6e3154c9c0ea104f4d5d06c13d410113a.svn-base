/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief PrinterMsg class interface.
 */

#ifndef DC_PRINTERMSG_H
#define DC_PRINTERMSG_H

#include "../typedefs.h"

namespace PrinterBackup {

    /**
     * @brief An abstract base class for printer messages.
     */
    class PrinterMsg
    {
        public:
            /**
             * @brief Sends print request to printer via IPC.
             * @param printerTaskId The specific IPC queue ID for the printe 
             * task.
             */
            virtual void print(IpcMessageQueueId printerTaskId) = 0;
            virtual ~PrinterMsg() {};
    };
}

#endif
