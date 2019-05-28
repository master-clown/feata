#pragma once


namespace util
{
    class DirtyInterface
    {
    public:
        void MakeDirty() { is_dirty_ = true; }

        bool IsDirty() const { return is_dirty_; }

    protected:
        bool is_dirty_ = true;

        void MakeUndirty() { is_dirty_ = false; }
    };
}
