#include "test/test.hpp"
#include "util/log.hpp"
#include "util/filesys.hpp"
#include "data/textstream.hpp"
#include "defs/matrix.hpp"
#include "core/shlib_mgr.hpp"
#include "plug/pluginmgr.hpp"
#include "plug/pluginsets.hpp"


namespace test
{
    using util::logger::Print;
    using util::logger::PrintE;

    void VecAndMatrix(const util::CmdArgList& arg_lst)
    {
        UNUSED_PARAM(arg_lst);

        String str = "\n";
        const auto m = Matrix<7, 4>::CreateSpecialMatrix(MATRIX_SPECIAL_TYPE_IDENTITY, 2.0, MATRIX_SPECIAL_TYPE_UDIAG, 4.0,
                                                         MATRIX_SPECIAL_TYPE_UDIAG, 4.0, MATRIX_SPECIAL_TYPE_LDIAG, 3.0);

        for(int i = 0; i < 7; ++i)
        {
            for(int j = 0; j < 4; ++j)
                str += TO_STRING(m.el(i, j)) + " ";
            str += "\n";
        }

        util::logger::Print(str + "\n");
    }

    void DataIO(const util::CmdArgList& arg_lst)
    {
        UNUSED_PARAM(arg_lst);

        String log_str { "\n" };

        using namespace data;

        TextStream fstr("../test/dataio_read.txt", TextStreamOpenMode::ReadOnly);

        int16 i16 = fstr.ReadInt16();
        int32 i32 = fstr.ReadInt32();
        int64 i64 = fstr.ReadInt64();

        log_str += String("Int read: %1, %2, %3\n").arg(i16).arg(i32).arg(i64);

        log_str += String("Word read: %1\nRemaining word: %2\n").arg(fstr.ReadWord()).arg(fstr.ReadWord());
        log_str += "Line read: " + fstr.ReadLine();
        log_str += "\nLine read: " + fstr.ReadLine();

        fstr.Open("../test/dataio_write.txt", TextStreamOpenMode::WriteOnly);

        fstr.WriteInt16(i16).WriteChar(' ');
        fstr.WriteInt32(i32).WriteChar(' ');
        fstr.WriteInt64(i64);
        fstr.WriteString(log_str);

        fstr.Close();

        util::logger::Print(log_str + "\n");
    }

    void Plugin(const util::CmdArgList& arg_lst)
    {
        UNUSED_PARAM(arg_lst);
        using namespace plug;

        core::ShLibMgr lib_mgr;
        plug::PluginMgr plmgr(lib_mgr);
        if(plmgr.LoadPlugin("./plugin-sample.dll") == PLUGIN_ID_NULL)
            util::logger::PrintE("Her'");
    }

    void PluginSets(const util::CmdArgList& arg_lst)
    {
        UNUSED_PARAM(arg_lst);
//        using namespace plug;

//        const auto json {util::filesys::ReadFile("../test/pluginsets.json")};
//        PluginSettings sets(json);
//        const auto m { sets.ToMap() };

//        for(const auto& s: m)
//        {
//            const auto txt = "fullname: %1\n"
//                             "desc:     %2\n"
//                             "type:     %3"_qs.arg(s.FullName)
//                             .arg(s.Desc).arg((int)s.Type);
//            Print("\n" + txt);
//        }
    }
}
