

.PHONY: install
install:
	Rscript -e 'Rcpp::compileAttributes("sdr")'
	R CMD INSTALL sdr
.PHONY: test
test: install
	Rscript sdr/tests/tinytest.R

.PHONY: coverage
coverage: install
	Rscript -e 'covr::report(covr::package_coverage("sdr"), file = "sdr_coverage.html")'



## Singularity container for LEO5
build:
	sudo /opt/singularity/bin/singularity build --sandbox ssdr singularity_sdr.def

shell:
	sudo /opt/singularity/bin/singularity shell --bind /home/retos/Documents/2023_Annex_files:/scratch --writable ssdr

# Must run build which creates the ssdr folder,
# this builds the ssdr.sif target ('container')
sif:
	sudo /opt/singularity/bin/singularity build ssdr.sif ssdr

clean:
	-rm slurm-*

##export SINGULARITY_BIND="/scratch/c4031021/foehnreconstruction_git:/foehn"
##cd /foehn && \

.PHONY: run
run:
	./run
	singularity exec ssdr.sif Rscript test.R


