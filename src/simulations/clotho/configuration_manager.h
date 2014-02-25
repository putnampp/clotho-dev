#ifndef CONFIGURATION_MANAGER_H_
#define CONFIGURATION_MANAGER_H_

template < class APP >
class ConfigurationManager {
public:
    ConfigurationManager( APP * app ) : m_app( app ) {}

    void initialize();

    virtual ~ConfigurationManager() {}

protected:
    APP * m_app;
};

#endif  // CONFIGURATION_MANAGER_H_
