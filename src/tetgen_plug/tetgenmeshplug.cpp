#include "tetgenmeshplug.hpp"
#include <cstdarg>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tetgen.h>

#define PLUGIN_NAME "TetGen 1.5.1 Meshing Plugin"
#define PLUGIN_SETS_FILE "tetgen_plugin_sets.json"
#define LOG_FILE "tetgen_plugin_tmp_log.log"

#define TO_WIDE_HELPER(str) L ## str
#define TO_WIDE(str) TO_WIDE_HELPER(str)


struct MeshParams
{
    double MaxRadEdgeRatio;
    double MaxElemVol;
    std::map<int, double> MaxBoundElemFaceArea;

    bool FromString(std::string& emsg, char* max_radedge,
                    char* max_vol,
                    char* max_area_lst_str);

    bool IsValid(std::string& error_msg);
    std::string ToString() const;
};

static void ConvertToTetgen(const MeshParams& params, const MeshInfo& in, tetgenio& out);
static void ConvertFromTetgen(const tetgenio& in, MeshInfo& out);
static std::string gStatsBuf;
static std::string gSetsStr;

#define SETTING_COUNT 3

static const std::string gMaxElemVolumeParamName    = "max_elem_volume";
static const std::string gMaxRadEdgeRatParamName    = "max_rad_edge_ratio";
static const std::string gMaxBoundFaceAreaParamName = "max_bound_face_elem_area";


static std::wstring GetLibDir();

TetgenMeshPlug::TetgenMeshPlug()
{}

TetgenMeshPlug::~TetgenMeshPlug()
{
    const std::string msg = "Plugin '" PLUGIN_NAME "' has been unloaded.";
    CallService("log", msg.c_str(), msg.size());
}

int TetgenMeshPlug::Init(const ServiceInfo service_lst[],
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

    msg = "Plugin '" PLUGIN_NAME "' has been initilized";
    CallService("log", msg.c_str(), msg.size());

    status_.store(MESHING_STATUS_NOT_STARTED);

    return 0;
}

void TetgenMeshPlug::Mesh()
{
    tetgenio tet_in;
    tetgenio tet_out;
    MeshParams params;
    std::string log_msg;
    char* param_maxvol_str = nullptr,
        * param_maxradedge_str = nullptr,
        * param_maxfacelst_str = nullptr;
    unsigned int param_maxvol_size,
                 param_maxradedge_size,
                 param_maxfacelst_size;

    status_.store(MESHING_STATUS_IN_PROGRESS);

    int stcode[4];
    stcode[0] = CallService("get_setting_value",
                &gMaxElemVolumeParamName[0], gMaxElemVolumeParamName.size(),
                &param_maxvol_str, &param_maxvol_size);
    stcode[1] = CallService("get_setting_value",
                &gMaxRadEdgeRatParamName[0], gMaxRadEdgeRatParamName.size(),
                &param_maxradedge_str, &param_maxradedge_size);
    stcode[2] = CallService("get_setting_value",
                &gMaxBoundFaceAreaParamName[0], gMaxBoundFaceAreaParamName.size(),
                &param_maxfacelst_str, &param_maxfacelst_size);

    for(int i = 0 ; i < SETTING_COUNT; ++i)
        if(stcode[i])
        {
            log_msg = PLUGIN_NAME ": cannot get " + std::to_string(i) +
                      "th setting. Error code " + std::to_string(stcode[i]);
            CallService("loge", &log_msg[0], log_msg.size());
            status_.store(MESHING_STATUS_ERROR);
            goto lbl_free_mem;
        }


    if(!params.FromString(log_msg, param_maxradedge_str, param_maxvol_str, param_maxfacelst_str) ||
       !params.IsValid(log_msg))
    {
        log_msg = PLUGIN_NAME ": mesh parameters error: " + log_msg;
        CallService("loge", &log_msg[0], log_msg.size());
        status_.store(MESHING_STATUS_ERROR);
        goto lbl_free_mem;
    }

    ConvertToTetgen(params, *mesh_in, tet_in);

    gStatsBuf.clear();

    try
    {
        std::string params_str = "pAV" + params.ToString() + "\0";
        tetrahedralize(&params_str[0], &tet_in, &tet_out);
    }
    catch(const int code)
    {
        gStatsBuf += TetGenErrorCodeToString((TetGenErrorCode)code);

        CallService("loge", &gStatsBuf[0], gStatsBuf.size());
        status_.store(MESHING_STATUS_ERROR);
        goto lbl_free_mem;
    }

    ConvertFromTetgen(tet_out, *mesh_out);

    status_.store(MESHING_STATUS_FINISHED);

lbl_free_mem:
    if(param_maxvol_str) CallService("free_output_data", param_maxvol_str);
    if(param_maxradedge_str) CallService("free_output_data", param_maxradedge_str);
    if(param_maxfacelst_str) CallService("free_output_data", param_maxfacelst_str);
}

