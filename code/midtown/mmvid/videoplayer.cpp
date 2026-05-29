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

    SDL_AudioStream* audio_stream_handle = nullptr;

    if (audio_stream >= 0)
    {
        SDL_AudioSpec aspec {};
        aspec.format = SDL_AUDIO_U8;
        aspec.channels = 1;
        aspec.freq = fmt_ctx->streams[audio_stream]->codecpar->sample_rate;

        audio_stream_handle = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &aspec, nullptr, nullptr);

        if (audio_stream_handle)
            SDL_ResumeAudioStreamDevice(audio_stream_handle);
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
        else if (packet->stream_index == audio_stream && audio_stream_handle)
        {
            SDL_PutAudioStreamData(audio_stream_handle, packet->data, static_cast<i32>(packet->size));
        }

        av_packet_unref(packet);
    }

    // Drain remaining frames
    if (!quit)
    {
        avcodec_send_packet(vctx, nullptr);

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
    avformat_close_input(&fmt_ctx);

    if (audio_stream_handle)
        SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(audio_stream_handle));

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    return true;
}

#endif // ARTS_HAVE_FFMPEG
