#pragma once

namespace Nectere
{
    namespace Concurrency
    {
        enum class TaskResult : int
        {
            Success,
            Fail,
            NeedUpdate
        };
    }
}