void TetgenMeshPlug::CancelMeshing()
{
    return;
}

void TetgenMeshPlug::SetInputMeshInfo(const MeshInfo& in)
{ mesh_in = &in; }

void TetgenMeshPlug::SetOutputMeshInfo(MeshInfo& out)
{ mesh_out = &out; }

void TetgenMeshPlug::FreeOutputMeshInfo(MeshInfo& out)
{ out.Dealloc(); out.Nullify(); }

MeshingStatus TetgenMeshPlug::GetMeshingStatus() const
{
    return status_.load();
}

const char* TetgenMeshPlug::GetMeshStats() const
{
    if(status_.load() != MESHING_STATUS_FINISHED)
        return nullptr;

    return gStatsBuf.c_str();
}

const char* TetgenMeshPlug::GetMesherLog() const
{
    const auto s = status_.load();

    if(s != MESHING_STATUS_ERROR &&
       s != MESHING_STATUS_FINISHED)
        return nullptr;

    return gStatsBuf.c_str();
}

const char* TetgenMeshPlug::ListRequiredSettings() const
{
    return gSetsStr.c_str();
}

bool TetgenMeshPlug::IsCancelPossible() const { return false; }

int TetgenMeshPlug::CallService(const char* service_name,
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

DEF_CREATE_FREE_PLUG_ROUTINES(TetgenMeshPlug)

void ConvertToTetgen(const MeshParams& params, const MeshInfo& in, tetgenio& out)
{
    out.deinitialize();

    out.firstnumber = 1;
    out.mesh_dim = 3;
    out.numberofcorners = in.NumNodePerElem;

    out.pointlist       = new double[3*(out.numberofpoints = in.NodeNum)];
    for(int i = 0; i < 3*out.numberofpoints; ++i)
        out.pointlist[i] = in.NodeLst[i];
//    out.pointmarkerlist = new int[out.numberofpoints]();
//    for(unsigned int i = 0; i < in.NodeMarkerLstSize; ++i)
//        out.pointmarkerlist[in.NodeMarkerLst[i].TargetObjIdx] = in.NodeMarkerLst[i].Marker;

//    out.holelist = in.HoleNum ? new double[3*(out.numberofholes = in.HoleNum)] : nullptr;
//    for(int i = 0; i < 3*out.numberofholes; ++i)
//        out.holelist[i] = in.HoleLst[i];

    out.facetconstraintlist = !params.MaxBoundElemFaceArea.empty() ? new double
                              [2*(out.numberoffacetconstraints
                                  = params.MaxBoundElemFaceArea.size())] : nullptr;
    int i_fconstr = 0;
    for(const auto& p: params.MaxBoundElemFaceArea)
    {
        out.facetconstraintlist[i_fconstr + 0] = p.first + 1;
        out.facetconstraintlist[i_fconstr + 1] = p.second;
        i_fconstr += 2;
    }

//    out.edgemarkerlist = new int[out.numberofpoints]();
//    for(unsigned int i = 0; i < in.NodeMarkerLstSize; ++i)
//        out.pointmarkerlist[in.NodeMarkerLst[i].TargetObjIdx] = in.NodeMarkerLst[i].Marker;

    out.facetlist = new tetgenio::facet[out.numberoffacets = in.FacetNum];
    out.facetmarkerlist = new int[out.numberoffacets];
    for(int i = 0; i < out.numberoffacets; ++i)
    {
        const auto num_poly = out.facetlist[i].numberofpolygons
                            = in.FacetLst[i].PolyNum;
        const auto num_hole = out.facetlist[i].numberofholes
                            = in.FacetLst[i].HoleNum;
        out.facetlist[i].polygonlist = new tetgenio::polygon[num_poly];
        out.facetlist[i].holelist = num_hole ? new double[3*num_hole] : nullptr;

        for(int i_poly = 0; i_poly < num_poly; ++i_poly)
        {
            const auto ptr = &out.facetlist[i].polygonlist[i_poly];
            ptr->numberofvertices = in.FacetLst[i].PolyLst[i_poly].VertexNum;
            ptr->vertexlist       = new int[ptr->numberofvertices];

            for(int i_v = 0; i_v < ptr->numberofvertices; ++i_v)
                ptr->vertexlist[i_v] = in.FacetLst[i].PolyLst[i_poly].VertexLst[i_v];
        }
        for(int i_hc = 0; i_hc < 3*num_hole; ++i_hc)
        {
            out.facetlist[i].holelist[i_hc] = in.FacetLst[i].HoleLst[i_hc];
        }

        out.facetmarkerlist[i] = in.FacetMarkerLst[i] + 1;       // offset
    }

    out.regionlist = new double[5*(out.numberofregions = in.RegionNum)]; // { 0.5, 0.5, 0.5, 1.0, 0.1 };
    for(int i = 0; i < 5*out.numberofregions; ++i)
        out.regionlist[i] = in.RegionLst[i];
}

void ConvertFromTetgen(const tetgenio& in, MeshInfo& out)
{
    using siz_t = MeshInfo::siz_t;

    out.Dealloc();
    out.Nullify();

    out.NumNodePerElem = in.numberofcorners;
    out.VertexLocalIdxLst = new MeshInfo::id_t
                            [out.VertexLocalIdxLstSize = 4]
                            { 0, 1, 2, 3 };
    out.ElemFaceVertexCountLst = new MeshInfo::siz_t[out.NumFacePerElem = 4]
                            { 3, 3, 3, 3 };
    out.ElemFaceVertexLocalIdxLst = new MeshInfo::id_t[12]
                            { 0, 2, 1, 1, 2, 3, 0, 3, 2, 0, 1, 3 };

    out.NodeLst = new MeshInfo::num_t[3*(out.NodeNum = in.numberofpoints)];
    for(siz_t i = 0; i < 3*out.NodeNum; ++i)
        out.NodeLst[i] = in.pointlist[i];

    out.ElemLst = new MeshInfo::id_t
                  [out.NumNodePerElem*(out.ElemNum = in.numberoftetrahedra)];
    for(siz_t i = 0; i < out.NumNodePerElem*out.ElemNum; ++i)
        out.ElemLst[i] = in.tetrahedronlist[i];

    out.ElemAttribPerObj = in.numberoftetrahedronattributes - 1;
    out.ElemAttribLst = out.ElemAttribPerObj ? new MeshInfo::num_t
                        [out.ElemAttribPerObj*out.ElemNum] : nullptr;
    out.ElemRegionLst = new MeshInfo::id_t[out.ElemNum];
    for(siz_t i = 0; i < out.ElemNum; i += out.ElemAttribPerObj + 1)
    {
        if(out.ElemAttribLst)
        for(siz_t j = 0; j < out.ElemAttribPerObj; ++j)
            out.ElemAttribLst[i*out.ElemAttribPerObj + j]
                = in.tetrahedronattributelist[i*(out.ElemAttribPerObj+1) + j];
        out.ElemRegionLst[i]
                = (MeshInfo::id_t)in.tetrahedronattributelist[i*(out.ElemAttribPerObj+1) + out.ElemAttribPerObj];
    }

    const auto vert_per_face = 3;
    out.ElemFaceLst = new MeshInfo::id_t[vert_per_face*(out.ElemFaceNum = in.numberoftrifaces)];
    out.ElemFaceMarkerLst = new MeshInfo::mrk_t[out.ElemFaceNum];
    for(siz_t i = 0; i < out.ElemFaceNum; ++i)
    {
        for(siz_t j = 0; j < vert_per_face; ++j)
            out.ElemFaceLst[i*vert_per_face + j] = in.trifacelist[i*vert_per_face + j];
        out.ElemFaceMarkerLst[i] = in.trifacemarkerlist[i] - 1;
    }

    out.EdgeLst = new MeshInfo::id_t[2*(out.EdgeNum = in.numberofedges)];
    for(siz_t i = 0; i < 2*out.EdgeNum; ++i)
    {
        out.EdgeLst[i] = in.edgelist[i];
        // look for edge marker list
    }
}

bool MeshParams::FromString(std::string& emsg,
                            char* max_radedge,
                            char* max_vol,
                            char* max_area_lst_str)
{
    MaxRadEdgeRatio = atof(max_radedge);
    MaxElemVol = atof(max_vol);

    if(MaxRadEdgeRatio < DBL_EPSILON ||     // negative too
       MaxElemVol < DBL_EPSILON)
    {
        emsg = "invalid values for numeric parameters. They must be positive.";
        return false;
    }

    MaxBoundElemFaceArea.clear();

    if(int maxarea_strsize = strlen(max_area_lst_str))
    {
        const auto semicln_ptr = strchr(max_area_lst_str, '~');
        if(!maxarea_strsize || !semicln_ptr)
        {
            emsg = "invalid string of maximum element boundary faces area.";
            return false;
        }

        char* maxarea_str = new char[maxarea_strsize + 1];
        strcpy_s(maxarea_str, maxarea_strsize + 1, semicln_ptr+1);

        char* ptr_tmp;
        char* p_str = strtok_s(maxarea_str, "$", &ptr_tmp);
        while(p_str)
        {
            char* ptr_tmp2;
            char* num_str = strtok_s(p_str, "@", &ptr_tmp2);
            int id = atoi(num_str);

            num_str = strtok_s(nullptr, "@", &ptr_tmp2);
            double num;
            if(!num_str || (num = atof(num_str)) < DBL_EPSILON)
            {
                emsg = "area for face '" + std::to_string(id) + "' is not positive or not specified.";
                delete[] maxarea_str;
                return false;
            }

            MaxBoundElemFaceArea[id] = num;

            p_str = strtok_s(nullptr, "$", &ptr_tmp);
        }

        delete[] maxarea_str;
    }

    return true;
}

bool MeshParams::IsValid(std::string& error_msg)
{
    error_msg = "";

    if(MaxElemVol < 0.0)
        error_msg += "Maximal element volume is negative;\n";
    if(MaxRadEdgeRatio < 0.0)
        error_msg += "Maximal radius-edge ratio is negative;\n";

    for(const auto& p: MaxBoundElemFaceArea)
        if(p.second <= 0.0)
            error_msg += "Max area of boundary face of elements on shape's face '"
                         + std::to_string(p.first) + "' is negative;\n";

    return error_msg.empty();
}

std::string MeshParams::ToString() const
{
    return "q" + std::to_string(MaxRadEdgeRatio) +
           "a" + std::to_string(MaxElemVol) + (MaxBoundElemFaceArea.empty() ? "" : "a");
}

const char* TetGenErrorCodeToString(const TetGenErrorCode code)
{
    switch(code)
    {
        case TET_GEN_ECODE_OUT_OF_MEMORY:
            return "Out of memory";
        case TET_GEN_ECODE_INTERNAL_ERROR:
            return PLUGIN_NAME " internal error";
        case TET_GEN_ECODE_SELF_INTERSECT:
            return "Geometry self-intersection detected";
        case TET_GEN_ECODE_BAD_GEOM_PRECISION:
            return "Geometry features are too small for current tolerance";
        case TET_GEN_ECODE_CLOSE_FACETS:
            return "Some facets are too close to each other";
        case TET_GEN_ECODE_INPUT_ERROR:
            return "Plugin input error";
    }

    return nullptr;
}

extern "C" int printf_modified(const char *fmt, ...)
{
   va_list argp;
   va_start(argp, fmt);

   int ret = vsnprintf(nullptr, 0, fmt, argp);
   if(ret <= 0)
       return ret;

   va_end(argp);
   va_start(argp, fmt);

   std::string buf(ret+1, '\0');
   ret = vsnprintf(&buf[0], ret+1, fmt, argp);
   gStatsBuf += buf.c_str();

   return ret;
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
