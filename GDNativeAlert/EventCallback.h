#ifndef EVENTCALLBACK_H
#define EVENTCALLBACK_H

#include "CNCDll.h"

class ClassWithCallback
{
public:
    virtual void event_callback(const EventCallbackStruct& event) = 0;
};

typedef void (ClassWithCallback::* LPFN_EventMemberFunctionCallback)(const EventCallbackStruct& event);

class EventCallbackBase
{
    public:
        EventCallbackBase(CNC_Event_Callback_Type pCCallback)
            : m_pClass(NULL),
            m_pMethod(NULL),
            m_pCCallback(pCCallback)
        {
        }

        void Free()
        {
            m_pClass = NULL;
        }

        CNC_Event_Callback_Type Reserve(ClassWithCallback* instance, LPFN_EventMemberFunctionCallback method)
        {
            if (m_pClass)
                return NULL;

            m_pClass = instance;
            m_pMethod = method;
            return m_pCCallback;
        }

    protected:
        static void StaticInvoke(int context, const EventCallbackStruct& event);

    private:
        CNC_Event_Callback_Type m_pCCallback;
        ClassWithCallback* m_pClass;
        LPFN_EventMemberFunctionCallback m_pMethod;
};

template <int context> class DynamicEventCallback : public EventCallbackBase
{
    public:
        DynamicEventCallback()
            : EventCallbackBase(&DynamicEventCallback<context>::GeneratedStaticFunction)
        {
        }

    private:
        static void GeneratedStaticFunction(const EventCallbackStruct& event)
        {
            return StaticInvoke(context, event);
        }
};

class EventMemberFunctionCallback
{
    public:
        EventMemberFunctionCallback(ClassWithCallback* instance, LPFN_EventMemberFunctionCallback method);
        ~EventMemberFunctionCallback();

    public:
        operator CNC_Event_Callback_Type() const
        {
            return m_cbCallback;
        }

        bool IsValid() const
        {
            return m_cbCallback != NULL;
        }

    private:
        CNC_Event_Callback_Type m_cbCallback;
        int m_nAllocIndex;

    private:
        EventMemberFunctionCallback(const EventMemberFunctionCallback& os);
        EventMemberFunctionCallback& operator=(const EventMemberFunctionCallback& os);
};

static EventCallbackBase* AvailableCallbackSlots[] = {
    new DynamicEventCallback<0x00>(),
    new DynamicEventCallback<0x01>(),
    new DynamicEventCallback<0x02>(),
    new DynamicEventCallback<0x03>(),
    new DynamicEventCallback<0x04>(),
    new DynamicEventCallback<0x05>(),
    new DynamicEventCallback<0x06>(),
    new DynamicEventCallback<0x07>(),
    new DynamicEventCallback<0x08>(),
    new DynamicEventCallback<0x09>(),
    new DynamicEventCallback<0x0A>(),
    new DynamicEventCallback<0x0B>(),
    new DynamicEventCallback<0x0C>(),
    new DynamicEventCallback<0x0D>(),
    new DynamicEventCallback<0x0E>(),
    new DynamicEventCallback<0x0F>(),
};

#endif
