#ifndef APPLICATION_H_
#define APPLICATION_H_

struct application {
    virtual void initialize();
    virtual void finalize();
};

#endif  // APPLICATION_H_
