#pragma once

#include <atomic>
#include <list>
#include <map>
#include "plug/solverplugin.h"


struct BCond
{
    enum GeomType
    {
        Node, Edge, Face
    };
    struct bc_t
    {
        double Value;
        char Tag[4] = {0};    // [0] = 'u','f','p'; any of [1-3] > 0 if 'x','y''z' resp. are specified in input
    };

    GeomType GeType;
    std::list<bc_t> BcLst;

    bool Parse(const GeomType type, const char* str, std::string& emsg);

};

struct SolvParams
{
    double Young;
    double Poisn;
    std::map<int, BCond> BcLst;

    bool FromString(const char* young, const char* poisn, const char* bc_lst, std::string& emsg);
};

class SolvPlugin
        : public SolverPlugin
{
public:
    SolvPlugin();
   ~SolvPlugin() override;

    int Init(const ServiceInfo service_lst[],
             const unsigned int service_num) override;

    void Solve() override;
    void CancelSolving() override;

    void SetSolvInfo(const SolvInfo& in) override;

    SolvStatus GetSolvStatus() const override;
    const char* GetSolvStats() const override;
    const char* GetSolvLog() const override;

    const char* ListRequiredSettings() const override;

    const char* GetNodalDataSetList() const override;
    int GetNodalDataSet(const int dataset_id, double* nodal_array) const override;

    bool IsCancelPossible() const override;

protected:
    virtual int CallService(const char* service_name,
                            const char* service_args,
                            const unsigned int arg_size = 0,
                            char** result = nullptr,
                            unsigned int* res_size = nullptr) override;

private:
    const ServiceInfo* serv_lst_;
    unsigned int serv_num_;
    const SolvInfo* solv_info_ = nullptr;
    SolvParams* solv_params_ = nullptr;

    std::string stats_buf;
    std::atomic<SolvStatus> status_;
    std::atomic<bool> stop_flag_ = false;

    bool Solve_(std::string& log_buf);
};

DECL_CREATE_FREE_PLUG_ROUTINES
