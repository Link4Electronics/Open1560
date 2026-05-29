#ifdef ARTS_HAVE_FFMPEG

define_dummy_symbol(mmvid_videoplayer);

#include "videoplayer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>

#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

static bool GetSDLFormat(AVSampleFormat fmt, SDL_AudioFormat& sdl_fmt, i32& bytes_per_sample)
{
    switch (fmt)
    {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
            sdl_fmt = SDL_AUDIO_U8;
            bytes_per_sample = 1;
            return true;
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
            sdl_fmt = SDL_AUDIO_S16LE;
            bytes_per_sample = 2;
            return true;
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
            sdl_fmt = SDL_AUDIO_S32LE;
            bytes_per_sample = 4;
            return true;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
            sdl_fmt = SDL_AUDIO_F32LE;
            bytes_per_sample = 4;
            return true;
        default:
            return false;
    }
}

bool PlayIntroVideo(SDL_Window* window, const char* filepath)
{
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, filepath, nullptr, nullptr) < 0)
        return false;

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0)
    {
        avformat_close_input(&fmt_ctx);
        return false;
    }

    i32 video_stream = -1;
    i32 audio_stream = -1;

    for (unsigned i = 0; i < fmt_ctx->nb_streams; ++i)
    {
        AVCodecParameters* par = fmt_ctx->streams[i]->codecpar;

        if (par->codec_type == AVMEDIA_TYPE_VIDEO && video_stream < 0)
            video_stream = static_cast<i32>(i);

        if (par->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream < 0)
            audio_stream = static_cast<i32>(i);
    }

    if (video_stream < 0)
    {
        avformat_close_input(&fmt_ctx);
        return false;
    }

    AVCodecParameters* vpar = fmt_ctx->streams[video_stream]->codecpar;
    const AVCodec* vcodec = avcodec_find_decoder(vpar->codec_id);

    if (!vcodec)
    {
        avformat_close_input(&fmt_ctx);
        return false;
    }

    AVCodecContext* vctx = avcodec_alloc_context3(vcodec);
    avcodec_parameters_to_context(vctx, vpar);

    if (avcodec_open2(vctx, vcodec, nullptr) < 0)
    {
        avcodec_free_context(&vctx);
        avformat_close_input(&fmt_ctx);
        return false;
    }

    SwsContext* sws = sws_getContext(vctx->width, vctx->height, vctx->pix_fmt, vctx->width, vctx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        sws_freeContext(sws);
        avcodec_free_context(&vctx);
        avformat_close_input(&fmt_ctx);
        return false;
    }

    // Maintain aspect ratio with letterboxing
    SDL_SetRenderLogicalPresentation(renderer, vctx->width, vctx->height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, vctx->width, vctx->height);

    f64 fps = av_q2d(av_guess_frame_rate(fmt_ctx, fmt_ctx->streams[video_stream], nullptr));
    if (fps <= 0.0)
        fps = 15.0;

    f64 frame_duration = 1.0 / fps;

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    i32 rgb_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, vctx->width, vctx->height, 1);
    std::vector<u8> rgb_buf(static_cast<usize>(rgb_size));

    // Audio setup
    AVCodecContext* actx = nullptr;
    SDL_AudioStream* audio_stream_handle = nullptr;

    if (audio_stream >= 0)
    {
        AVCodecParameters* apar = fmt_ctx->streams[audio_stream]->codecpar;
        const AVCodec* acodec = avcodec_find_decoder(apar->codec_id);

        if (acodec)
        {
            actx = avcodec_alloc_context3(acodec);
            avcodec_parameters_to_context(actx, apar);

            if (avcodec_open2(actx, acodec, nullptr) == 0)
            {
                SDL_AudioFormat sdl_fmt {};
                i32 bytes_per_sample = 0;

                if (GetSDLFormat(actx->sample_fmt, sdl_fmt, bytes_per_sample))
                {
                    // Audio subsystem may not be initialized yet (video plays before main menu)
                    SDL_InitSubSystem(SDL_INIT_AUDIO);

                    SDL_AudioSpec aspec {};
                    aspec.format = sdl_fmt;
                    aspec.channels = static_cast<Uint8>(actx->ch_layout.nb_channels);
                    aspec.freq = actx->sample_rate;

                    audio_stream_handle =
                        SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &aspec, nullptr, nullptr);

                    if (audio_stream_handle)
                        SDL_ResumeAudioStreamDevice(audio_stream_handle);
                }
            }
        }
    }

    bool quit = false;
    u64 start_tick = SDL_GetTicks();
    i32 frame_index = 0;

    while (av_read_frame(fmt_ctx, packet) >= 0 && !quit)
    {
        if (packet->stream_index == video_stream)
        {
            if (avcodec_send_packet(vctx, packet) >= 0)
            {
                while (avcodec_receive_frame(vctx, frame) >= 0)
                {
                    AVFrame* rgb_tmp = av_frame_alloc();
                    av_image_fill_arrays(
                        rgb_tmp->data, rgb_tmp->linesize, rgb_buf.data(), AV_PIX_FMT_RGB24, vctx->width, vctx->height, 1);

                    sws_scale(sws, frame->data, frame->linesize, 0, frame->height, rgb_tmp->data, rgb_tmp->linesize);

                    SDL_UpdateTexture(texture, nullptr, rgb_buf.data(), vctx->width * 3);

                    SDL_RenderClear(renderer);
                    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
                    SDL_RenderPresent(renderer);

                    av_frame_free(&rgb_tmp);

                    u64 target_ms = static_cast<u64>(frame_index * frame_duration * 1000.0);
                    u64 elapsed = SDL_GetTicks() - start_tick;

                    if (target_ms > elapsed)
                        SDL_Delay(static_cast<u32>(target_ms - elapsed));

                    ++frame_index;

                    SDL_Event event;

                    while (SDL_PollEvent(&event))
                    {
                        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                            event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN)
                        {
                            quit = true;
                        }
                    }
                }
            }
        }
        else if (packet->stream_index == audio_stream && actx)
        {
            // Decode audio packets and feed decoded PCM to SDL
            if (avcodec_send_packet(actx, packet) >= 0)
            {
                while (avcodec_receive_frame(actx, frame) >= 0)
                {
                    if (audio_stream_handle)
                    {
                        i32 data_size = av_get_bytes_per_sample(actx->sample_fmt) * frame->nb_samples *
                            (actx->ch_layout.nb_channels);

                        SDL_PutAudioStreamData(audio_stream_handle, frame->extended_data[0], data_size);
                    }
                }
            }
        }

        av_packet_unref(packet);
    }

    // Drain remaining frames
    if (!quit)
    {
        if (actx)
            avcodec_send_packet(actx, nullptr);

        avcodec_send_packet(vctx, nullptr);

        bool video_done = false;
        bool audio_done = !actx;

        while (!video_done || !audio_done)
        {
            if (!video_done)
            {
                i32 ret = avcodec_receive_frame(vctx, frame);
                if (ret == 0)
                {
                    AVFrame* rgb_tmp = av_frame_alloc();
                    av_image_fill_arrays(rgb_tmp->data, rgb_tmp->linesize, rgb_buf.data(), AV_PIX_FMT_RGB24, vctx->width,
                        vctx->height, 1);

                    sws_scale(sws, frame->data, frame->linesize, 0, frame->height, rgb_tmp->data, rgb_tmp->linesize);

                    SDL_UpdateTexture(texture, nullptr, rgb_buf.data(), vctx->width * 3);

                    SDL_RenderClear(renderer);
                    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
                    SDL_RenderPresent(renderer);

                    av_frame_free(&rgb_tmp);

                    u64 target_ms = static_cast<u64>(frame_index * frame_duration * 1000.0);
                    u64 elapsed = SDL_GetTicks() - start_tick;

                    if (target_ms > elapsed)
                        SDL_Delay(static_cast<u32>(target_ms - elapsed));

                    ++frame_index;
                }
                else
                {
                    video_done = true;
                }
            }

            if (!audio_done)
            {
                i32 ret = avcodec_receive_frame(actx, frame);
                if (ret == 0)
                {
                    if (audio_stream_handle)
                    {
                        i32 data_size = av_get_bytes_per_sample(actx->sample_fmt) * frame->nb_samples *
                            (actx->ch_layout.nb_channels);

                        SDL_PutAudioStreamData(audio_stream_handle, frame->extended_data[0], data_size);
                    }
                }
                else
                {
                    audio_done = true;
                }
            }
        }
    }

    // Let audio finish
    if (audio_stream_handle && !quit)
    {
        while (SDL_GetAudioStreamAvailable(audio_stream_handle) > 0)
            SDL_Delay(16);
    }

    av_packet_free(&packet);
    av_frame_free(&frame);

    sws_freeContext(sws);
    avcodec_free_context(&vctx);

    if (actx)
        avcodec_free_context(&actx);

    avformat_close_input(&fmt_ctx);

    if (audio_stream_handle)
        SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(audio_stream_handle));

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    return true;
}

#endif // ARTS_HAVE_FFMPEG
