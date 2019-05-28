#pragma once

#include <QThread>
#include <functional>


namespace util
{
    class ParallelTask : public QThread
    {
    public:
        ParallelTask() = default;
        ParallelTask(const std::function<void(const QAtomicInt&)>& func);

        void SetFunction(const std::function<void(const QAtomicInt&)>& func);
        void SetOnFinishedFunc(const std::function<void()>& on_finish_func);

    public slots:
        void SetStopFlag();

    protected:
        void run() override;

    private:
        std::function<void(const QAtomicInt&)> func_;
        std::function<void()> on_finish_;   // for convenience instead of signal
        QMetaObject::Connection on_finish_conn_;

        QAtomicInt flag_stop_;
    };
}


namespace util
{
    inline ParallelTask::ParallelTask(const std::function<void(const QAtomicInt&)>& func)
        : func_(func)
    {}

    inline void ParallelTask::SetFunction(const std::function<void(const QAtomicInt&)>& func)
    { func_ = func; }

    inline void ParallelTask::SetOnFinishedFunc(const std::function<void()>& on_finish_func)
    {
        on_finish_ = on_finish_func;

        if(on_finish_conn_)
            disconnect(on_finish_conn_);

        on_finish_conn_ = connect(this, &ParallelTask::finished, this,
        [this]
        {
            flag_stop_.store(false);

            if(on_finish_)
                on_finish_();
        }, Qt::QueuedConnection);
    }

    inline void ParallelTask::SetStopFlag() { flag_stop_.store(1); }

    inline void ParallelTask::run()
    {
        if(!func_)
            return;

        flag_stop_.store(false);

        func_(std::cref(flag_stop_));
    }
}
