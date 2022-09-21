﻿// -----------------------------------------------------------------------------------------
// QSVEnc by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2011-2016 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

#ifndef _QSV_QUERY_H_
#define _QSV_QUERY_H_

#include "rgy_tchar.h"
#include <emmintrin.h>
#include <vector>
#include <array>
#include <string>
#include <chrono>
#include <memory>
#include <optional>
#include <type_traits>
#include "rgy_osdep.h"
#include "rgy_opencl.h"
#include "rgy_util.h"
#include "rgy_version.h"
#include "cpu_info.h"
#include "gpu_info.h"
#include "qsv_util.h"

// VP9ではmfxExtCodingOptionはチェックしないようにしないと正常に動作しない
#define AVOID_COP_EXCEPT_SAFE_CODECS 1

static bool add_cop(const uint32_t CodecId) {
    if (!AVOID_COP_EXCEPT_SAFE_CODECS) return true;
    return CodecId == MFX_CODEC_AVC || CodecId == MFX_CODEC_HEVC || CodecId == MFX_CODEC_MPEG2;
}

static bool add_vui(const uint32_t CodecId) {
    return CodecId == MFX_CODEC_AVC || CodecId == MFX_CODEC_HEVC || CodecId == MFX_CODEC_MPEG2 || CodecId == MFX_CODEC_AV1;
}

enum QSV_CPU_GEN {
    CPU_GEN_UNKNOWN = 0,
    CPU_GEN_SANDYBRIDGE,
    CPU_GEN_IVYBRIDGE,
    CPU_GEN_SILVERMONT,
    CPU_GEN_HASWELL,
    CPU_GEN_AIRMONT,
    CPU_GEN_BROADWELL,
    CPU_GEN_SKYLAKE,
    CPU_GEN_GOLDMONT,
    CPU_GEN_KABYLAKE,
    CPU_GEN_GEMINILAKE,
    CPU_GEN_COFFEELAKE,
    CPU_GEN_CANNONLAKE,
    CPU_GEN_ICELAKE,
    CPU_GEN_JASPERLAKE,
    CPU_GEN_ELKHARTLAKE,
    CPU_GEN_TIGERLAKE,
    CPU_GEN_ROCKETLAKE,
    CPU_GEN_ALDERLAKE_S,
    CPU_GEN_ALDERLAKE_P,
    CPU_GEN_ARCTICSOUND_P,
    CPU_GEN_XEHP_SDV,
    CPU_GEN_DG2,
    CPU_GEN_ATS_M,
    CPU_GEN_ALDERLAKE_N,
    CPU_GEN_KEEMBAY,

    CPU_GEN_MAX,
};

static const TCHAR *const CPU_GEN_STR[] = {
    _T(""),
    _T("SandyBridge"),
    _T("IvyBridge"),
    _T("Silvermont"),
    _T("Haswell"),
    _T("Airmont"),
    _T("Broadwell"),
    _T("Skylake"),
    _T("Apollolake"),
    _T("Kabylake"),
    _T("Geminilake"),
    _T("Coffeelake"),
    _T("Cannonlake"),
    _T("Icelake"),
    _T("Jasperlake"),
    _T("Elkhartlake"),
    _T("Tigerlake"),
    _T("Rocketlake"),
    _T("AlderlakeS"),
    _T("AlderlakeP"),
    _T("ArcticSoundP"),
    _T("XeHP_SDV"),
    _T("DG2"),
    _T("ArcticSoundM"),
    _T("AlderlakeN"),
    _T("Keembay")
};

static_assert(_countof(CPU_GEN_STR) == CPU_GEN_MAX);

QSV_CPU_GEN getCPUGenCpuid();
QSV_CPU_GEN getCPUGen(MFXVideoSession *pSession);

static constexpr mfxVersion LIB_VER_LIST[] = {
    {  0, 0 },
    {  0, 1 },
    {  1, 1 },
    {  3, 1 },
    {  4, 1 },
    {  6, 1 },
    {  7, 1 },
    {  8, 1 },
    {  9, 1 },
    { 10, 1 },
    { 11, 1 },
    { 13, 1 },
    { 15, 1 },
    { 16, 1 },
    { 17, 1 },
    { 19, 1 },
    { 23, 1 },
    { 26, 1 },
    { 27, 1 },
    { 33, 1 },
    { 34, 1 },
    { 35, 1 },
    {  0, 2 },
    {  1, 2 },
    {  2, 2 },
    {  3, 2 },
    {  4, 2 },
    {  5, 2 },
    {  6, 2 },
    {  0, 0 }
};

