//
// Created by protocol on 4/29/21.
//

#ifndef BOOSTTESTING_SERVICE_H
#define BOOSTTESTING_SERVICE_H

#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <list>
#include "defs.hpp"

namespace vae{
    class Service: private boost::noncopyable{
    public:
        enum ServiceStatus{
            unknown,
            ok,
            failed,
            starting,
            stopped,
        };
        typedef int ServiceId;
        //const ServiceId serviceId;
        const std::string serviceName;

        typedef std::shared_ptr<Service> Ptr;
        //Service(boost::asio::io_service &ios, std::string serviceName): ios(ios), serviceName(serviceName){}
        Service(std::string serviceName): serviceName(serviceName){}
        virtual bool startService(){ return false; }
        virtual bool stopService(){ return false; }
        virtual ServiceStatus getServiceStatus(){ return ServiceStatus::unknown; }
        //virtual void cycle(vae::TimeType delta_t){};
        //virtual Service* createInstance(boost::asio::io_service &ios, ServiceId id)=0;
        //virtual std::list<std::string> getRequirements()=0;
    private:
        //boost::asio::io_service &ios;
    };

    class ServiceRegistry{
    public:
        void registryService(Service::Ptr dis){
            registeredServices.push_back(dis);
        }
    private:
        std::list<Service::Ptr> registeredServices;
    };
}


//  //  //  //  // Old version:
/*
namespace vae{

    template<typename T>
    class ServiceEntry {
    public:
        ServiceEntry(const char *name);
    private:
        //static ServiceEntry<ServiceEntry> reg;
    };

    class Service: private boost::noncopyable{
    public:
        typedef int ServiceId;
        const ServiceId serviceId;
        std::shared_ptr<Service> Ptr;
        Service(boost::asio::io_service &ios, ServiceId id): ios(ios), serviceId(id){}
        //virtual std::list<std::string> getRequirements()=0;
        virtual bool start()=0;
        virtual Service* createInstance(boost::asio::io_service &ios, ServiceId id)=0;
    private:
        boost::asio::io_service &ios;

    protected:
    };

    template<typename T> Service * createT() { return new T; }

    class ServiceRegistry: private boost::noncopyable{
        //std::vector<std::string, Service::Ptr> serviceRegistry;
    public:
        typedef std::map<std::string, Service*(*)()> map_type;

        static Service *createInstance(std::string const& s, boost::asio::io_service &ios){
        }
        static void registerServiceType(std::string const& s){
            knownServices.push_back(s);
        }
    protected:
        static std::list<std::string> knownServices;
    private:
    };

    class TestXService: public Service {
    public:
        TestXService(boost::asio::io_service &ios, ServiceId id): Service(ios, id){}
        TestXService* createInstance(boost::asio::io_service &ios, ServiceId id){
            return new TestXService(ios, id);
        }
        bool start(){ return false; }
    private:
        static ServiceEntry<TestXService> reg;
    };
}
*/


#endif //BOOSTTESTING_SERVICE_H
