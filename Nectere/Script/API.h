#pragma once

#ifdef _WIN32
#define NECTERE_SCRIPT_EXPORT __declspec(dllexport)
#else
#define NECTERE_SCRIPT_EXPORT
#endif