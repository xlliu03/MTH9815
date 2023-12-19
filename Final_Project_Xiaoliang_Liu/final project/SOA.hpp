

#ifndef SOA_HPP
#define SOA_HPP

#include <vector>
#include <memory>

using namespace std;

/**
 * Definition of a generic base class ServiceListener to listen to add, update, and remve
 * events on a Service. This listener should be registered on a Service for the Service
 * to notify all listeners for these events.
 */
template<typename V>
class ServiceListener
{
public:
    virtual ~ServiceListener() {}

    // Listener callback to process an add event to the Service
    virtual void ProcessAdd(V& data) = 0;

    // Listener callback to process a remove event to the Service
    virtual void ProcessRemove(V& data) = 0;

    // Listener callback to process an update event to the Service
    virtual void ProcessUpdate(V& data) = 0;
};


/**
 * Definition of a generic base class Service.
 * Uses key generic type K and value generic type V.
 */
template<typename K, typename V>
class Service
{
protected:
    vector<ServiceListener<V>* > listeners;

public:
    virtual ~Service() {}

    // Get data on our service given a key
    virtual V& GetData(K key) = 0;

    // The callback that a Connector should invoke for any new or updated data
    virtual void OnMessage(V& data) = 0;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    virtual void AddListener(ServiceListener<V>* listener) 
    {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    virtual const vector< ServiceListener<V>* >& GetListeners() const 
    {
        return listeners;
    }

    // Notify all listeners of the update.
    virtual void Notify(V& data) 
    {
        for (auto& e : listeners) 
            e->ProcessAdd(data);
    }
};


/**
 * Definition of a Connector class.
 * This will invoke the Service.OnMessage() method for subscriber Connectors
 * to push data to the Service.
 * Services can invoke the Publish() method on this Service to publish data to the Connector
 * for a publisher Connector.
 * Note that a Connector can be publisher-only, subscriber-only, or both publisher and susbcriber.
 */
template<typename V>
class Connector
{
public:
    virtual ~Connector() {}

    // Publish data to the Connector
    virtual void Publish(V& data) = 0;

    // Subscribe data from the Connector
    virtual void Subscribe(string file_name) = 0;
};

#endif
