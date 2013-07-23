/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/
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
