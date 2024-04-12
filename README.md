
module load CMake/3.27.6-GCCcore-13.2.0 foss/2022a

sbatch --time=1 --mem-per-cpu=512M --cpus-per-task=1 --ntasks=1 --nodes=1 ./pt1shell.sh

sbatch --time=1 --mem-per-cpu=512M --cpus-per-task=1 --ntasks=8 --nodes=1 --constraint=moles ./pt1shell.sh


