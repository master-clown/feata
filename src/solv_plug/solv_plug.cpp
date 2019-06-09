#include "solv_plug.hpp"
#include <cstdarg>
#include <fstream>
#include <string>
#include <sstream>
#include "mesh/meshinfo.hpp"

#define PLUGIN_NAME "Test Solver Plugin"
#define PLUGIN_SETS_FILE "solver_plug_sets.json"
#define LOG_FILE "solver_plug_log.log"

#define TO_WIDE_HELPER(str) L ## str
#define TO_WIDE(str) TO_WIDE_HELPER(str)


static std::string gSetsStr;

#define SETTING_COUNT 3

static const std::string gYoungParamName = "young";
static const std::string gPoisnParamName = "poisn";
static const std::string gFaceBcParamName = "face_bc";

static std::wstring GetLibDir();


SolvPlugin::SolvPlugin()
{}

SolvPlugin::~SolvPlugin()
{
    if(solv_params_) delete solv_params_;

    const std::string msg = "Plugin '" PLUGIN_NAME "' has been unloaded.";
    CallService("log", msg.c_str(), msg.size());
}

int SolvPlugin::Init(const ServiceInfo service_lst[],
                     const unsigned int service_num)
{
    serv_lst_ = service_lst;
    serv_num_ = service_num;

    std::string msg;
    const auto file_name = GetLibDir() + TO_WIDE(PLUGIN_SETS_FILE);
    std::ifstream ifs(file_name);
    if(!ifs.is_open())
    {
        msg = PLUGIN_NAME ": cannot open settings file '" PLUGIN_SETS_FILE "'.";
        CallService("loge", msg.c_str(), msg.size());
        return 1;
    }

    gSetsStr = static_cast<const std::stringstream&>(std::stringstream() << ifs.rdbuf()).str();
    if(gSetsStr.empty())
    {
        msg = PLUGIN_NAME ": settings file '" PLUGIN_SETS_FILE "' is empty.";
        CallService("loge", msg.c_str(), msg.size());
        return 2;
    }

    msg = "Plugin '" PLUGIN_NAME "' has been initilized";
    CallService("log", msg.c_str(), msg.size());

//    SolvParams params;
//    params.FromString("2.0e+11", "1.0", "face~"
//                                     "9@ux=12.0, fyz = 13.0$"
//                                     "3@fz = 100.12345, fyx = -123.321$"
//                                     "7@p = 53.98712", msg);

    status_.store(SOLV_STATUS_NOT_STARTED);

    return 0;
}

void SolvPlugin::Solve()
{
    std::string log_msg;
    char *young_str = nullptr, *poisn_str = nullptr, *face_bclst_str = nullptr;
    unsigned int young_len, poisn_len, face_bclst_size;

    if(!solv_info_->Mesh->NodeLst ||
       !solv_info_->Mesh->NodeNum)
    {
        log_msg += " mesh node list is empty;";
    }
    if(!solv_info_->Mesh->ElemLst ||
       !solv_info_->Mesh->ElemNum)
    {
        log_msg += " mesh element list is empty;";
    }
    if(!solv_info_->Mesh->ElemFaceLst ||
       !solv_info_->Mesh->ElemFaceNum ||
       !solv_info_->Mesh->ElemFaceMarkerLst)
    {
        log_msg += " mesh element face list and/or list of their markers are empty;";
    }
    if(!solv_info_->Mesh->ElemFaceLst ||
       !solv_info_->Mesh->ElemFaceNum ||
       !solv_info_->Mesh->ElemFaceMarkerLst)
    {
        log_msg += " mesh element face list and/or list of their markers are empty;";
    }
    if(!solv_info_->Mesh->VertexLocalIdxLst ||
       !solv_info_->Mesh->VertexLocalIdxLstSize)
        log_msg += " vertex local indices list is empty;";
    if(solv_info_->Mesh->VertexLocalIdxLstSize != 4)
        log_msg += " the plugin is for tetrahedral elements, which have 4 geometrical vertices;";
    if(solv_info_->Mesh->NumFacePerElem != 4)
        log_msg += " the plugin is for tetrahedral elements, which have 4 faces;";
    if(!solv_info_->DispLst)
    {
        log_msg += " results vector is not allocated: 3 doubles per node are required;";
    }
    if(!log_msg.empty())
    {
        log_msg = PLUGIN_NAME ": " + log_msg;
        CallService("loge", &log_msg[0], log_msg.size());
        status_.store(SOLV_STATUS_ERROR);
        return;
    }

    stop_flag_.store(false);
    status_.store(SOLV_STATUS_IN_PROGRESS);

    int stcode[SETTING_COUNT];
    stcode[0] = CallService("get_setting_value",
                &gYoungParamName[0], gYoungParamName.size(),
                &young_str, &young_len);
    stcode[1] = CallService("get_setting_value",
                &gPoisnParamName[0], gPoisnParamName.size(),
                &poisn_str, &poisn_len);
    stcode[2] = CallService("get_setting_value",
                &gFaceBcParamName[0], gFaceBcParamName.size(),
                &face_bclst_str, &face_bclst_size);

    for(int i = 0 ; i < SETTING_COUNT; ++i)
        if(stcode[i])
        {
            log_msg = PLUGIN_NAME ": cannot get " + std::to_string(i) +
                      "th setting. Error code " + std::to_string(stcode[i]);
            CallService("loge", &log_msg[0], log_msg.size());
            status_.store(SOLV_STATUS_ERROR);
            goto lbl_free_mem;
        }

    if(!solv_params_) solv_params_ = new SolvParams();
    if(!solv_params_->FromString(young_str, poisn_str, face_bclst_str, log_msg))
    {
        log_msg = PLUGIN_NAME ": input parameters error: " + log_msg;
        CallService("loge", &log_msg[0], log_msg.size());
        status_.store(SOLV_STATUS_ERROR);
        goto lbl_free_mem;
    }

    stats_buf.clear();

    if(!Solve_(log_msg))
    {
        if(!log_msg.empty())
            CallService("loge", &log_msg[0], log_msg.size());
        status_.store(SOLV_STATUS_ERROR);
    }

    status_.store(SOLV_STATUS_FINISHED);

lbl_free_mem:
    if(young_str) CallService("free_output_data", young_str);
    if(poisn_str) CallService("free_output_data", poisn_str);
    if(face_bclst_str) CallService("free_output_data", face_bclst_str);
}

