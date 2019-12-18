# ./build/test/tests
bash $HOME/miniconda/etc/profile.d/conda.sh
export PATH=$HOME/miniconda/bin/:$PATH
source activate thermohubclient
python -c "import thermohubclient"