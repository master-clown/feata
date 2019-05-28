#pragma once

#include <functional>
#include "stl/map.hpp"
#include "util/event_id.hpp"


template<class func_t>
class Event
{
public:
    Event() = default;

    template<class...Args>
    inline void Raise(Args&&... params)
    {
        for(const auto& f: call_lst_)
            f(std::forward<Args>(params)...);
    }

    // returns id of the new subscriber ('id > 0' !!!)
    [[nodiscard]]
    inline event_id AddFollower(const std::function<func_t>& func)
    {
        call_lst_.insert(avail_id_, func);
        return avail_id_++;
    }

    inline void RemoveFollower(const event_id id)
    {
        if(call_lst_.contains(id))
            call_lst_.remove(id);
    }

private:
    Map<event_id, std::function<func_t>> call_lst_;
    event_id avail_id_ = 1;
};
