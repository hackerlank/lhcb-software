################################################################################
# Package: CommonMCParticles
################################################################################
gaudi_subdir(CommonMCParticles v1r1)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Tr/TrackTools)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
