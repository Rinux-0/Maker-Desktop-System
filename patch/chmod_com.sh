#!zsh

# 指定目标目录
target_dir="$HOME/.wine/dosdevices"

# 检查目录是否存在
cd "$target_dir" || exit 1

# 查找目标目录下的文件和目录，名称匹配com+数字且无其他字符
find "./" -maxdepth 1 -regextype posix-extended -regex ".*/com[0-9]+$" |
while read -r f_path; do
    f_name=$(basename "$f_path")
    f_com_num="${f_name#com}"

    if [[ $f_com_num=~^[0-9]+$ ]]; then
        if ((10#$f_com_num > 0)); then
            chmod 777 "$f_path"
            echo "成功 篡权上位: $f_path"
        fi
    fi
done
