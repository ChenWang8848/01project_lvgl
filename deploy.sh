#!/bin/bash

# 定义变量
EXEC_FILE="build/digitpic"
REMOTE_USER="root"
REMOTE_HOST="192.168.1.4"
REMOTE_PATH="/home/root"

# 提取纯文件名（去掉路径）
EXEC_NAME=$(basename "$EXEC_FILE")

# 检查可执行文件是否存在
if [ ! -f "$EXEC_FILE" ]; then
    echo "错误: 可执行文件 $EXEC_FILE 不存在!"
    echo "请先执行 'make' 命令进行编译"
    exit 1
fi

# 检查文件是否具有执行权限
if [ ! -x "$EXEC_FILE" ]; then
    echo "警告: 文件 $EXEC_FILE 没有执行权限，正在添加..."
    chmod +x "$EXEC_FILE"
fi

# 显示传输信息
echo "=========================================="
echo "LVGL 部署脚本"
echo "=========================================="
echo "源文件: $EXEC_FILE"
echo "目标: ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_PATH}/${EXEC_NAME}"
echo "=========================================="
echo ""

# 执行 scp 传输
echo "[1/2] 正在传输可执行文件..."
scp "$EXEC_FILE" "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_PATH}/${EXEC_NAME}"

# 检查传输结果
if [ $? -ne 0 ]; then
    echo "错误: 文件传输失败!"
    exit 1
fi

echo "✓ 文件传输成功!"
echo ""

# 通过 SSH 设置执行权限并验证
echo "[2/2] 正在设置远程文件权限并验证..."
ssh ${REMOTE_USER}@${REMOTE_HOST} "chmod +x ${REMOTE_PATH}/${EXEC_NAME} && ls -lh ${REMOTE_PATH}/${EXEC_NAME}"

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "部署完成!"
    echo "=========================================="
    echo "您可以在开发板上执行以下命令运行程序:"
    echo "  cd ${REMOTE_PATH}"
    echo "  ./${EXEC_NAME}"
    echo "=========================================="
else
    echo "警告: 无法验证远程文件，但传输可能已成功"
fi