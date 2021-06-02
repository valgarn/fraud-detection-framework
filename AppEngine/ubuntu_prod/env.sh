
export PATH=./:/opt/conda/envs/env/bin:/opt/conda/envs/env/lib:/usr/bin:/usr/local/bin:/usr/local/sbin:/root/lib:$PATH

source activate env

export LD_LIBRARY_PATH=/root/lib:/usr/lib/x86_64-linux-gnu:/opt/conda/envs/env/lib:LD_LIBRARY_PATH
ldconfig /root/lib
ldconfig /usr/lib/x86_64-linux-gnu
ldconfig /opt/conda/envs/env/lib

python --version
echo $PATH

cd /root/srv/
/opt/conda/envs/env/bin/python /root/srv/docker-server.py
