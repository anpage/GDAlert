#include "EventCallback.h"

void EventCallbackBase::StaticInvoke(int context, const EventCallbackStruct& event)
{
    ((AvailableCallbackSlots[context]->m_pClass)->*(AvailableCallbackSlots[context]->m_pMethod))(event);
}

EventMemberFunctionCallback::EventMemberFunctionCallback(ClassWithCallback* instance, LPFN_EventMemberFunctionCallback method)
{
    int imax = sizeof(AvailableCallbackSlots) / sizeof(AvailableCallbackSlots[0]);
    for (m_nAllocIndex = 0; m_nAllocIndex < imax; ++m_nAllocIndex)
    {
        m_cbCallback = AvailableCallbackSlots[m_nAllocIndex]->Reserve(instance, method);
        if (m_cbCallback != NULL)
            break;
    }
}

EventMemberFunctionCallback::~EventMemberFunctionCallback()
{
    if (IsValid())
    {
        AvailableCallbackSlots[m_nAllocIndex]->Free();
    }
}
