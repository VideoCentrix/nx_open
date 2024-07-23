// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include <unordered_map>
#include <vector>

#include <QtCore/QSize>

#include <CoreMedia/CMFormatDescription.h>
#include <VideoToolbox/VTDecompressionSession.h>

#include <nx/utils/platform/core_foundation_mac/cf_ref_holder.h>

#include "mac_utils.h"

namespace {

// Size to use for NALU length headers (can be 1, 2, or 4).
static constexpr int kNALUHeaderLength = 4;

bool canCreateHWDecoderSession(CMFormatDescriptionRef format)
{
    cf::QnCFRefHolder<CFMutableDictionaryRef> decoderConfig(
      CFDictionaryCreateMutable(
        kCFAllocatorDefault,
        /* capacity */ 1,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks));

    CFDictionarySetValue(
        decoderConfig,
        kVTVideoDecoderSpecification_EnableHardwareAcceleratedVideoDecoder,
        kCFBooleanTrue);
    CFDictionarySetValue(
        decoderConfig,
        kVTVideoDecoderSpecification_RequireHardwareAcceleratedVideoDecoder,
        kCFBooleanTrue);

    cf::QnCFRefHolder<VTDecompressionSessionRef> session;
    const OSStatus osStatus = VTDecompressionSessionCreate(
        kCFAllocatorDefault,
        format,
        decoderConfig,
        /* destinationImageBufferAttributes */ nullptr,
        /* outputCallback */ nullptr,
        session.into());

    return osStatus == noErr;
}

struct HevcParams
{
    std::vector<uint8_t> vps;
    std::vector<uint8_t> sps;
    std::vector<uint8_t> pps;
};

QSize checkHevc(const HevcParams& params)
{
    const uint8_t* const parameterSetPointers[] = {
        params.vps.data(), params.sps.data(), params.pps.data()};
    const size_t parameterSetSizes[] = {params.vps.size(), params.sps.size(), params.pps.size()};

    cf::QnCFRefHolder<CMFormatDescriptionRef> format;

    OSStatus osStatus = CMVideoFormatDescriptionCreateFromHEVCParameterSets(
        kCFAllocatorDefault,
        /* parameterSetCount */ 3,
        parameterSetPointers,
        parameterSetSizes,
        kNALUHeaderLength,
        nullptr,
        format.into());

    if (osStatus != noErr)
        return {};

    if (!canCreateHWDecoderSession(format))
        return {};

    const CMVideoDimensions dim = CMVideoFormatDescriptionGetDimensions(format);

    return {dim.width, dim.height};
}

struct H264Params
{
    std::vector<uint8_t> sps;
    std::vector<uint8_t> pps;
};

QSize checkH264(const H264Params& params)
{
    const uint8_t* const parameterSetPointers[] = {params.sps.data(), params.pps.data()};
    const size_t parameterSetSizes[] = {params.sps.size(), params.pps.size()};

    cf::QnCFRefHolder<CMFormatDescriptionRef> format;

    OSStatus osStatus = CMVideoFormatDescriptionCreateFromH264ParameterSets(
        kCFAllocatorDefault,
        /* parameterSetCount */ 2,
        parameterSetPointers,
        parameterSetSizes,
        kNALUHeaderLength,
        format.into());

    if (osStatus != noErr)
        return {};

    if (!canCreateHWDecoderSession(format))
        return {};

    const CMVideoDimensions dim = CMVideoFormatDescriptionGetDimensions(format);

    return {dim.width, dim.height};
}

} // namespace

/*
 *  To generate H264 and HEVC parameters for different resolutions the following commands were used:
 *
 *      ffmpeg -i sample.mp4 -s 8192x4320 -t 1 -c:v hevc -pix_fmt yuv420p -c:a copy tmp.mp4
 *      ffmpeg -i tmp.mp4 -vcodec copy -bsf hevc_mp4toannexb -an stream.hevc
 *
 *  Then sps, pps, etc were extracted from the stream.hevc file.
 */

