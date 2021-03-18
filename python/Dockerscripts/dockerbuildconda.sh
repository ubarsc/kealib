
. /opt/conda/etc/profile.d/conda.sh
conda activate mybuild

cd /home/conda/recipe_root/python

conda build --output-folder=. --python=3.8 condarecipe

