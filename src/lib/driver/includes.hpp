#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winioctl.h>
#include <winternl.h>
#include <ntstatus.h>
#include <tlhelp32.h>

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <memory>

// #define encrypt(str) str
#include "../SKCrypter/skcrypter.hpp" // xor string encryption

#include "ia32.hpp"
#include "wdk.hpp"
#include "assembly.hpp"




