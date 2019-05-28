#pragma once


typedef int(*ServiceCb)(const char* arg_data,
                        const unsigned int data_size,
                        char** out_data,
                        unsigned int* out_data_size);

struct ServiceInfo
{
    const char* Name;
    ServiceCb Callback;
};
