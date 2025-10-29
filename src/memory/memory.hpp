#pragma once

#include "../lib/driver/driver.hpp"

namespace driver {
    class c_Memory
	{
    private:
        struct s_process {
            std::uint32_t process_id;
            std::uintptr_t base_address;
            eprocess_t* ptr_eprocess;
        };

        const char* hexdigits =
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\001\002\003\004\005\006\007\010\011\000\000\000\000\000\000"
            "\000\012\013\014\015\016\017\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\012\013\014\015\016\017\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
            "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000";


        auto GetByte(const char* hex) -> uint8_t
        {
            return static_cast<uint8_t>( (hexdigits[hex[0]] << 4) | (hexdigits[hex[1]]) );
        }

	public:
        std::unique_ptr<driver::c_driver> kernel = nullptr;
        s_process currentProcess;

        // Initialize
        c_Memory(std::wstring target_name)
        {
            std::cout << encrypt("Initializing Driver Handle") << std::endl;

            kernel = std::make_unique<driver::c_driver>();

            if (!kernel->setup())
            {
                std::cout << encrypt("Failed to setup driver communication") << std::endl;
                std::cin.get();
                return;
            }

            std::cout << "Name:\t";
            std::wcout << target_name << std::endl;

            if (!kernel->attach(target_name))
            {
                std::cout << encrypt("Failed to attach to process") << std::endl;
                std::cin.get();
                return;
            }

            currentProcess.process_id = GetProcessIDFromName(target_name);
            currentProcess.ptr_eprocess = kernel->get_eprocess(currentProcess.process_id);
            currentProcess.base_address = GetBaseAddress(currentProcess.ptr_eprocess);

            
            std::cout << "Procid:\t" << currentProcess.process_id << std::endl;

        }

        // Deinit
        ~c_Memory()
        {
            kernel->unload();

            currentProcess.process_id = 0;
            currentProcess.base_address = 0x0;
            currentProcess.ptr_eprocess = nullptr;
        }

        auto GetBaseAddress(eprocess_t* process) -> std::uintptr_t
        {
            std::uintptr_t addr = kernel->get_base_address(process);
            
            if (!addr)
            {
                std::cout << encrypt("[-] Couldn't find base address") << std::endl;
                return 0;
            }
            
            std::cout << encrypt("[+] Got Base Address at:\t") << addr << std::endl;
            
            return addr;
        }

        auto GetProcessIDFromName(std::wstring name) -> std::uint32_t
        {
            std::uint32_t procID = kernel->get_process_id(name);

            if (!procID)
            {
                std::cout << encrypt("[-] Couldn't find process id") << std::endl;
                return 0;
            }

            std::cout << encrypt("[+] Found Process ID:\t") << procID << std::endl;

            return procID;
        }

        // For reading pointers
        template<typename ret_t = std::uintptr_t, typename addr_t>
        auto Read(addr_t address) -> ret_t
        {
            return kernel->read(address);
        }

        // For reading data
        template<typename ret_t, typename addr_t>
        ret_t ReadBuffer(addr_t address)
        {
            ret_t buffer{};

            if (!kernel->read_buffer(static_cast<std::uintptr_t>(address), &buffer, sizeof(ret_t)))
                return ret_t{};

            return buffer;
        }

        template<typename addr_t, typename data_t>
        auto Write(addr_t address, data_t data) -> bool
        {
            if (address > 0x7FFFFFFFFFFF || address < 0x2000000)
                return false;

            return kernel->write(address, data);
        }
        

        auto FindSignature(const char* signature, std::uint64_t range_start, std::uint64_t range_end) -> uint64_t
        {
            if (!signature || signature[0] == '\0' || range_start >= range_end)
                return 0;

            std::vector<uint8_t> buffer(range_end - range_start);
            
            if (!kernel->read_buffer((uintptr_t)range_start, buffer.data(), buffer.size()))
                return 0;

            const char* pat = signature;
            uint64_t first_match = 0;
            for (uint64_t i = range_start; i < range_end; i++)
            {
                if (*pat == '?' || buffer[i - range_start] == GetByte(pat))
                {
                    if (!first_match)
                        first_match = i;

                    if (!pat[2])
                        break;

                    pat += (*pat == '?') ? 2 : 3;
                }
                else
                {
                    pat = signature;
                    first_match = 0;
                }
            }

            return first_match;
        }
    };
}



