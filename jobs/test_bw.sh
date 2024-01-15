#!/bin/bash

#SBATCH --job-name=testBW
#SBATCH --cpus-per-task=1
#SBATCH --mail-user=Reto.Stauffer@uibk.ac.at
#SBATCH --mail-type=END,FAIL
#SBATCH --output=slurm-%x-%j_%a.out
#SBATCH --error=slurm-%x-%j_%a.err
#SBATCH --time=0:05:00
#SBATCH --mem-per-cpu=4G

if [ !$# -eq 1 ] ; then
    printf "There must be one input argument (number of batches)"
    exit 3
fi
BATCHES=$1
printf " ======== BATCHES:  %d" ${BATCHES}

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
Rscript test_bw.R --batches ${BATCHES}
EOF

