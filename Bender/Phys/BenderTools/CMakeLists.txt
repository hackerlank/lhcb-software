################################################################################
# Package: BenderTools
################################################################################
gaudi_subdir(BenderTools v29r0)

gaudi_depends_on_subdirs(Phys/Bender
                         Phys/PhysConf)

gaudi_install_python_modules()
gaudi_install_scripts()
gaudi_add_test(QMTest QMTEST)

gaudi_alias(check-trg
            trg-check)
gaudi_alias(dump-dst
            dst-dump)
gaudi_alias(db-tags
            get-dbtags)
gaudi_alias(get-meta-info
            get-metainfo)
gaudi_alias(no-mc-decay
            no-mc-decays)

