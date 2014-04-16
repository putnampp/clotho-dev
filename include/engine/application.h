#ifndef APPLICATION_H_
#define APPLICATION_H_

struct application {
    
    virtual void initialize() = 0;
    virtual void finalize() = 0;

    virtual ~application() {}
};

#endif  // APPLICATION_H_
