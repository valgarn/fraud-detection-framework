
# >>> conda initialize >>>
# !! Contents within this block are managed by 'conda init' !!
__conda_setup="$('/root/anaconda3/bin/conda' 'shell.bash' 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "/root/anaconda3/etc/profile.d/conda.sh" ]; then
        . "/root/anaconda3/etc/profile.d/conda.sh"
    else
        export PATH="/root/anaconda3/bin:$PATH"
    fi
fi
unset __conda_setup
# <<< conda initialize <<<

export PATH=./:/usr/bin:/usr/local/bin:/usr/local/sbin:/root/lib:$PATH

source activate

export LD_LIBRARY_PATH=/root/lib:/usr/lib/x86_64-linux-gnu/:LD_LIBRARY_PATH
ldconfig /root/lib
ldconfig /usr/lib/x86_64-linux-gnu

python --version
echo $PATH

cd /root/srv/
/root/anaconda3/bin/python /root/srv/docker-server.py