NX_MEDIA_API QSize mac_maxDecodeResolutionH264()
{
    const std::vector<H264Params> params = {
        // h264 8192x4320 yuv420p
        {
            .sps = {
                0x67, 0x64, 0x00, 0x3d, 0xac, 0xd9, 0x40, 0x08, 0x00, 0x02, 0x1d, 0xbf, 0xf0, 0x02,
                0xd0, 0x04, 0x01, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x78, 0x0f,
                0x08, 0x04, 0x19, 0x60 },
            .pps = {
                0x68, 0xeb, 0xe3, 0xcb, 0x22, 0xc0 },
        },

        // h264 4096x2160 yuv420p
        {
            .sps = {
                0x67, 0x64, 0x00, 0x34, 0xac, 0xd9, 0x40, 0x10, 0x00, 0x10, 0xfb, 0xff, 0x00, 0x2d,
                0x00, 0x40, 0x10, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x07, 0x80, 0xf1, 0x83,
                0x19, 0x60 },
            .pps = {
                0x68, 0xeb, 0xe3, 0xcb, 0x22, 0xc0 },
        },

        // h264 1920x1080 yuv420p
        {
            .sps = {
                0x67, 0x64, 0x00, 0x2a, 0xac, 0xd9, 0x40, 0x78, 0x02, 0x27, 0xe5, 0xff, 0xc0, 0x00,
                0xc0, 0x01, 0x04, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x03, 0x01, 0xe0, 0x3c,
                0x60, 0xc6, 0x58 },
            .pps = {
                0x68, 0xeb, 0xe3, 0xcb, 0x22, 0xc0 },
        },

        // h264 1280x720 yuv420p
        {
            .sps = {
                0x67, 0x64, 0x00, 0x20, 0xac, 0xd9, 0x40, 0x50, 0x05, 0xbb, 0xff, 0x00, 0x03, 0x00,
                0x04, 0x10, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x07, 0x80, 0xf1, 0x83, 0x19,
                0x60 },
            .pps = {
                0x68, 0xeb, 0xe3, 0xcb, 0x22, 0xc0 },
        },
    };

    // Parameters are ordered by resolution from the highest to the lowest.
    for (const H264Params& param: params)
    {
        if (const QSize size = checkH264(param); size.isValid())
            return size;
    }

    return {};
}

