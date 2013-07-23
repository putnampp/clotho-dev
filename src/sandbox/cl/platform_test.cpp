#define __CL_ENABLE_EXCEPTIONS

#include "CL/opencl.h"

#include "common.h"

#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;


int main( int argc, char ** argv ) {

    const cl_uint MAX_PLATFORMS = 10;
    cl_platform_id p_ids[ MAX_PLATFORMS ];
    cl_uint      nPlatforms;

    cl_int  ret = clGetPlatformIDs(MAX_PLATFORMS, p_ids, &nPlatforms );

    cout << "Found " << nPlatforms << " Platforms" << endl;

    const int BUFFER_SIZE = 1024;
    char buffer[ BUFFER_SIZE ];
    size_t nSize;

    const int MAX_DEVICES = 10;
    cl_device_id d_ids[ MAX_DEVICES ];
    cl_uint nDevices;

    for( cl_uint i = 0; i < nPlatforms; ++i ) {
        ret = clGetPlatformInfo( p_ids[i], CL_PLATFORM_NAME, BUFFER_SIZE, (void *)buffer, &nSize );
        cout << "Platform '" << buffer;

        ret = clGetPlatformInfo( p_ids[i], CL_PLATFORM_VERSION, BUFFER_SIZE, (void *)buffer, &nSize );
        cout << "' supports " << buffer << " and has the following devices:" << endl;

        // profile CPU devices
        ret = clGetDeviceIDs( p_ids[i], CL_DEVICE_TYPE_ALL, MAX_DEVICES, d_ids, &nDevices );
        for( cl_uint j = 0; j < nDevices; ++j ) {
            ret = clGetDeviceInfo( d_ids[j], CL_DEVICE_NAME, BUFFER_SIZE, (void *)buffer, &nSize );

            cout << "\tDevice '" <<  buffer << "' supports ";

            ret = clGetDeviceInfo( d_ids[j], CL_DEVICE_VERSION, BUFFER_SIZE, (void *)buffer, &nSize );

            cout << buffer << " and is ";
            cl_device_type dType;
            ret = clGetDeviceInfo( d_ids[j], CL_DEVICE_TYPE, sizeof( cl_device_type ), (void *)&dType, NULL );

            switch( dType ) {
            case CL_DEVICE_TYPE_CPU:
                cout << "a CPU";
                break;
            case CL_DEVICE_TYPE_GPU:
                cout << "a GPU";
                break;
            case CL_DEVICE_TYPE_ACCELERATOR:
                cout << "an ACCELERATOR type";
                break;
            case CL_DEVICE_TYPE_DEFAULT:
                cout << "a DEFAULT type";
                break;
            default:
                cout << "of UNKNOWN type";
                break;
            }

            cout << endl;
        }
    }

    return EXIT_SUCCESS;
}
