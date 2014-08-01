#ifndef MEMORY_PAGE_H_
#define MEMORY_PAGE_H_

#define PAGE_SIZE 4096

typedef unsigned short offset_t;

//struct memory_page_header {
//    offset_t free_start, free_end;
//
//    memory_page_header( offset_t s_off, offset_t e_off ) :
//        free_start( s_off ),
//        free_end( e_off )
//    {}
//
//    memory_page_header( offset_t size ) :
//        memory_page_header(0, size )
//    {}
//};

struct memory_page_header {
    char * free_start, * free_end;

    memory_page_header( char * s = NULL, char * e = NULL ) : free_start(s), free_end(e) {}
};

struct memory_page {
    static const unsigned short BLOCK_SIZE = PAGE_SIZE - sizeof( memory_page_header ) - sizeof( void * );
    memory_page * next;
    memory_page_header header;

    char block[ BLOCK_SIZE ];

//    memory_page( memory_page * n = NULL) : next(n), header( 0, BLOCK_SIZE ) {}
    memory_page( memory_page * n = NULL ) : next(n), header(block, block + BLOCK_SIZE) {}
    void clear() {
        header.free_start = block;
        header.free_end = block + BLOCK_SIZE;
    }
};

#endif  // MEMORY_PAGE_H_
