#ifndef COMMON_THREAD_H_
#define COMMON_THREAD_H_

#include <pthread.h>

#include <iostream>

using std::cout;
using std::endl;

static pthread_key_t common_thread_key;
static pthread_once_t common_key_once = PTHREAD_ONCE_INIT;
static pthread_once_t common_destroy_once = PTHREAD_ONCE_INIT;

static void make_thread_key() {
    cout << "Creating common thread key for thread local storage" << endl;
    pthread_key_create( &common_thread_key, NULL );
}

static void destroy_thread_key() {
    pthread_key_delete( common_thread_key );
}
#endif  // COMMON_THREAD_H_
