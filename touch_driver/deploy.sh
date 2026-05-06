#!/bin/bash

# 定义变量
KO_FILE="gt9147.ko"
REMOTE_USER="root"
REMOTE_HOST="192.168.1.4"
REMOTE_PATH="/home/root"

# 检查文件是否存在
if [ ! -f "$KO_FILE" ]; then
    echo "错误: 文件 $KO_FILE 不存在!"
    exit 1
fi

# 显示传输信息
echo "正在将 $KO_FILE 传输到 ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_PATH}..."

# 执行 scp 传输
scp "$KO_FILE" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_PATH}/"

# 检查传输结果
if [ $? -eq 0 ]; then
    echo "传输成功!"
    echo "文件已保存到: ${REMOTE_PATH}/${KO_FILE}"
else
    echo "传输失败!"
    exit 1
fi