void SolvPlugin::CancelSolving()
{
    return;
}

void SolvPlugin::SetSolvInfo(const SolvInfo &in)
{ solv_info_ = &in; }

//void SolvPlugin::Free(MeshInfo& out)
//{ out.Dealloc(); out.Nullify(); }

SolvStatus SolvPlugin::GetSolvStatus() const
{
    return status_.load();
}

const char* SolvPlugin::GetSolvStats() const
{
    if(status_.load() != SOLV_STATUS_FINISHED)
        return nullptr;

    return stats_buf.c_str();
}

const char* SolvPlugin::GetSolvLog() const
{
    const auto s = status_.load();

    if(s != SOLV_STATUS_ERROR &&
       s != SOLV_STATUS_FINISHED)
        return nullptr;

    return stats_buf.c_str();
}

const char* SolvPlugin::ListRequiredSettings() const
{
    return gSetsStr.c_str();
}

// "id1$name1$desc1~id2$name2$desc2~..."
// id > 2 for strains and stresses
const char* SolvPlugin::GetNodalDataSetList() const
{
    return "1$X-component of displacement$~"
           "2$Y-component of displacement$~"
           "3$Z-component of displacement$~"
           "4$Total displacement$~"
           "5$X-component of strain$~"
           "6$Y-component of strain$~"
           "7$Z-component of strain$~"
           "8$XY-component of strain$~"
           "9$YZ-component of strain$~"
           "10$ZX-component of strain$~"
           "11$X-component of stress$~"
           "12$Y-component of stress$~"
           "13$Z-component of stress$~"
           "14$XY-component of stress$~"
           "15$YZ-component of stress$~"
           "16$ZX-component of stress$";
}

bool SolvPlugin::IsCancelPossible() const { return false; }

int SolvPlugin::CallService(const char* service_name,
                                const char* service_args,
                                const unsigned int arg_size,
                                char** result,
                                unsigned int* res_size)
{
    unsigned int i_serv = 0;
    for(; i_serv < serv_num_; ++i_serv)
        if(!strcmp(serv_lst_[i_serv].Name, service_name))
            break;

    if(i_serv >= serv_num_ || !serv_lst_[i_serv].Callback)
        return -1;

    return serv_lst_[i_serv].Callback(service_args, arg_size, result, res_size);
}

DEF_CREATE_FREE_PLUG_ROUTINES(SolvPlugin)

