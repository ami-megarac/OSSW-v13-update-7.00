/*
Copyright (c) 2023, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "i3c_debug_handler.h"

void debug_i3c_rx(i3c_cmd* cmd)
{
    if (cmd->read_len > 0 && cmd->rx_buffer != NULL)
    {
        ASD_log_buffer(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
           cmd->rx_buffer, cmd->read_len, "[RX]");
    }
}

void debug_i3c_tx(i3c_cmd* cmd)
{
    if (cmd->msgType == opcode)
    {
        ASD_log(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
           "[TX OPCODE]: %02X ", cmd->opcode);
    }
    if (cmd->msgType == action)
    {
        ASD_log(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
           "[TX ACTION]: %02X ", cmd->action);
    }
    if (cmd->write_len > 0  && cmd->tx_buffer != NULL)
    {
        ASD_log_buffer(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
           cmd->tx_buffer, cmd->write_len, "[TX]");
    }
}

STATUS send_i3c_action(SPP_Handler* state, i3c_cmd *cmd)
{
    struct i3c_debug_action_ccc action_ccc;
    action_ccc.action = cmd->action;
    int ret;
    if (!state->spp_driver_handle)
    {
        ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                "Failed to use file descriptor:  %d\n", state->spp_driver_handle);
        return ST_ERR;
    }
    debug_i3c_tx(cmd);
    ret = ioctl(state->spp_driver_handle, I3C_DEBUG_IOCTL_DEBUG_ACTION_CCC,
                (int32_t*)&action_ccc);
    ASD_log(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
            "Ioctl debug action status: %i, errno=%i\n", ret, errno);
    debug_i3c_rx(cmd);

    if (ret < 0)
    {
        ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                "Failed to send Debug Action ioctl\n");
        return ST_ERR;
    }
    return ST_OK;
}

STATUS send_i3c_opcode(SPP_Handler* state, i3c_cmd *cmd)
{
    struct i3c_debug_opcode_ccc opcode_ccc = {0};
    opcode_ccc.opcode = cmd->opcode;
    int ret;
    if (!state->spp_driver_handle)
    {
        ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                "Failed to use file descriptor:  %d\n", state->spp_driver_handle);
        return ST_ERR;
    }
    if (cmd->write_len)
    {
        opcode_ccc.write_len = cmd->write_len;
        opcode_ccc.write_ptr = (uintptr_t)cmd->tx_buffer;
    }
    if (cmd->read_len)
    {
        opcode_ccc.read_len = cmd->read_len;
        opcode_ccc.read_ptr = (uintptr_t)cmd->rx_buffer;
    }

    debug_i3c_tx(cmd);
    ret = ioctl(state->spp_driver_handle, I3C_DEBUG_IOCTL_DEBUG_OPCODE_CCC,
                (int32_t*)&opcode_ccc);
    ASD_log(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
            "Ioctl debug opcode status: %i, errno=%i\n", ret, errno);
    debug_i3c_rx(cmd);
    if (ret < 0)
    {
        ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                "Failed to send Debug Opcode ioctl\n");
        return ST_ERR;
    }
    return ST_OK;
}

ssize_t receive_i3c(SPP_Handler* state, i3c_cmd *cmd)
{
    ssize_t read_ret=0;
    struct pollfd debug_poll_fd;
    debug_poll_fd.fd = state->spp_driver_handle;
    debug_poll_fd.events = POLLIN;
    for (;;)
    {
        int ret = poll(&debug_poll_fd, 1, TIMEOUT_I3C_DEBUG_RX);
        if (ret < 0)
        {
            ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                    "Error while polling\n");
            return ret;
        }
        if (!ret) {
            ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
               "Timeout on read");
            break;
        }
        if ((debug_poll_fd.revents & POLLIN) == POLLIN)
        {

            memset(cmd->rx_buffer, 0, cmd->read_len);
            read_ret = read(state->spp_driver_handle, cmd->rx_buffer, cmd->read_len);
            ASD_log(ASD_LogLevel_Debug, ASD_LogStream_SPP, ASD_LogOption_None,
                    "Read status: %i, errno=%i\n", read_ret, errno);
            if (read_ret < 0)
            {
                ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                        "Failed to read data %d\n", read_ret);
                return read_ret;
            }
            cmd->read_len = read_ret;
            break;
        }
    }
    debug_i3c_rx(cmd);
    return read_ret;
}


STATUS send_i3c_cmd(SPP_Handler* state, i3c_cmd *cmd)
{
    ssize_t write_ret;
    if (state->spp_driver_handle == UNINITIALIZED_SPP_DEBUG_DRIVER_HANDLE)
    {
        ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                "Failed to use file descriptor:  %d\n", state->spp_driver_handle);
        return ST_ERR;
    }
    debug_i3c_tx(cmd);

    write_ret = write(state->spp_driver_handle, cmd->tx_buffer, cmd->write_len);
    if (write_ret < 0)
    {
        ASD_log(ASD_LogLevel_Error, ASD_LogStream_SPP, ASD_LogOption_None,
                "Failed to write data");
        return ST_ERR;
    }
    return ST_OK;
}
