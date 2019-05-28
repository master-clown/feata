inline SolvInfo::SolvInfo()  { Nullify(); }
inline SolvInfo::~SolvInfo() { Dealloc(); }

inline void SolvInfo::Dealloc()
{
    if(DispLst)
        delete[] DispLst;
}

inline void SolvInfo::Nullify()
{
    char* p = reinterpret_cast<char*>(this);
    for(size_t s = 0; s < sizeof(*this); ++s)
        p[s] = 0;
}
