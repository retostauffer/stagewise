#!/bin/bash

#SBATCH --job-name=retoA
#SBATCH --cpus-per-task=1
#SBATCH --mail-user=Reto.Stauffer@uibk.ac.at
#SBATCH --mail-type=END,FAIL
#SBATCH --output=slurm-%x-%j_%a.out
#SBATCH --error=slurm-%x-%j_%a.err
#SBATCH --time=0:05:00
#SBATCH --mem-per-cpu=2G

# Must be started outside this folder! Checking if the
# singulariy container is there.
SIF="ssdr.sif"
if [ ! -f ${SIF} ] ; then
	printf "Cannot find singularity container \"%s\"" "${SIF}"
	exit 667
fi

# Loading module
module load singularityce/3.10.3-python-3.10.8-gcc-8.5.0-fzp3had

# Bind directory
export SINGULARITY_BIND="/home/c403/c4031021/stagewise:/stagewise"

# Call singularity
singularity exec ${SIF} /bin/bash <<-EOF
cd /stagewise && \
Rscript reto_test.R -b 5 --maxit 100 --ff
EOF