#define MFX_LIB_VERSION(major, minor, idx) \
    static constexpr mfxVersion MFX_LIB_VERSION_ ## major ## _ ## minor = LIB_VER_LIST[(idx)]; \
    static_assert(MFX_LIB_VERSION_ ## major ## _ ## minor.Major == (major) && MFX_LIB_VERSION_ ## major ## _ ## minor.Minor == (minor), "MFX_LIB_VERSION");

MFX_LIB_VERSION(0, 0,  0);
MFX_LIB_VERSION(1, 0,  1);
MFX_LIB_VERSION(1, 1,  2);
MFX_LIB_VERSION(1, 3,  3);
MFX_LIB_VERSION(1, 4,  4);
MFX_LIB_VERSION(1, 6,  5);
MFX_LIB_VERSION(1, 7,  6);
MFX_LIB_VERSION(1, 8,  7);
MFX_LIB_VERSION(1, 9,  8);
MFX_LIB_VERSION(1,10,  9);
MFX_LIB_VERSION(1,11, 10);
MFX_LIB_VERSION(1,13, 11);
MFX_LIB_VERSION(1,15, 12);
MFX_LIB_VERSION(1,16, 13);
MFX_LIB_VERSION(1,17, 14);
MFX_LIB_VERSION(1,19, 15);
MFX_LIB_VERSION(1,23, 16);
MFX_LIB_VERSION(1,26, 17);
MFX_LIB_VERSION(1,27, 18);
MFX_LIB_VERSION(1,33, 19);
MFX_LIB_VERSION(1,34, 20);
MFX_LIB_VERSION(1,35, 21);
MFX_LIB_VERSION(2, 0, 22);
MFX_LIB_VERSION(2, 1, 23);
MFX_LIB_VERSION(2, 2, 24);
MFX_LIB_VERSION(2, 3, 25);
MFX_LIB_VERSION(2, 4, 26);
MFX_LIB_VERSION(2, 5, 27);
MFX_LIB_VERSION(2, 6, 28);

static const RGY_CODEC CODEC_LIST_AUO[] = {
    RGY_CODEC_H264,
    RGY_CODEC_HEVC,
    RGY_CODEC_VP9,
    RGY_CODEC_AV1,
};

BOOL Check_HWUsed(mfxIMPL impl);
int GetAdapterID(mfxIMPL impl);
int GetAdapterID(mfxSession session);
int GetAdapterID(MFXVideoSession *session);
mfxVersion get_mfx_libhw_version(const QSVDeviceNum deviceNum);
mfxVersion get_mfx_libsw_version();

static BOOL check_lib_version(mfxVersion value, mfxVersion required) {
    if (value.Major < required.Major)
        return FALSE;
    if (value.Major > required.Major)
        return TRUE;
    if (value.Minor < required.Minor)
        return FALSE;
    return TRUE;
}

static BOOL check_lib_version(mfxU32 _value, mfxU32 _required) {
    mfxVersion value, required;
    value.Version = _value;
    required.Version = _required;
    if (value.Major < required.Major)
        return FALSE;
    if (value.Major > required.Major)
        return TRUE;
    if (value.Minor < required.Minor)
        return FALSE;
    return TRUE;
}

static bool inline rc_is_type_lookahead(int rc) {
    return ((rc == MFX_RATECONTROL_LA)
        | (rc == MFX_RATECONTROL_LA_ICQ)
        | (rc == MFX_RATECONTROL_LA_HRD));
}

enum : uint64_t {
    ENC_FEATURE_CURRENT_RC             = 0x0000000000000001,
    ENC_FEATURE_AVBR                   = 0x0000000000000002,
    ENC_FEATURE_LA                     = 0x0000000000000004,
    ENC_FEATURE_ICQ                    = 0x0000000000000008,
    ENC_FEATURE_LA_ICQ                 = 0x0000000000000010,
    ENC_FEATURE_VCM                    = 0x0000000000000020,
    ENC_FEATURE_AUD                    = 0x0000000000000040,
    ENC_FEATURE_PIC_STRUCT             = 0x0000000000000080,
    ENC_FEATURE_VUI_INFO               = 0x0000000000000100,
    ENC_FEATURE_CAVLC                  = 0x0000000000000200,
    ENC_FEATURE_RDO                    = 0x0000000000000400,
    ENC_FEATURE_ADAPTIVE_I             = 0x0000000000000800,
    ENC_FEATURE_ADAPTIVE_B             = 0x0000000000001000,
    ENC_FEATURE_B_PYRAMID              = 0x0000000000002000,
    ENC_FEATURE_TRELLIS                = 0x0000000000004000,
    ENC_FEATURE_EXT_BRC                = 0x0000000000008000,
    ENC_FEATURE_MBBRC                  = 0x0000000000010000,
    ENC_FEATURE_LA_DS                  = 0x0000000000020000,
    ENC_FEATURE_INTERLACE              = 0x0000000000040000,
    ENC_FEATURE_ADAPTIVE_REF           = 0x0000000000080000,
    ENC_FEATURE_ADAPTIVE_LTR           = 0x0000000000100000,
    ENC_FEATURE_ADAPTIVE_CQM           = 0x0000000000200000,
    ENC_FEATURE_B_PYRAMID_MANY_BFRAMES = 0x0000000000400000,
    ENC_FEATURE_LA_HRD                 = 0x0000000000800000,
    ENC_FEATURE_QVBR                   = 0x0000000001000000,
    ENC_FEATURE_INTRA_REFRESH          = 0x0000000002000000,
    ENC_FEATURE_NO_DEBLOCK             = 0x0000000004000000,
    ENC_FEATURE_QP_MINMAX              = 0x0000000008000000,
    ENC_FEATURE_WINBRC                 = 0x0000000010000000,
    ENC_FEATURE_PERMBQP                = 0x0000000020000000,
    ENC_FEATURE_DIRECT_BIAS_ADJUST     = 0x0000000040000000,
    ENC_FEATURE_GLOBAL_MOTION_ADJUST   = 0x0000000080000000,
    ENC_FEATURE_GOPREFDIST             = 0x0000000100000000,
    ENC_FEATURE_WEIGHT_P               = 0x0000000200000000,
    ENC_FEATURE_WEIGHT_B               = 0x0000000400000000,
    ENC_FEATURE_FADE_DETECT            = 0x0000000800000000,
    ENC_FEATURE_PYRAMID_QP_OFFSET      = 0x0000001000000000,
    ENC_FEATURE_DISABLE_GPB            = 0x0000002000000000,
    ENC_FEATURE_10BIT_DEPTH            = 0x0000004000000000,
    ENC_FEATURE_HEVC_SAO               = 0x0000008000000000,
    ENC_FEATURE_HEVC_CTU               = 0x0000010000000000,
    ENC_FEATURE_HEVC_TSKIP             = 0x0000020000000000,
    ENC_FEATURE_HYPER_MODE             = 0x0000040000000000,
};

enum : uint64_t {
    VPP_FEATURE_RESIZE                = 0x00000001,
    VPP_FEATURE_DENOISE               = 0x00000002,
    VPP_FEATURE_DETAIL_ENHANCEMENT    = 0x00000004,
    VPP_FEATURE_PROC_AMP              = 0x00000008,
    VPP_FEATURE_IMAGE_STABILIZATION   = 0x00000010,
    VPP_FEATURE_VIDEO_SIGNAL_INFO     = 0x00000020,
    VPP_FEATURE_FPS_CONVERSION        = 0x00000040,
    VPP_FEATURE_FPS_CONVERSION_ADV    = 0x00000080 | VPP_FEATURE_FPS_CONVERSION,
    VPP_FEATURE_DEINTERLACE           = 0x00000100,
    VPP_FEATURE_DEINTERLACE_AUTO      = 0x00000200,
    VPP_FEATURE_DEINTERLACE_IT_MANUAL = 0x00000400,
    VPP_FEATURE_ROTATE                = 0x00000800,
    VPP_FEATURE_MIRROR                = 0x00001000,
    VPP_FEATURE_SCALING_QUALITY       = 0x00002000,
    VPP_FEATURE_MCTF                  = 0x00004000,
    VPP_FEATURE_DENOISE2              = 0x00008000,
};

static const CX_DESC list_rate_control_ry[] = {
    { _T("CBR  "), MFX_RATECONTROL_CBR    },
    { _T("VBR  "), MFX_RATECONTROL_VBR    },
    { _T("AVBR "), MFX_RATECONTROL_AVBR   },
    { _T("QVBR "), MFX_RATECONTROL_QVBR   },
    { _T("CQP  "), MFX_RATECONTROL_CQP    },
    { _T("LA   "), MFX_RATECONTROL_LA     },
    { _T("LAHRD"), MFX_RATECONTROL_LA_HRD },
    { _T("ICQ  "), MFX_RATECONTROL_ICQ    },
    { _T("LAICQ"), MFX_RATECONTROL_LA_ICQ },
    //{ _T("LAEXT"), MFX_RATECONTROL_LA_EXT },
    { _T("VCM  "), MFX_RATECONTROL_VCM    },
};
static const FEATURE_DESC list_enc_feature[] = {
    { _T("RC mode      "), ENC_FEATURE_CURRENT_RC             },
    { _T("10bit depth  "), ENC_FEATURE_10BIT_DEPTH            },
    { _T("Hyper Mode   "), ENC_FEATURE_HYPER_MODE             },
    { _T("Interlace    "), ENC_FEATURE_INTERLACE              },
    { _T("VUI info     "), ENC_FEATURE_VUI_INFO               },
    //{ _T("aud          "), ENC_FEATURE_AUD                    },
    //{ _T("pic_struct   "), ENC_FEATURE_PIC_STRUCT             },
    { _T("Trellis      "), ENC_FEATURE_TRELLIS                },
    //{ _T("rdo          "), ENC_FEATURE_RDO                    },
    //{ _T("CAVLC        "), ENC_FEATURE_CAVLC                  },
    { _T("BFrame/GopRef"), ENC_FEATURE_GOPREFDIST             },
    { _T("Adaptive_I   "), ENC_FEATURE_ADAPTIVE_I             },
    { _T("Adaptive_B   "), ENC_FEATURE_ADAPTIVE_B             },
    { _T("WeightP      "), ENC_FEATURE_WEIGHT_P               },
    { _T("WeightB      "), ENC_FEATURE_WEIGHT_B               },
    { _T("FadeDetect   "), ENC_FEATURE_FADE_DETECT            },
    { _T("B_Pyramid    "), ENC_FEATURE_B_PYRAMID              },
    { _T(" +ManyBframes"), ENC_FEATURE_B_PYRAMID_MANY_BFRAMES },
    { _T("PyramQPOffset"), ENC_FEATURE_PYRAMID_QP_OFFSET      },
    { _T("MBBRC        "), ENC_FEATURE_MBBRC                  },
    { _T("ExtBRC       "), ENC_FEATURE_EXT_BRC                },
    { _T("AdaptiveRef  "), ENC_FEATURE_ADAPTIVE_REF           },
    { _T("AdaptiveLTR  "), ENC_FEATURE_ADAPTIVE_LTR           },
    { _T("AdaptiveCQM  "), ENC_FEATURE_ADAPTIVE_CQM           },
    { _T("LA Quality   "), ENC_FEATURE_LA_DS                  },
    { _T("QP Min/Max   "), ENC_FEATURE_QP_MINMAX              },
    { _T("IntraRefresh "), ENC_FEATURE_INTRA_REFRESH          },
    { _T("No Deblock   "), ENC_FEATURE_NO_DEBLOCK             },
    { _T("No GPB       "), ENC_FEATURE_DISABLE_GPB            },
    { _T("Windowed BRC "), ENC_FEATURE_WINBRC                 },
    { _T("PerMBQP(CQP) "), ENC_FEATURE_PERMBQP                },
    { _T("DirectBiasAdj"), ENC_FEATURE_DIRECT_BIAS_ADJUST     },
    { _T("MVCostScaling"), ENC_FEATURE_GLOBAL_MOTION_ADJUST   },
    { _T("SAO          "), ENC_FEATURE_HEVC_SAO               },
    { _T("Max CTU Size "), ENC_FEATURE_HEVC_CTU               },
    { _T("TSkip        "), ENC_FEATURE_HEVC_TSKIP             },
    { NULL, 0 },
};
static const FEATURE_DESC list_vpp_feature[] = {
    { _T("Resize               "), VPP_FEATURE_RESIZE              },
    { _T("Deinterlace          "), VPP_FEATURE_DEINTERLACE         },
    { _T("Scaling Quality      "), VPP_FEATURE_SCALING_QUALITY     },
    { _T("Denoise              "), VPP_FEATURE_DENOISE             },
    { _T("Denoise2             "), VPP_FEATURE_DENOISE2            },
    { _T("Mctf                 "), VPP_FEATURE_MCTF                },
    { _T("Rotate               "), VPP_FEATURE_ROTATE              },
    { _T("Mirror               "), VPP_FEATURE_MIRROR              },
    { _T("Detail Enhancement   "), VPP_FEATURE_DETAIL_ENHANCEMENT  },
    { _T("Proc Amp.            "), VPP_FEATURE_PROC_AMP            },
    { _T("Image Stabilization  "), VPP_FEATURE_IMAGE_STABILIZATION },
    { _T("Video Signal Info    "), VPP_FEATURE_VIDEO_SIGNAL_INFO   },
    { _T("FPS Conversion       "), VPP_FEATURE_FPS_CONVERSION      },
    { _T("FPS Conversion (Adv.)"), VPP_FEATURE_FPS_CONVERSION_ADV  },
    { NULL, 0 },
};

struct QSVEncFeatureData {
    QSVDeviceNum dev;
    RGY_CODEC codec;
    bool lowPwer;
    std::vector<uint64_t> feature;
};

enum FeatureListStrType {
    FEATURE_LIST_STR_TYPE_UNKNOWN,
    FEATURE_LIST_STR_TYPE_TXT,
    FEATURE_LIST_STR_TYPE_CSV,
    FEATURE_LIST_STR_TYPE_HTML,
};

struct QSVVideoParam {
    mfxVersion mfxVer;
    bool isVppParam;
    mfxVideoParam videoPrmVpp;
    mfxVideoParam videoPrm;
    std::vector<mfxExtBuffer *> buf;

    uint8_t spsbuf[256];
    uint8_t ppsbuf[256];
    mfxExtCodingOptionSPSPPS spspps;
    mfxExtCodingOption cop;
    mfxExtCodingOption2 cop2;
    mfxExtCodingOption3 cop3;
    mfxExtVP8CodingOption copVp8;
    mfxExtVP9Param vp9Prm;
    mfxExtHEVCParam hevcPrm;
    mfxExtAV1BitstreamParam av1BitstreamPrm;
    mfxExtAV1ResolutionParam av1ResolutionPrm;
    mfxExtAV1TileParam av1TilePrm;
    mfxExtHyperModeParam hyperModePrm;

    QSVVideoParam(uint32_t CodecId, mfxVersion mfxver_);
    QSVVideoParam() = delete;
    QSVVideoParam(const QSVVideoParam&) = delete;             // 禁止
    QSVVideoParam &operator=(const QSVVideoParam &) = delete; // 禁止
    ~QSVVideoParam() {};
};

uint64_t CheckEncodeFeature(MFXVideoSession& session, const int ratecontrol, const RGY_CODEC codec, const bool lowPower);
uint64_t CheckEncodeFeatureWithPluginLoad(MFXVideoSession& session, const int ratecontrol, const RGY_CODEC codec, const bool lowPower);
QSVEncFeatureData MakeFeatureList(const QSVDeviceNum deviceNum, const std::vector<CX_DESC>& rateControlList, const RGY_CODEC codecId, const bool lowPower, std::shared_ptr<RGYLog> log);
std::vector<QSVEncFeatureData> MakeFeatureListPerCodec(const QSVDeviceNum deviceNum, const std::vector<CX_DESC>& rateControlList, const std::vector<RGY_CODEC>& codecIdList, std::shared_ptr<RGYLog> log);

tstring MakeFeatureListStr(const uint64_t feature);
std::vector<std::pair<QSVEncFeatureData, tstring>> MakeFeatureListStr(const QSVDeviceNum deviceNum, const FeatureListStrType type, std::shared_ptr<RGYLog> log);
std::vector<std::pair<QSVEncFeatureData, tstring>> MakeFeatureListStr(const QSVDeviceNum deviceNum, const FeatureListStrType type, const vector<RGY_CODEC>& codecLists, std::shared_ptr<RGYLog> log);

mfxU64 CheckVppFeatures(MFXVideoSession& session);
mfxU64 CheckVppFeatures(const QSVDeviceNum deviceNum, std::shared_ptr<RGYLog> log);
tstring MakeVppFeatureStr(const QSVDeviceNum deviceNum, FeatureListStrType outputType, std::shared_ptr<RGYLog> log);

std::vector<RGY_CSP> CheckDecFeaturesInternal(MFXVideoSession& session, mfxVersion mfxVer, mfxU32 codecId);
CodecCsp MakeDecodeFeatureList(const QSVDeviceNum deviceNum, const vector<mfxU32>& codecIdList, std::shared_ptr<RGYLog> log, const bool skipHWDecodeCheck);
tstring MakeDecFeatureStr(const QSVDeviceNum deviceNum, FeatureListStrType type, std::shared_ptr<RGYLog> log);
CodecCsp getHWDecCodecCsp(const QSVDeviceNum deviceNum, std::shared_ptr<RGYLog> log, const bool skipHWDecodeCheck);

int GetImplListStr(tstring& str);
std::vector<tstring> getDeviceList();
#if ENABLE_OPENCL
std::optional<RGYOpenCLDeviceInfo> getDeviceCLInfoQSV(const QSVDeviceNum dev);
#endif

#endif //_QSV_QUERY_H_
