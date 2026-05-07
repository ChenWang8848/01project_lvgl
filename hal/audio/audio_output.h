/**
 * @file    audio_output.h
 * @brief   音频输出虚基类 — 定义 ALSA/OSS 等音频后端的统一接口
 *
 * 设计模式: 函数指针表 (vtable) 实现运行时多态。
 * 子类 (alsa_output_t) 填充具体的 ALSA PCM 操作。
 */

#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>

typedef struct audio_output_s audio_output_t;

struct audio_output_s {
    const char *name;           /**< 驱动名称 ("alsa" / "oss") */

    /**
     * @brief 打开音频设备并配置参数
     * @param sample_rate 采样率 (Hz), e.g. 44100
     * @param channels    声道数 (1=单声道, 2=立体声)
     * @param bits        量化位宽 (8/16/24/32)
     * @return 0: 成功, <0: 失败
     */
    int  (*init)(audio_output_t *self, uint32_t sample_rate,
                 uint8_t channels, uint8_t bits);

    /**
     * @brief 写入 PCM 音频帧
     * @param buf     PCM 数据缓冲区
     * @param frames  帧数
     * @return 实际写入的帧数, <0: 错误
     */
    int  (*write)(audio_output_t *self, const void *buf, uint32_t frames);

    /**
     * @brief 排空音频缓冲区 (drain) — 等待所有待播放数据输出完毕
     */
    int  (*drain)(audio_output_t *self);

    /** @brief 关闭设备, 释放资源 */
    void (*deinit)(audio_output_t *self);
};

#endif
