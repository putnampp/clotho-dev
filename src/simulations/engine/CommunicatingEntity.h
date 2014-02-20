#ifndef COMMUNICATINGENTITY_H_
#define COMMUNICATINGENTITY_H_

struct CommunicatingEntity {
    typedef string identifier_t;

    virtual identifier_t    getIdentifier() const = 0;
    virtual void            receiveKernelMessage   
    virtual ~CommunicatingEntity() {}
};

#endif  // COMMUNICATINGENTITY_H_