NX_MEDIA_API QSize mac_maxDecodeResolutionHevc()
{
    const std::vector<HevcParams> params = {
        // hevc 8192x4320 yuv420p
        {
            .vps = {
                0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00,
                0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0xb7, 0x95, 0x98, 0x09 },
            .sps = {
                0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00, 0x00, 0x03, 0x00,
                0x00, 0x03, 0x00, 0xb7, 0xa0, 0x00, 0x40, 0x02, 0x00, 0x10, 0xe1, 0x65, 0x95, 0x9a,
                0x49, 0x32, 0xbf, 0xfc, 0x00, 0xb4, 0x01, 0x01, 0xa7, 0x08, 0x00, 0x00, 0x03, 0x00,
                0x08, 0x00, 0x00, 0x03, 0x01, 0xe0, 0x40 },
            .pps = {
                0x44, 0x01, 0xc1, 0x72, 0xb4, 0x62, 0x40 },
        },

        // hevc 4096x4096 yuv420p
        {
            .vps = {
                0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00,
                0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0xb4, 0x95, 0x98, 0x09 },
            .sps = {
                0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00, 0x00, 0x03, 0x00,
                0x00, 0x03, 0x00, 0xb4, 0xa0, 0x00, 0x80, 0x08, 0x00, 0x40, 0x05, 0x96, 0x56, 0x69,
                0x24, 0xca, 0xf0, 0xe6, 0x9c, 0x20, 0x00, 0x00, 0x03, 0x00, 0x20, 0x00, 0x00, 0x07,
                0x81 },
            .pps = {
                0x44, 0x01, 0xc1, 0x72, 0xb4, 0x62, 0x40 },
        },

        // hevc 4096x2160 yuv420p
        {
            .vps = {
                0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00,
                0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x99, 0x95, 0x98, 0x09 },
            .sps = {
                0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00, 0x00, 0x03, 0x00,
                0x00, 0x03, 0x00, 0x99, 0xa0, 0x00, 0x80, 0x08, 0x00, 0x87, 0x16, 0x59, 0x59, 0xa4,
                0x93, 0x2b, 0xff, 0xc0, 0x0b, 0x40, 0x10, 0x1a, 0x70, 0x80, 0x00, 0x00, 0x03, 0x00,
                0x80, 0x00, 0x00, 0x1e, 0x04 },
            .pps = {
                0x44, 0x01, 0xc1, 0x72, 0xb4, 0x62, 0x40 },
        },

        // hevc 1920x1080 yuv420p
        {
            .vps = {
                0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00,
                0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x7b, 0x95, 0x98, 0x09 },
            .sps = {
                0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00, 0x00, 0x03, 0x00,
                0x00, 0x03, 0x00, 0x7b, 0xa0, 0x03, 0xc0, 0x80, 0x10, 0xe5, 0x96, 0x56, 0x69, 0x24,
                0xca, 0xff, 0xf0, 0x00, 0x30, 0x00, 0x46, 0x9c, 0x20, 0x00, 0x00, 0x03, 0x00, 0x20,
                0x00, 0x00, 0x07, 0x81 },
            .pps = {
                0x44, 0x01, 0xc1, 0x72, 0xb4, 0x62, 0x40 },
        },

        // hevc 1280x720 yuv420p
        {
            .vps = {
                0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00,
                0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x78, 0x95, 0x98, 0x09 },
            .sps = {
                0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00, 0x90, 0x00, 0x00, 0x03, 0x00,
                0x00, 0x03, 0x00, 0x78, 0xa0, 0x02, 0x80, 0x80, 0x2d, 0x16, 0x59, 0x59, 0xa4, 0x93,
                0x2b, 0xff, 0xc0, 0x00, 0xc0, 0x01, 0x1a, 0x70, 0x80, 0x00, 0x00, 0x03, 0x00, 0x80,
                0x00, 0x00, 0x1e, 0x04 },
            .pps = {
                0x44, 0x01, 0xc1, 0x72, 0xb4, 0x62, 0x40 },
        },
    };

    // Parameters are ordered by resolution from the highest to the lowest.
    for (const HevcParams& param: params)
    {
        if (const QSize size = checkHevc(param); size.isValid())
            return size;
    }

    return {};
}

bool mac_isHWDecodingSupported(const AVCodecID codec)
{
    // VideoToolBox supports H263 only.
    // If cheat it and provide H263P instead (by changing compression type H263P -> H263)
    // it would show green box.
    static const std::unordered_map<AVCodecID, CMVideoCodecType> kCodecToCMVideoCodecType = {
        {AV_CODEC_ID_H263, kCMVideoCodecType_H263},
        {AV_CODEC_ID_H264, kCMVideoCodecType_H264},
        {AV_CODEC_ID_HEVC, kCMVideoCodecType_HEVC},
        {AV_CODEC_ID_MPEG1VIDEO, kCMVideoCodecType_MPEG1Video},
        {AV_CODEC_ID_MPEG2VIDEO, kCMVideoCodecType_MPEG2Video},
        {AV_CODEC_ID_MPEG4, kCMVideoCodecType_MPEG4Video},
        {AV_CODEC_ID_VP9, kCMVideoCodecType_VP9},
        {AV_CODEC_ID_AV1, kCMVideoCodecType_AV1},
    };

    const auto it = kCodecToCMVideoCodecType.find(codec);
    if (it == kCodecToCMVideoCodecType.end())
        return false;

    return VTIsHardwareDecodeSupported(it->second);
}