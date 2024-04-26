To run this project you need these modules below:

module load CMake/3.27.6-GCCcore-13.2.0 foss/2022a

Then to schedule jobs on beo cat use this line

sbatch --mem-per-cpu=512M --ntasks-per-node=16 --nodes=1 --constraint=moles ./pt1shell.sh

sbatch --mem-per-cpu=512M --ntasks-per-node=16 --nodes=1 --constraint=moles ./pt2shell.sh

sbatch --mem-per-cpu=512M --ntasks-per-node=16 --nodes=1 --constraint=moles ./pt3shell.sh
