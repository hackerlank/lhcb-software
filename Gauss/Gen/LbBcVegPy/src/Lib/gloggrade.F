C $Id: gloggrade.F,v 1.2 2009-07-23 19:45:37 jhe Exp $
      INTEGER*4 FUNCTION LOGGRADE_ADDRESS() 
      IMPLICIT NONE

      INTEGER IEVNTDIS,IGENERATE,IVEGASOPEN,IGRADE,IUSECURDIR

      COMMON/LOGGRADE/IEVNTDIS,IGENERATE,IVEGASOPEN,IGRADE,IUSECURDIR

      INTEGER*4 GETADDR

      LOGGRADE_ADDRESS = GETADDR(IEVNTDIS)
      RETURN
      END