bool SolvParams::FromString(const char* young, const char* poisn, const char* bc_lst, std::string& emsg)
{
    try { Young = std::stod(young, nullptr);
          Poisn = std::stod(poisn, nullptr); } catch(...)
    {
        emsg = "Young modulus or Poisson ratio are invalid.";
        return false;
    }

    int bc_lst_len = strlen(bc_lst);
    if(!bc_lst_len)
    {
        emsg = "boundary conditions are not supplied.";
        return false;
    }

    BCond::GeomType geom_type;
    const auto geom_type_end = strchr(bc_lst, '~');
    if(!geom_type_end)
    {
        emsg = "geometry type is not supplied.";
        return false;
    }

    if(!strncmp("vertex", bc_lst, 6))
        geom_type = BCond::Node;
    else
    if(!strncmp("edge", bc_lst, 4))
        geom_type = BCond::Edge;
    else
    if(!strncmp("face", bc_lst, 4))
        geom_type = BCond::Face;
    else
    {
        emsg = "unknown geometry type: " + std::string(bc_lst, geom_type_end - bc_lst);
        return false;
    }

    char* bcstr_spl = new char[bc_lst_len + 1];             // just want to recall C-strings
    strcpy_s(bcstr_spl, bc_lst_len + 1, geom_type_end+1);

    BcLst.clear();

    char* ptr_tmp;
    char* p_str = strtok_s(bcstr_spl, "$", &ptr_tmp);
    while(p_str)
    {
        char* ptr_tmp2;
        char* pair_str = strtok_s(p_str, "@", &ptr_tmp2);
        int id = atoi(pair_str);
        pair_str = strtok_s(nullptr, "@", &ptr_tmp2);

        if(!id) emsg = "unknown geometry id.";
        if(!id || !BcLst[id].Parse(geom_type, pair_str, emsg))
        {
            delete[] bcstr_spl;
            return false;
        }

        p_str = strtok_s(nullptr, "$", &ptr_tmp);
    }

    delete[] bcstr_spl;

    return true;
}

bool BCond::Parse(const GeomType type,
                  const char* str,
                  std::string& emsg)
{
    const std::string parse_str(str);
    if(parse_str.empty())
    {
        emsg = "empty BC value.";
        return false;
    }

    GeType = type;
    BcLst.clear();

    size_t beg = 0;
    bool stop = false;
    while(!stop)
    {
        const auto end = parse_str.find(',', beg);
        if(end == std::string::npos)
            stop = true;

        if(end == beg)
            continue;

        const auto bc_val = parse_str.substr(beg, end - beg);
        const auto eq_pos = bc_val.find('=');
        if(eq_pos == std::string::npos)
        {
            emsg = "syntax error: no '=' for some BC value.";
            return false;
        }

              auto lhs = bc_val.substr(0, eq_pos);
        const auto rhs = bc_val.substr(eq_pos + 1);
        bc_t bc;

        try { bc.Value = std::stod(rhs); } catch(...)
        {
            emsg = "syntax error: BC value's right hand side is not a number.";
            return false;
        }

        // trim
        int lhs_beg = -1, lhs_end = lhs.length();
        if(lhs_end)
        {
            while(isspace(lhs[++lhs_beg]));
            while(lhs_beg < lhs_end && isspace(lhs[--lhs_end]));
        }
        if(lhs_end - lhs_beg < 0)
        {
            emsg = "syntax error: BC value's left hand side is empty";
            return false;
        }
        lhs = lhs.substr(lhs_beg, lhs_end - lhs_beg + 1);

        bc.Tag[0] = tolower(lhs.front());
        for(const auto& prev_bc: BcLst)
            if(prev_bc.Tag[0] != bc.Tag[0])
            {
                emsg = "BC of different types are detected.";
                return false;
            }
        if(bc.Tag[0] == 'u' || bc.Tag[0] == 'f')
        {
            for(int i = 1; i <= lhs_end - lhs_beg; ++i)
            {
                const auto axis = tolower(lhs[i]);
                switch(axis)
                {
                case 'x': bc.Tag[1] = true; break;
                case 'y': bc.Tag[2] = true; break;
                case 'z': bc.Tag[3] = true; break;
                default:
                    emsg = "syntax error: BC value's direction label is invalid: "; emsg += axis;
                    return false;
                }
            }

            BcLst.push_back(bc);
        }
        else
        if(bc.Tag[0] == 'p')
        {
            if(!BcLst.empty())
            {
                emsg = "pressure cannot be specified with other BCs at the same time.";
                return false;
            }

            BcLst.push_back(bc);
            break;
        }
        else
        {
            emsg = "syntax error: unknown BC type label: "; emsg += bc.Tag[0];
            return false;
        }

        beg += end + 1;
    }

    return true;
}

#ifdef _WIN32

#include <Windows.h>


static HINSTANCE gDllHandle = nullptr;

std::wstring GetLibDir()
{
    wchar_t buffer[MAX_PATH];

    GetModuleFileName(gDllHandle, buffer, MAX_PATH);

    const auto pos = std::wstring(buffer).find_last_of(L"\\/");

    return pos == std::wstring::npos ? L"" : std::wstring(buffer).substr(0, pos+1);
}

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD, _In_ LPVOID)
{
    gDllHandle = hinstDLL;
    return TRUE;
}

#endif
