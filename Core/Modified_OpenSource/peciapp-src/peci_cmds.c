/*
// Copyright (c) 2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
#include <ctype.h>
#include <inttypes.h>
#include "peci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef ABS
#define ABS(_v_) (((_v_) > 0) ? (_v_) : -(_v_))
#endif

uint8_t domainId = 0;

extern EPECIStatus peci_GetDIB(uint8_t target, uint64_t* dib);

void Usage(char* progname)
{
    printf("Usage:\n");
    printf("%s [-a <addr>] [-s <size>] [-i <domain id>] <command> [parameters]\n", progname);
    printf("Options:\n");
    printf("\t%-6s%s\n", "-h", "Display this help information");
    printf("\t%-6s%s\n", "-v",
           "Display additional information about the command");
    printf("\t%-6s%s\n", "-a",
           "Address of the target. Accepted values are 48-55 (0x30-0x37). "
           "Default is 48 (0x30)");
    printf("\t%-6s%s\n", "-s",
           "Size of data to read or write in bytes. Accepted values are 1, 2, "
           "4, 8, and 16. Default is 4");
    printf("Commands:\n");
    printf("\t%-28s%s\n", "Ping", "Ping the target");
    printf("\t%-28s%s\n", "GetTemp", "Get the temperature");
    printf("\t%-28s%s\n", "GetDIB", "Get the DIB");
    printf("\t%-28s%s\n", "RdPkgConfig",
           "Read Package Config <Index Parameter>");
    printf("\t%-28s%s\n", "WrPkgConfig",
           "Write Package Config <Index Parameter Data>");
    printf("\t%-28s%s\n", "RdIAMSR", "MSR Read <Thread Address>");
    printf("\t%-28s%s\n", "RdPCIConfig", "PCI Read <Bus Dev Func [Reg]>");
    printf("\t%-28s%s\n", "RdPCIConfigLocal",
           "Local PCI Read <Bus Dev Func [Reg]>");
    printf("\t%-28s%s\n", "WrPCIConfigLocal",
           "Local PCI Write <Bus Dev Func Reg Data>");
    printf("\t%-28s%s\n", "RdEndpointConfigPCILocal",
           "Endpoint Local PCI Config Read <Seg Bus Dev Func Reg>");
    printf("\t%-28s%s\n", "WrEndpointConfigPCILocal",
           "Endpoint Local PCI Config Write <Seg Bus Dev Func Reg Data>");
    printf("\t%-28s%s\n", "RdEndpointConfigPCI",
           "Endpoint PCI Config Read <Seg Bus Dev Func Reg>");
    printf("\t%-28s%s\n", "WrEndpointConfigPCI",
           "Endpoint PCI Config Write <Seg Bus Dev Func Reg Data>");
    printf("\t%-28s%s\n", "RdEndpointConfigMMIO",
           "Endpoint MMIO Read <AType Bar Seg Bus Dev Func Reg>");
    printf("\t%-28s%s\n", "Telemetry", "Telemetry <opcode [parameters]>");
    printf("\t%-28s%s\n", "raw", "Raw PECI command in bytes");
    printf("\n");
}

int main(int argc, char* argv[])
{
    int c;
    int i = 0;
    char* cmd = NULL;
    EPECIStatus ret;
    uint8_t address = 0x30; // use default address of 48d
    uint8_t u8Size = 4;     // default to a DWORD
    uint32_t u32PciReadVal = 0;
    uint8_t u8Seg = 0;
    uint8_t u8Bar = 0;
    uint8_t u8AddrType = 0;
    uint8_t u8PciBus = 0;
    uint8_t u8PciDev = 0;
    uint8_t u8PciFunc = 0;
    uint16_t u16PciReg = 0;
    uint64_t u64Offset = 0;
    uint32_t u32PciWriteVal = 0;
    uint8_t u8PkgIndex = 0;
    uint16_t u16PkgParam = 0;
    uint32_t u32PkgValue = 0;
    uint8_t u8MsrThread = 0;
    uint16_t u16MsrAddr = 0;
    uint64_t u64MsrVal = 0;
    short temperature;
    uint64_t dib;
    int index = 0;
    uint8_t cc = 0;
    bool verbose = false;
    uint8_t u8Opcode = 0;
    uint8_t u8SubOpcode = 0;
    uint8_t u8Param0 = 0;
    uint16_t u16Param1 = 0;
    uint8_t u8Param2 = 0;
    uint16_t u16Index = 0;
    uint16_t u16Sample = 0;
    uint8_t u8RW = 0;
    uint16_t u16Watcher = 0;
    uint16_t u16Offset = 0;
    uint64_t u64Data = 0;
    uint8_t pData[16] = {0};

    //
    // Parse arguments.
    //
    while (-1 != (c = getopt(argc, argv, "hva:s:i:")))
    {
        switch (c)
        {
            case 'h':
                Usage(argv[0]);
                return 0;
                break;

            case 'v':
                verbose = true;
                break;

            case 'a':
                if (optarg != NULL)
                    address = (uint8_t)strtoul(optarg, NULL, 0);
                if (address < MIN_CLIENT_ADDR || address > MAX_CLIENT_ADDR)
                {
                    printf("ERROR: Invalid address \"0x%x\"\n", address);
                    goto ErrorExit;
                }

                break;

            case 's':
                if (optarg != NULL)
                    u8Size = (uint8_t)strtoul(optarg, NULL, 0);
                if (u8Size != 1 && u8Size != 2 && u8Size != 4 && u8Size != 8 &&
                    u8Size != 16)
                {
                    printf("ERROR: Invalid size \"%d\"\n", u8Size);
                    goto ErrorExit;
                }
                break;
	   case 'i':
                if (optarg != NULL)
                    domainId = (uint8_t)strtoul(optarg, NULL, 0);
                if (domainId >= 128) // Domain ID is only 7 bits
                {
                    printf("ERROR: Invalid domain ID \"%d\"\n", domainId);
                    goto ErrorExit;
                }
                break;

            default:
                printf("ERROR: Unrecognized option \"-%c\"\n", optopt);
                goto ErrorExit;
                break;
        }
    }

    // Get the command from the first parameter
    cmd = argv[optind++];
    if (cmd == NULL)
    {
        Usage(argv[0]);
        return 0;
    }

    // Allow any case
    while (cmd[i])
    {
        cmd[i] = (char)tolower((int)cmd[i]);
        i++;
    }

    //
    // Execute the command
    //
    if (verbose)
    {
        printf("PECI target[0x%x]: ", address);
    }
    if (strcmp(cmd, "ping") == 0)
    {
        if (verbose)
        {
            printf("Pinging ... ");
        }
        (0 == peci_Ping(address)) ? printf("Succeeded\n") : printf("Failed\n");
    }
    else if (strcmp(cmd, "getdib") == 0)
    {
        if (verbose)
        {
            printf("GetDIB\n");
        }
        ret = peci_GetDIB(address, &dib);
        if (0 != ret)
        {
            printf("ERROR %d: Retrieving DIB failed\n", ret);
            return 1;
        }
        printf("   0x%" PRIx64 "\n", dib);
    }

    else if (strcmp(cmd, "gettemp") == 0)
    {
        if (verbose)
        {
            printf("GetTemp\n");
        }
        ret = peci_GetTemp(address, &temperature);
        if (0 != ret)
        {
            printf("ERROR %d: Retrieving temperature failed\n", ret);
            return 1;
        }
        printf("   %04xh (%c%d.%02dC)\n",
               (int)(unsigned int)(unsigned short)temperature,
               (0 > temperature) ? '-' : '+',
               (int)((unsigned int)ABS(temperature) / 64),
               (int)(((unsigned int)ABS(temperature) % 64) * 100) / 64);
    }

    else if (strcmp(cmd, "rdpkgconfig") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 2:
                u16PkgParam = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8PkgIndex = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;
            default:
                printf("ERROR: Unsupported arguments for Pkg Read\n");
                goto ErrorExit;
                break;
        }
        if (verbose)
        {
            printf("Pkg Read of Index %02x Param %04x\n", u8PkgIndex,
                   u16PkgParam);
        }
        ret = peci_RdPkgConfig(address, u8PkgIndex, u16PkgParam, u8Size,
                               (uint8_t*)&u32PkgValue, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*x\n", cc, u8Size * 2, u32PkgValue);
    }
    else if (strcmp(cmd, "wrpkgconfig") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 3:
                u32PkgValue = strtoul(argv[--index], NULL, 0);
                u16PkgParam = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8PkgIndex = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;
            default:
                printf("ERROR: Unsupported arguments for Pkg Write\n");
                goto ErrorExit;
                break;
        }
        if (verbose)
        {
            printf("Pkg Write of Index %02x Param %04x: 0x%0*x\n", u8PkgIndex,
                   u16PkgParam, u8Size * 2, u32PkgValue);
        }
        ret = peci_WrPkgConfig(address, u8PkgIndex, u16PkgParam, u32PkgValue,
                               u8Size, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x\n", cc);
    }
    else if (strcmp(cmd, "rdiamsr") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 2:
                u16MsrAddr = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8MsrThread = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;
            default:
                printf("ERROR: Unsupported arguments for MSR Read\n");
                goto ErrorExit;
                break;
        }
        if (verbose)
        {
            printf("MSR Read of Thread %02x MSR %04x\n", u8MsrThread,
                   u16MsrAddr);
        }
        ret = peci_RdIAMSR(address, u8MsrThread, u16MsrAddr, &u64MsrVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*llx\n", cc, u8Size * 2, u64MsrVal);
    }
    else if (strcmp(cmd, "rdpciconfig") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 4:
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 3:
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 2:
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 1:
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;
            default:
                printf("ERROR: Unsupported arguments for PCI Read\n");
                goto ErrorExit;
                break;
        }
        if (verbose)
        {
            printf("PCI Read of %02x:%02x:%02x Reg %02x\n", u8PciBus, u8PciDev,
                   u8PciFunc, u16PciReg);
        }
        ret = peci_RdPCIConfig(address, u8PciBus, u8PciDev, u8PciFunc,
                               u16PciReg, (uint8_t*)&u32PciReadVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*x\n", cc, u8Size * 2, u32PciReadVal);
    }
    else if (strcmp(cmd, "rdpciconfiglocal") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 4:
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 3:
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 2:
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 1:
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;
            default:
                printf("ERROR: Unsupported arguments for Local PCI Read\n");
                goto ErrorExit;
                break;
        }
        if (verbose)
        {
            printf("Local PCI Read of %02x:%02x:%02x Reg %02x\n", u8PciBus,
                   u8PciDev, u8PciFunc, u16PciReg);
        }
        ret = peci_RdPCIConfigLocal(address, u8PciBus, u8PciDev, u8PciFunc,
                                    u16PciReg, u8Size, (uint8_t*)&u32PciReadVal,
                                    &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*x\n", cc, u8Size * 2, u32PciReadVal);
    }
    else if (strcmp(cmd, "wrpciconfiglocal") == 0)
    {
        index = argc;
        u32PciWriteVal = strtoul(argv[--index], NULL, 0);
        switch (argc - optind)
        {
            case 5:
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 4:
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 3:
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                /* FALLTHROUGH */
            case 2:
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;
            default:
                printf("ERROR: Unsupported arguments for Local PCI Write\n");
                goto ErrorExit;
                break;
        }
        if (verbose)
        {
            printf("Local PCI Write of %02x:%02x:%02x Reg %02x: 0x%0*x\n",
                   u8PciBus, u8PciDev, u8PciFunc, u16PciReg, u8Size * 2,
                   u32PciWriteVal);
        }
        ret = peci_WrPCIConfigLocal(address, u8PciBus, u8PciDev, u8PciFunc,
                                    u16PciReg, u8Size, u32PciWriteVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x\n", cc);
    }
    else if (strcmp(cmd, "rdendpointconfigpcilocal") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 5:
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8Seg = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;

            default:
                printf("ERROR: Unsupported arguments for Endpoint Local PCI "
                       "Read\n");
                goto ErrorExit;
        }
        if (verbose)
        {
            printf(
                "Endpoint Local PCI Read of Seg:%02x %02x:%02x:%02x Reg %02x\n",
                u8Seg, u8PciBus, u8PciDev, u8PciFunc, u16PciReg);
        }
        ret = peci_RdEndPointConfigPciLocal(address, u8Seg, u8PciBus, u8PciDev,
                                            u8PciFunc, u16PciReg, u8Size,
                                            (uint8_t*)&u32PciReadVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*x\n", cc, u8Size * 2, u32PciReadVal);
    }
    else if (strcmp(cmd, "wrendpointconfigpcilocal") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 6:
                u32PciWriteVal = strtoul(argv[--index], NULL, 0);
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8Seg = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;

            default:
                printf("ERROR: Unsupported arguments for Endpoint Local PCI "
                       "Write\n");
                goto ErrorExit;
        }
        if (verbose)
        {
            printf("Endpoint Local PCI Write of Seg:%02x %02x:%02x:%02x Reg "
                   "%02x: 0x%0*x\n",
                   u8Seg, u8PciBus, u8PciDev, u8PciFunc, u16PciReg, u8Size * 2,
                   u32PciWriteVal);
        }
        ret = peci_WrEndPointPCIConfigLocal(address, u8Seg, u8PciBus, u8PciDev,
                                            u8PciFunc, u16PciReg, u8Size,
                                            u32PciWriteVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x\n", cc);
    }
    else if (strcmp(cmd, "rdendpointconfigpci") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 5:
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8Seg = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;

            default:
                printf("ERROR: Unsupported arguments for Endpoint PCI Read\n");
                goto ErrorExit;
        }
        if (verbose)
        {
            printf("Endpoint PCI Read of Seg:%02x %02x:%02x:%02x Reg %02x\n",
                   u8Seg, u8PciBus, u8PciDev, u8PciFunc, u16PciReg);
        }
        ret = peci_RdEndPointConfigPci(address, u8Seg, u8PciBus, u8PciDev,
                                       u8PciFunc, u16PciReg, u8Size,
                                       (uint8_t*)&u32PciReadVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*x\n", cc, u8Size * 2, u32PciReadVal);
    }
    else if (strcmp(cmd, "wrendpointconfigpci") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 6:
                u32PciWriteVal = strtoul(argv[--index], NULL, 0);
                u16PciReg = (uint16_t)strtoul(argv[--index], NULL, 0);
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8Seg = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;

            default:
                printf("ERROR: Unsupported arguments for Endpoint PCI Write\n");
                goto ErrorExit;
        }
        if (verbose)
        {
            printf("Endpoint PCI Write of Seg:%02x %02x:%02x:%02x Reg %02x: "
                   "0x%0*x\n",
                   u8Seg, u8PciBus, u8PciDev, u8PciFunc, u16PciReg, u8Size * 2,
                   u32PciWriteVal);
        }
        ret = peci_WrEndPointPCIConfig(address, u8Seg, u8PciBus, u8PciDev,
                                       u8PciFunc, u16PciReg, u8Size,
                                       u32PciWriteVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x\n", cc);
    }
    else if (strcmp(cmd, "rdendpointconfigmmio") == 0)
    {
        index = argc;
        switch (argc - optind)
        {
            case 7:
                u64Offset = strtoull(argv[--index], NULL, 0);
                u8PciFunc = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciDev = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8PciBus = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8Seg = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8Bar = (uint8_t)strtoul(argv[--index], NULL, 0);
                u8AddrType = (uint8_t)strtoul(argv[--index], NULL, 0);
                break;

            default:
                printf("ERROR: Unsupported arguments for Endpoint MMIO Read\n");
                goto ErrorExit;
        }
        if (verbose)
        {
            printf("Endpoint MMIO Read of Seg:%02x %02x:%02x:%02x AType:%02x "
                   "Bar:%02x Offset:0x%" PRIx64 "\n",
                   u8Seg, u8PciBus, u8PciDev, u8PciFunc, u8AddrType, u8Bar,
                   u64Offset);
        }
        ret = peci_RdEndPointConfigMmio(address, u8Seg, u8PciBus, u8PciDev,
                                        u8PciFunc, u8Bar, u8AddrType, u64Offset,
                                        u8Size, (uint8_t*)&u32PciReadVal, &cc);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            return 1;
        }
        printf("   cc:0x%02x 0x%0*x\n", cc, u8Size * 2, u32PciReadVal);
    }
    else if (strcmp(cmd, "telemetry") == 0)
    {
        // Get the Opcode
        if (argc == optind)
        {
            // Exit if no Opcode provided
            goto ErrorExit;
        }
        u8Opcode = (uint8_t)strtoul(argv[optind++], NULL, 0);
        index = argc;
        switch (u8Opcode)
        {
            case 1:
            {
                switch (argc - optind)
                {
                    case 4:
                        u8Param2 = (uint8_t)strtoul(argv[--index], NULL, 0);
                        u16Param1 = (uint16_t)strtoul(argv[--index], NULL, 0);
                        u8Param0 = (uint8_t)strtoul(argv[--index], NULL, 0);
                        u8SubOpcode = (uint8_t)strtoul(argv[--index], NULL, 0);
                        break;

                    default:
                        printf("ERROR: Unsupported arguments for Telemetry "
                               "Discovery\n");
                        printf("Telemetry 0x01 <subopcode param0 param1 "
                               "param2>\n");
                        return 1;
                }
                if (verbose)
                {
                    printf("Telemetry Discovery Opcode:0x%02x SubOpcode:0x%02x "
                           "Param0:0x%02x Param1:0x%04x Param2:0x%02x\n",
                           u8Opcode, u8SubOpcode, u8Param0, u16Param1,
                           u8Param2);
                }
                // Set the return size based on the command
                switch (u8SubOpcode)
                {
                    case 0:
                        u8Size = 1;
                        break;
                    case 1:
                        u8Size = 2;
                        break;
                    case 2:
                        switch (u8Param0)
                        {
                            case 1:
                                u8Size = 8;
                                break;
                            case 2:
                                u8Size = 10;
                                break;
                            case 3:
                                u8Size = 4;
                                break;
                            case 4:
                                u8Size = 8;
                                break;
                            default:
                            	printf("ERROR: Unsupported param\n");
                            	goto ErrorExit;
                        }
                        break;
                    case 3:
                        u8Size = 8;
                        break;
                    case 4:
                        u8Size = 1;
                        break;
                    case 5:
                    	u8Size = 8;
                    	break;
                    default:
                    	printf("ERROR: Unsupported SubOpcode\n");
                    	goto ErrorExit;
                }
                ret = peci_Telemetry_Discovery_dom(
                		address, domainId, u8SubOpcode, u8Param0, u16Param1,
                		u8Param2, u8Size, pData, &cc);
                if (0 != ret)
                {
                	printf("ERROR %d: command failed\n", ret);
                	return 1;
                }
                printf("   cc:0x%02x", cc);
                for (size_t i = 0; i < u8Size; i++)
                {
                	printf(" 0x%02x", pData[i]);
                }
                printf("\n");

                break;
            }
            case 2:
            {
                switch (argc - optind)
                {
                    case 2:
                        u16Sample = (uint16_t)strtoul(argv[--index], NULL, 0);
                        u16Index = (uint16_t)strtoul(argv[--index], NULL, 0);
                        break;

                    default:
                        printf("ERROR: Unsupported arguments for Telemetry Get "
                               "Telem Sample\n");
                        printf("Telemetry 0x02 <index sample>\n");
                        return 1;
                }
                if (verbose)
                {
                    printf("Telemetry Get Telem Sample Opcode:0x%02x "
                           "Index:0x%04x Sample:0x%04x\n",
                           u8Opcode, u16Index, u16Sample);
                }
                // Set the return size based on the command
                u8Size = 8;
                ret = peci_Telemetry_GetTelemSample_dom(address, domainId,u16Index, u16Sample,u8Size, pData, &cc);
                if (0 != ret)
                {
                	printf("ERROR %d: command failed\n", ret);
                	return 1;
                }
                printf("   cc:0x%02x", cc);
                for (size_t i = 0; i < u8Size; i++)
                {
                	printf(" 0x%02x", pData[i]);
                }
                printf("\n");
                break;
            }
            case 3:
            {
                // Get the read/write parameter
                if (argc == optind)
                {
                    // Exit if no read/write parameter provided
                    printf("ERROR: Missing read/write argument for Telemetry "
                           "Config Watcher\n");
                    printf("Telemetry 0x03 <read/write watcher offset>\n");
                    return 1;
                }
                u8RW = (uint8_t)strtoul(argv[optind++], NULL, 0);
                switch (u8RW)
                {
                    case 0:
                        switch (argc - optind)
                        {
                            case 2:
                                u16Offset = (uint16_t)strtoul(argv[--index],
                                                              NULL, 0);
                                u16Watcher = (uint16_t)strtoul(argv[--index],
                                                               NULL, 0);
                                break;

                            default:
                                printf("ERROR: Unsupported arguments for "
                                       "Telemetry Config Watcher read\n");
                                printf(
                                    "Telemetry 0x03 <read watcher offset>\n");
                                return 1;
                        }
                        if (verbose)
                        {
                            printf("Telemetry Config Watcher Opcode:0x%02x "
                                   "Read/Write:0x%02x Watcher:0x%04x "
                                   "Offset:0x%04x\n",
                                   u8Opcode, u8RW, u16Watcher, u16Offset);
                        }
                        // Set the return size based on the command
                        u8Size = 8;
                        ret = peci_Telemetry_ConfigWatcherRd_dom(address, domainId, u16Watcher, u16Offset, u8Size, (uint8_t*)&u64Data, &cc);
                        if (0 != ret)
                        {
                        	printf("ERROR %d: command failed\n", ret);
                        	return 1;
                        }
                        printf("   cc:0x%02x 0x%08" PRIx64 "\n", cc,u64Data);
                        break;
                    case 1:
                        switch (argc - optind)
                        {
                            case 3:
                                u64Data = (uint64_t)strtoull(argv[--index],
                                                             NULL, 0);
                                u16Offset = (uint16_t)strtoul(argv[--index],
                                                              NULL, 0);
                                u16Watcher = (uint16_t)strtoul(argv[--index],
                                                               NULL, 0);
                                break;

                            default:
                                printf("ERROR: Unsupported arguments for "
                                       "Telemetry Config Watcher write\n");
                                printf("Telemetry 0x03 <write watcher offset "
                                       "data>\n");
                                return 1;
                        }
                        if (verbose)
                        {
                            printf("Telemetry Config Watcher Opcode:0x%02x "
                                   "Read/Write:0x%02x Watcher:0x%04x "
                                   "Offset:0x%04x Data:0x%llx\n",
                                   u8Opcode, u8RW, u16Watcher, u16Offset,
                                   (unsigned long long)u64Data);
                        }
                        ret = peci_Telemetry_ConfigWatcherWr_dom(
                        		address, domainId, u16Watcher, u16Offset,
                        		sizeof(u64Data), (uint8_t*)&u64Data, &cc);
                        if (0 != ret)
                        {
                        	printf("ERROR %d: command failed\n", ret);
                        	return 1;
                        }
                        printf("   cc:0x%02x\n", cc);
                        break;
                    default:
                        printf("ERROR: Unsupported read/write argument for "
                               "Telemetry Config Watcher\n");
                        printf("Telemetry 0x03 <read/write watcher offset>\n");
                        return 1;
                }
                break;
            }
            case 12:
            {
                switch (argc - optind)
                {
                    case 2:
                        u16Sample = (uint16_t)strtoul(argv[--index], NULL, 0);
                        u16Index = (uint16_t)strtoul(argv[--index], NULL, 0);
                        break;

                    default:
                        printf("ERROR: Unsupported arguments for Telemetry Get "
                               "Crashlog Sample\n");
                        printf("Telemetry 0x0C <index sample>\n");
                        return 1;
                }
                if (verbose)
                {
                    printf("Telemetry Get Crashlog Sample Opcode:0x%02x "
                           "Index:0x%04x Sample:0x%04x\n",
                           u8Opcode, u16Index, u16Sample);
                }
                // Set the return size based on the command
                u8Size = 8;
                ret = peci_Telemetry_GetCrashlogSample_dom(
                		address, domainId, u16Index, u16Sample, u8Size, pData,
                		&cc);
                if (0 != ret)
                {
                	printf("ERROR %d: command failed\n", ret);
                	return 1;
                }
                printf("   cc:0x%02x", cc);
                for (size_t i = 0; i < u8Size; i++)
                {
                	printf(" 0x%02x", pData[i]);
                }
                printf("\n");

                break;
            }
            default:
                printf("Telemetry Opcode 0x%x not supported. Please use the "
                       "raw command.\n",
                       u8Opcode);
                return 1;
        }
    }
    else if (strcmp(cmd, "raw") == 0)
    {
        if ((argc - optind) < 3)
        {
            printf("ERROR: Unsupported arguments for raw command\n");
            goto ErrorExit;
        }

        // Address is provided in the first byte of the PECI command
        uint8_t rawAddr = (uint8_t)strtoul(argv[optind++], NULL, 0);
        // Write length is provided in the second byte of the PECI command
        uint8_t writeLength = (uint8_t)strtoul(argv[optind++], NULL, 0);
        // Read length is provided in the third byte of the PECI command
        uint8_t readLength = (uint8_t)strtoul(argv[optind++], NULL, 0);

        // remaining parameters should match write length
        if ((argc - optind) != writeLength)
        {
            printf("ERROR: Incorrect write length for raw command\n");
            goto ErrorExit;
        }
        /*Reason for False Positive - parameters without wrong*/
        /* coverity[remediation : FALSE] */
        uint8_t* rawCmd = (uint8_t*)calloc(writeLength, sizeof(uint8_t));
        if (rawCmd == NULL)
        {
            // calloc failed, abort the sequence
            printf("Raw command memory allocation failed\n");
            return 1;
        }
        for (i = 0; i < writeLength; i++)
        {
            rawCmd[i] = (uint8_t)strtoul(argv[i + optind], NULL, 0);
        }
        if (verbose)
        {
            printf("Raw command: %02x %02x %02x ", rawAddr, writeLength,
                   readLength);
            for (i = 0; i < writeLength; i++)
            {
                printf("0x%02x ", rawCmd[i]);
            }
            printf("\n");
        }

        /*Reason for False Positive - parameters without wrong*/
        /* coverity[remediation : FALSE] */
        uint8_t* rawResp = (uint8_t*)calloc(readLength, sizeof(uint8_t));
        if (rawResp == NULL)
        {
            // calloc failed, abort the sequence
            printf("Raw command memory allocation failed\n");
            free(rawCmd);
            return 1;
        }
        ret = peci_raw(rawAddr, readLength, rawCmd, writeLength, rawResp,
                       readLength);
        if (0 != ret)
        {
            printf("ERROR %d: command failed\n", ret);
            free(rawCmd);
            free(rawResp);
            return 1;
        }
        printf("   ");
        for (i = 0; i < readLength; i++)
        {
            printf("0x%02x ", rawResp[i]);
        }
        printf("\n");

        free(rawCmd);
        free(rawResp);
    }
    else
    {
        printf("ERROR: Unrecognized command\n");
        goto ErrorExit;
    }

    return 0;

ErrorExit:
    Usage(argv[0]);
    return 1;
}
